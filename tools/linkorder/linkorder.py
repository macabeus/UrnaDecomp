"""Invert binaryen's function-order scramble: recover each function's link ordinal.

The order law (docs/reference/facts/link-order.md §L1, §L2): emcc -Oz's wasm-metadce
re-reads the post-`wasm-opt -Oz` module without a name section, so every function is named
by its defined index k (= wasm-ld link order compacted after -Oz inlining/merging, with the
merge-similar-functions thunks appended at the end), and sorts by (use count desc,
strcmp(str(k)) desc).  Inside a use-count band the strings therefore descend; across bands
nothing orders.  The ordinals are a permutation of 0..M-1, M = N + holes, the holes being
metadce's own removals (2 in the skeleton: ntohs, htonl).

What the string order does to a band: ordinals with the same digit count descend numerically;
a 3-digit c sits right after the 4-digit 10c ("425" after "4250"), a 4-digit a inside the
5-digit zone right after 10a, and so on ("marker slots").  rank(x) = position of str(x) in
the descending strcmp order of 0..M-1; band order is "strictly increasing rank".

Anchor blocks: an object whose target functions are identified and whose emission order is
known occupies a contiguous ordinal range in that order (wasm-ld numbers an input file's live
functions in order; -Oz only deletes inside a block): member i has ordinal B + o_i.  With
--offsets the offsets come from a reference build's live function list, and runs of adjacent
offsets form rigid CHUNKS chained by loose links (members that exist but are not identified).

Solver: every chunk keeps the set of feasible bases B, shrunk to a fixpoint by
  * the band-adjacency test: two chunk members consecutive among the chunk's members of one
    band with k functions between them need rank(B+o_g) - rank(B+o_f) >= k+1 (the markers
    pin B modulo 10, 100, 1000 when they show);
  * the order-count bounds: a member's rank is at least the number of anchored functions of
    the blocks certainly above it (band adjacency DAG) and at most M-1 minus those below;
  * chunk chains: the next chunk of an object starts span .. span+(gap-1)*(1+slack)+2 ordinals
    after the previous chunk's base;
  * the cut equations: at rank r, the functions of every band certainly above r (positions of
    the anchored members whose whole window is above r) plus the member's own band position
    may not exceed r, and r may not exceed the functions possibly above r (+ holes).
Output: per function the ordinal hull of its rank window, a point estimate (window middle)
and, for anchored functions, block and offset.  Windows are sound when the anchors are right.

usage:
  python3 -I tools/linkorder/linkorder.py solve MODULE.wasm [--anchors anchors.tsv]... [--offsets | --positions]
          [--top mgfn.txt --top-max C] [--holes H] [--slack S] [--truth truth.tsv] -o ordinals.tsv [--blocks blocks.tsv]
  anchors.tsv: index<TAB>block<TAB>pos   (pos = position in the object's function list, or
               with --offsets the offset among the object's live functions in a reference build;
               with --positions the position in the object's FULL function list (.o order): adjacent
               positions form rigid chunks and a gap of g positions bounds the next chunk exactly, span .. span+g-1)
  --top:       function indices of merge-similar-functions bodies (binaryen appends them after every linked
               function): confined to the top --top-max ordinals, every other chunk ends below them
  truth.tsv:   index<TAB>ordinal         (evaluation only)
"""
import sys, os, argparse, collections, bisect, time
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmcode import Module, uses


def string_rank(M):
    """rank_of[x] = position of str(x) in the DESCENDING strcmp order of 0..M-1; ord_of_rank its inverse."""
    ord_of_rank = np.array(sorted(range(M), key=lambda x: str(x).encode(), reverse=True), dtype=np.int64)
    rank_of = np.empty(M, dtype=np.int64); rank_of[ord_of_rank] = np.arange(M)
    return rank_of, ord_of_rank


class Solver:
    def __init__(self, module_path, holes=2, log=print, exact_chain=False):
        self.log = log
        self.exact_chain = exact_chain      # anchor pos = position in the object's FULL function list: chain gaps are exact
        self.robust = False                 # second-order band envelopes (see band_eval): sound but far looser; experimental
        self.top_chunks = set()             # chunks confined to the top `top_max` ordinals (merge-similar-functions bodies)
        self.top_max = 0
        m = self.m = Module(module_path)
        self.u = uses(m)
        self.funcs = [f for f, _, _ in m.code]          # module order = (uses desc, str(k) desc)
        self.N = len(self.funcs)
        self.M = self.N + holes
        self.rank_of, self.ord_of_rank = string_rank(self.M)
        self.row = {f: i for i, f in enumerate(self.funcs)}
        self.bands = []; self.band_of = np.zeros(self.N, dtype=np.int64); self.pos_in_band = np.zeros(self.N, dtype=np.int64)
        cur = []; last = None
        for i, f in enumerate(self.funcs):
            if self.u[f] != last and cur: self.bands.append(cur); cur = []
            cur.append(i); last = self.u[f]
        if cur: self.bands.append(cur)
        for b, band in enumerate(self.bands):
            for p, i in enumerate(band): self.band_of[i] = b; self.pos_in_band[i] = p
        self.blocks = {}
        self.blk_of_row = {}

    # ---- anchors -----------------------------------------------------------------------------------
    def _make_block(self, name, members, parent, prev, gap):
        inband = collections.defaultdict(list)
        for r, o in members: inband[int(self.band_of[r])].append((int(self.pos_in_band[r]), r, o))
        pairs = []
        for band, lst2 in inband.items():
            lst2.sort()
            for (p1, r1, o1), (p2, r2, o2) in zip(lst2, lst2[1:]): pairs.append((o1, o2, p2 - p1 - 1))
        self.blocks[name] = dict(members=members, pairs=pairs, L=members[-1][1] + 1, n=len(members),
                                 parent=parent, prev=prev, gap=gap)
        for r, o in members: self.blk_of_row[r] = (name, o)

    def add_anchors(self, anchors, offsets=False):
        """anchors: (func_index, block, pos). offsets=False: members get offsets 0..n-1 in pos order (complete
        identification).  offsets=True: pos is the offset among the object's live functions; runs of adjacent
        offsets become rigid chunks `block#k` chained to the previous chunk."""
        byb = collections.defaultdict(list)
        self.abs_pos = getattr(self, 'abs_pos', {})
        for f, b, p in anchors:
            if f in self.row: byb[b].append((self.row[f], p)); self.abs_pos[self.row[f]] = p
        for b, lst in byb.items():
            lst.sort(key=lambda t: t[1])
            if offsets:
                chunks = []; cur = [lst[0]]
                for prev, nxt in zip(lst, lst[1:]):
                    if nxt[1] - prev[1] > 1: chunks.append(cur); cur = []
                    cur.append(nxt)
                chunks.append(cur)
            else:
                chunks = [[(r, o) for o, (r, p) in enumerate(lst)]]
            prev_name = None
            span = lst[-1][1] - lst[0][1] + 1
            for ci, ch in enumerate(chunks):
                name = b if len(chunks) == 1 else f'{b}#{ci}'
                base = ch[0][1]
                gap = (ch[0][1] - chunks[ci - 1][-1][1]) if ci else 0
                self._make_block(name, [(r, p - base) for r, p in ch], b, prev_name, gap)
                self.blocks[name]['obj_span'] = span
                prev_name = name

    def add_top(self, indices, top_max):
        """Functions created by binaryen's merge-similar-functions (byn$mgfn-shared$*): appended after every linked
        function, so their ordinals are the highest ones. Each becomes a one-member chunk confined to [M-top_max, M-1]."""
        self.top_max = top_max
        for k, f in enumerate(indices):
            if f not in self.row or self.row[f] in self.blk_of_row: continue
            name = f'<binaryen>#{k}'
            self._make_block(name, [(self.row[f], 0)], '<binaryen>', None, 0)
            self.top_chunks.add(name)

    def check_anchors_zones(self, log=None):
        """Positions mode: in one band the members of an object appear in DESCENDING position order inside one digit
        zone; an object straddling a zone boundary shows at most two such runs, the later one (band order) holding the
        higher positions. Members outside the best such pattern are dropped (misidentified functions)."""
        byobj = collections.defaultdict(list)
        for n, blk in self.blocks.items():
            for r, o in blk['members']: byobj[blk['parent']].append((r, blk['gap0'] + o if 'gap0' in blk else None, n))
        dropped = []
        for obj, mem in byobj.items():
            inband = collections.defaultdict(list)
            for r, _, n in mem: inband[int(self.band_of[r])].append((int(self.pos_in_band[r]), self.abs_pos[r], r))
            for b, lst in inband.items():
                lst.sort(); poss = [p for _, p, _ in lst]
                keep = self._lds(poss)
                rest = [i for i in range(len(poss)) if i not in keep]
                if rest:
                    # a second descending run, later in the band and entirely above the first run, is a zone straddle
                    keep2 = self._lds([poss[i] for i in rest]); keep2 = {rest[j] for j in keep2}
                    lo_first = min(poss[i] for i in keep); last_first = max(keep)
                    if keep2 and min(keep2) > last_first and min(poss[i] for i in keep2) > max(poss[i] for i in keep): keep |= keep2
                for i, (p, pos, r) in enumerate(lst):
                    if i not in keep: dropped.append(r)
        for r in set(dropped):
            n, o = self.blk_of_row.pop(r)
            self.blocks[n]['members'] = [(rr, oo) for rr, oo in self.blocks[n]['members'] if rr != r]
        # rebuild the affected chunks (members renumbered from their first offset; chain gaps follow)
        rebuilt = 0
        for n in list(self.blocks):
            blk = self.blocks[n]
            if not blk['members']: del self.blocks[n]; rebuilt += 1; continue
            if blk['n'] != len(blk['members']):
                base = blk['members'][0][1]
                self._make_block(n, [(r, o - base) for r, o in blk['members']], blk['parent'], blk['prev'], blk['gap'] + base)
                self.blocks[n]['obj_span'] = blk['obj_span']; rebuilt += 1
        for n, blk in self.blocks.items():
            if blk['prev'] and blk['prev'] not in self.blocks: blk['prev'] = None
        if log: log(f'zone-aware anchor check: {len(set(dropped))} anchors contradict their object\'s order in a band and were dropped; {rebuilt} chunks rebuilt, {len(self.blocks)} chunks left')
        return len(set(dropped))

    @staticmethod
    def _lds(vals):
        """indices of a longest strictly decreasing subsequence"""
        tails = []; tidx = []; prev = [-1] * len(vals)
        for i, v in enumerate(vals):
            k = bisect.bisect_left([-t for t in tails], -v)
            if k == len(tails): tails.append(v); tidx.append(i)
            else: tails[k] = v; tidx[k] = i
            prev[i] = tidx[k - 1] if k > 0 else -1
        keep = set(); i = tidx[-1] if tidx else -1
        while i != -1: keep.add(i); i = prev[i]
        return keep

    def clean_anchors(self, log=None):
        """Drop anchors that contradict their own chunk: in a band the chunk's members must appear in
        descending offset order; members outside the longest such subsequence are removed."""
        dropped = 0
        for n, blk in list(self.blocks.items()):
            inband = collections.defaultdict(list)
            for r, o in blk['members']: inband[int(self.band_of[r])].append((int(self.pos_in_band[r]), o, r))
            bad = set()
            for b, lst in inband.items():
                lst.sort(); offs = [o for p, o, r in lst]
                tails = []; prev = [-1] * len(offs); tidx = []
                for i, o in enumerate(offs):                      # longest strictly decreasing subsequence
                    k = bisect.bisect_left([-t for t in tails], -o)
                    if k == len(tails): tails.append(o); tidx.append(i)
                    else: tails[k] = o; tidx[k] = i
                    prev[i] = tidx[k - 1] if k > 0 else -1
                keep = set(); i = tidx[-1] if tidx else -1
                while i != -1: keep.add(i); i = prev[i]
                for i, (p, o, r) in enumerate(lst):
                    if i not in keep: bad.add(r)
            if bad:
                dropped += len(bad)
                for r in bad: self.blk_of_row.pop(r, None)
                members = [(r, o) for r, o in blk['members'] if r not in bad]
                if not members: del self.blocks[n]; continue
                base = members[0][1]
                self._make_block(n, [(r, o - base) for r, o in members], blk['parent'], blk['prev'], blk['gap'] + base)
                self.blocks[n]['obj_span'] = blk['obj_span']
        for n, blk in self.blocks.items():                         # chains over deleted chunks are cut
            if blk['prev'] and blk['prev'] not in self.blocks: blk['prev'] = None
        if log: log(f"anchor cleaning: {dropped} anchors contradicted their chunk's order and were dropped; {len(self.blocks)} chunks left")
        return dropped

    def base_candidates(self, name):
        """bool over B in [0, M-L]: bases for which every band-adjacency pair of the chunk fits (and, for every
        ordinary chunk, the chunk ends below the top block; a top chunk lies inside it)."""
        blk = self.blocks[name]; nB = self.M - blk['L'] + 1
        Bs = np.arange(nB); ok = np.ones(nB, dtype=bool)
        for of, og, k in blk['pairs']:
            ok &= (self.rank_of[Bs + og] - self.rank_of[Bs + of]) >= k + 1
        if self.top_max:
            if name in self.top_chunks: ok &= Bs >= self.M - self.top_max
            else: ok &= Bs + blk['L'] - 1 < self.M - len(self.top_chunks)
        return ok

    @staticmethod
    def residues(ok):
        feas = np.where(ok)[0]; out = {}
        if len(feas) == 0: return out
        for mod in (10, 100, 1000):
            vals, cnt = np.unique(feas % mod, return_counts=True)
            if cnt.max() >= 0.9 * len(feas) and len(feas) >= 2 * mod // 10: out[mod] = int(vals[cnt.argmax()])
        return out

    # ---- block order ---------------------------------------------------------------------------------
    def block_order(self):
        """Linear extension of the band-adjacency DAG (weighted edges; cycles from misidentified anchors or
        interleaved zones are broken at the block with the largest out-in weight). Returns (order, succ, reach, tied)."""
        w = collections.Counter()
        for band in self.bands:
            prev = None
            for r in band:
                if r in self.blk_of_row:
                    cur = self.blk_of_row[r][0]
                    if prev is not None and prev != cur: w[(prev, cur)] += 1
                    prev = cur
        names = list(self.blocks)
        succ = collections.defaultdict(set); pred = collections.defaultdict(set)
        for (a, b), c in w.items(): succ[a].add(b); pred[b].add(a)
        remaining = set(names); order = []; dropped = []
        indeg = {n: sum(w[(m, n)] for m in pred[n]) for n in names}
        outdeg = {n: sum(w[(n, m)] for m in succ[n]) for n in names}
        while remaining:
            ready = [n for n in remaining if indeg[n] == 0]
            if not ready:
                n = max(remaining, key=lambda x: (outdeg[x] - indeg[x], self.blocks[x]['n']))
                for m in pred[n]:
                    if m in remaining and w[(m, n)]: dropped.append((m, n, w[(m, n)])); outdeg[m] -= w[(m, n)]
                indeg[n] = 0; ready = [n]
            ready.sort(key=lambda x: (-self.blocks[x]['n'], x))
            n = ready[0]; order.append(n); remaining.discard(n)
            for m in succ[n]:
                if m in remaining: indeg[m] -= w[(n, m)]
        pos = {n: i for i, n in enumerate(order)}
        fsucc = {n: {m for m in succ[n] if pos[m] > pos[n]} for n in names}
        reach = {}
        for n in reversed(order):
            s = set(fsucc[n])
            for m in fsucc[n]: s |= reach[m]
            reach[n] = s
        tied = {n: (i > 0 and n not in reach[order[i - 1]]) for i, n in enumerate(order)}
        if dropped: self.log(f'block order: {len(dropped)} adjacency edges dropped to break cycles (weight {sum(c for *_, c in dropped)})')
        return order, fsucc, reach, tied

    # ---- propagation ---------------------------------------------------------------------------------
    def member_rank_intervals(self, cand):
        iv = {}
        for n, blk in self.blocks.items():
            if n in getattr(self, 'suspect', set()): continue
            feas = np.where(cand[n])[0]
            if len(feas) == 0: continue
            for r, o in blk['members']:
                rk = self.rank_of[feas + o]; iv[r] = (int(rk.min()), int(rk.max()))
        return iv

    def band_structs(self, iv, bands=None, exclude=None):
        """Per band (all, or the given indices): arrays over rank r of LB(r) = 1 + max position among anchored
        members certainly above r (rk_max < r) and UB(r) = min position among members certainly below r
        (rk_min > r), else len(band). Members of chunk `exclude` are left out."""
        R = np.arange(self.M); out = {}
        self.band_sorted(iv)
        for bi in (range(len(self.bands)) if bands is None else bands):
            lb, ub = self.band_eval(bi, R, exclude)
            out[bi] = (lb, ub)
        return out

    def band_sorted(self, iv):
        """Per band, the anchored members sorted by rk_max and by rk_min (numpy), once per propagation iteration."""
        self._bs = {}
        for bi, band in enumerate(self.bands):
            rows = np.array([r for r in band if r in iv], dtype=np.int64)
            if len(rows) == 0: self._bs[bi] = None; continue
            pos = np.array([int(self.pos_in_band[r]) for r in rows], dtype=np.int64)
            rmax = np.array([iv[int(r)][1] for r in rows], dtype=np.int64); rmin = np.array([iv[int(r)][0] for r in rows], dtype=np.int64)
            ox = np.lexsort((pos, rmax)); on = np.lexsort((pos, rmin))
            self._bs[bi] = dict(mx_key=rmax[ox], mx_pos=pos[ox], mx_row=rows[ox], mn_key=rmin[on], mn_pos=pos[on], mn_row=rows[on])

    def band_eval(self, bi, R, exclude=None):
        """LB and UB of band bi at the ranks R (array), leaving out the members of chunk `exclude`."""
        st = self._bs[bi]; band = self.bands[bi]
        if st is None: return np.zeros(len(R), dtype=np.int64), np.full(len(R), len(band), dtype=np.int64)
        mx_pos = st['mx_pos']; mn_pos = st['mn_pos']
        if exclude is not None:
            ex = np.array([r for r, o in self.blocks[exclude]['members']], dtype=np.int64)
            mask_x = np.isin(st['mx_row'], ex); mask_n = np.isin(st['mn_row'], ex)
            if mask_x.any(): mx_pos = np.where(mask_x, -1, mx_pos)
            if mask_n.any(): mn_pos = np.where(mask_n, len(band), mn_pos)
        # robust envelopes: the SECOND largest position certainly above r (and second smallest below): a single wrong
        # anchor with a tight wrong window would otherwise move the count by its whole band position, not by one
        pm = self._second_max_prefix(mx_pos) if self.robust else np.maximum.accumulate(mx_pos)
        k = np.searchsorted(st['mx_key'], R, side='left')
        lb = np.where(k > 0, pm[np.maximum(k - 1, 0)] + 1, 0)
        rev = mn_pos[::-1]
        sm = (len(band) - 1 - self._second_max_prefix(len(band) - 1 - rev))[::-1] if self.robust else np.minimum.accumulate(rev)[::-1]
        k2 = np.searchsorted(st['mn_key'], R, side='right')
        ub = np.where(k2 < len(sm), sm[np.minimum(k2, len(sm) - 1)], len(band))
        return lb, ub

    @staticmethod
    def _second_max_prefix(a):
        """second[i] = second largest value among a[:i+1] (-1 when fewer than two)."""
        a = np.asarray(a, dtype=np.int64)
        if len(a) == 0: return a
        pm = np.maximum.accumulate(a)
        prev_pm = np.concatenate(([-1], pm[:-1]))
        record = a > prev_pm                     # a new maximum: the previous maximum becomes the second
        c = np.where(record, prev_pm, a)
        c = np.where(a == pm, c, a)              # a repeat of the maximum counts as a second copy
        return np.maximum.accumulate(c)

    def chain_filter(self, cand, slack):
        """Consecutive chunks of one object: the next chunk's base lies span .. span+(gap-1)*(1+slack)+2 above the
        previous chunk's base (both directions, by dilation of the feasible sets)."""
        changed = 0; self.chain_inconsistent = 0; self.chain_bad = []
        suspect = getattr(self, 'suspect', set())
        for n, blk in self.blocks.items():
            pv = blk.get('prev')
            if not pv or pv not in cand or n not in cand or pv in suspect or n in suspect: continue
            span = self.blocks[pv]['L']; g = blk['gap']
            if self.exact_chain:
                # pos = position in the object's full function list: between the two chunks lie gap-1 object
                # functions of which 0 .. gap-1 are live, so the next base is span .. span+gap-1 above (exact, sound)
                dmin = span; dmax = span + g - 1
            else:
                dmin = span; dmax = span + int((g - 1) * (1 + slack)) + 2
            c_prev = cand[pv]; c_cur = cand[n]
            cp = np.concatenate(([0], np.cumsum(c_prev)))
            Bc = np.arange(len(c_cur)); a = np.clip(Bc - dmax, 0, len(c_prev)); b2 = np.clip(Bc - dmin + 1, 0, len(c_prev))
            ok_cur = (cp[b2] - cp[a]) > 0
            cc = np.concatenate(([0], np.cumsum(c_cur)))
            Bp = np.arange(len(c_prev)); a2 = np.clip(Bp + dmin, 0, len(c_cur)); b3 = np.clip(Bp + dmax + 1, 0, len(c_cur))
            ok_prev = (cc[b3] - cc[a2]) > 0
            n1 = c_cur & ok_cur; n2 = c_prev & ok_prev
            if not n1.any() or not n2.any():
                if self.exact_chain:
                    # the link cannot hold: an anchor of this object is wrong; keep both sets, report
                    self.chain_inconsistent += 1; self.chain_bad.append((pv, n)); continue
                # inconsistent link (more insertions than the slack allows): widen it instead of dropping it
                dmax2 = span + 4 * (dmax - span) + 20
                a = np.clip(Bc - dmax2, 0, len(c_prev)); ok_cur = (cp[b2] - cp[a]) > 0
                b3 = np.clip(Bp + dmax2 + 1, 0, len(c_cur)); ok_prev = (cc[b3] - cc[a2]) > 0
                n1 = c_cur & ok_cur; n2 = c_prev & ok_prev
                if not n1.any() or not n2.any(): continue
            if n1.sum() != c_cur.sum(): cand[n] = n1; changed += 1
            if n2.sum() != c_prev.sum(): cand[pv] = n2; changed += 1
        return changed

    def object_window(self, cand, factor=2.0, tight=3):
        """Chunks of one object lie within factor*span of the object's tightly placed chunks (<= `tight` bases)."""
        byobj = collections.defaultdict(list)
        for n, blk in self.blocks.items(): byobj[blk['parent']].append(n)
        changed = 0
        for obj, names in byobj.items():
            if len(names) < 2: continue
            anchors = [n for n in names if 0 < cand[n].sum() <= tight]
            if not anchors: continue
            lo = min(int(np.where(cand[n])[0][0]) for n in anchors); hi = max(int(np.where(cand[n])[0][-1]) for n in anchors)
            w = int(factor * self.blocks[names[0]]['obj_span']) + 50
            for n in names:
                if n in anchors: continue
                c = cand[n].copy(); c[:max(0, lo - w)] = False; c[hi + w + 1:] = False
                if c.any() and c.sum() != cand[n].sum(): cand[n] = c; changed += 1
        return changed

    def order_count_bounds(self, cand, reach):
        """Sound rank bounds from the block order: a member's rank is at least the number of anchored functions
        of the blocks certainly above it (plus its chunk-mates above) and at most M-1 minus those below."""
        above_blk = collections.defaultdict(int)
        for m, rs in reach.items():
            for n in rs: above_blk[n] += self.blocks[m]['n']
        for n, blk in self.blocks.items():
            below_blk = sum(self.blocks[m]['n'] for m in reach.get(n, ()))
            Bs = np.arange(len(cand[n])); ok = cand[n].copy(); mem = blk['members']
            for k, (r, o) in enumerate(mem):
                rk = self.rank_of[Bs + o]
                ok &= (rk >= above_blk[n] + (len(mem) - 1 - k)) & (rk <= self.M - 1 - below_blk - k)
            if ok.any(): cand[n] = ok
        return cand

    def propagate(self, cand, holes, slack=0.5, log=None, max_iters=50, tolerance=0):
        """Shrink every chunk's base set (chains + cut equations) until nothing changes. In exact-chain mode a chunk of an
        inconsistent link is QUARANTINED: both chunks go back to their initial base sets, leave the count structures and
        constrain nothing further (a misidentified member would otherwise poison every tight cut equation). `tolerance`
        loosens the cut equations by that many functions on each side (wrong anchors that no chain catches)."""
        cand = {n: c.copy() for n, c in cand.items()}; R_all = np.arange(self.M)
        initial = {n: c.copy() for n, c in cand.items()}; self.suspect = set()
        for it in range(max_iters):
            changed = self.chain_filter(cand, slack) + (0 if self.exact_chain else self.object_window(cand))
            for pv, n in getattr(self, 'chain_bad', []):
                for x in (pv, n):
                    if x not in self.suspect: self.suspect.add(x); cand[x] = initial[x].copy(); changed += 1
            iv = self.member_rank_intervals(cand)
            structs = self.band_structs(iv)
            S_LB = sum(lb for lb, ub in structs.values()); S_UB = sum(ub for lb, ub in structs.values())
            for n, blk in self.blocks.items():
                if n in self.suspect: continue
                feas = np.where(cand[n])[0]
                if len(feas) <= 1: continue
                own_bands = sorted(set(int(self.band_of[r]) for r, o in blk['members']))
                own = {b: [] for b in own_bands}
                for r, o in blk['members']: own[int(self.band_of[r])].append((o, int(self.pos_in_band[r])))
                ok = np.ones(len(feas), dtype=bool)
                sub_cache = {}
                for r, o in blk['members']:
                    rk = self.rank_of[feas + o]
                    b = int(self.band_of[r]); p = int(self.pos_in_band[r])
                    lo_cnt = S_LB[rk].copy(); hi_cnt = S_UB[rk].copy()
                    for b2 in own_bands:
                        lb_all, ub_all = structs[b2]
                        if b2 == b:
                            lo_cnt += p - lb_all[rk]; hi_cnt += p - ub_all[rk]
                        else:
                            if b2 not in sub_cache: sub_cache[b2] = self.band_eval(b2, R_all, exclude=n)
                            lbx, ubx = sub_cache[b2]
                            above = [pp for oo, pp in own[b2] if oo > o]; below = [pp for oo, pp in own[b2] if oo < o]
                            lb_o = (max(above) + 1) if above else 0; ub_o = min(below) if below else len(self.bands[b2])
                            lo_cnt += np.maximum(lbx[rk], lb_o) - lb_all[rk]
                            hi_cnt += np.minimum(ubx[rk], ub_o) - ub_all[rk]
                    ok &= (lo_cnt - tolerance <= rk) & (rk <= hi_cnt + holes + tolerance)
                if not ok.all():
                    if not ok.any(): continue                   # inconsistent anchors: keep the set, report later
                    cand[n][:] = False; cand[n][feas[ok]] = True; changed += 1
            if log and (it % 5 == 0 or not changed):
                w = [int(c.sum()) for c in cand.values()]
                log(f'  propagate iter {it}: chunks changed {changed}, median feasible bases {np.median(w):.0f}, max {max(w)}, decided {sum(1 for x in w if x == 1)}, quarantined chunks {len(self.suspect)}', flush=True)
            if not changed: break
        return cand

    def windows_from(self, mlo, mhi):
        """rank windows for every row: anchored rows keep theirs; the others lie between the anchored neighbours'
        windows in their band, shifted by how many unanchored functions sit between."""
        lo = np.zeros(self.N, dtype=np.int64); hi = np.full(self.N, self.M - 1, dtype=np.int64)
        for band in self.bands:
            prev_lo = -1; pend = []
            for r in band:
                if r in mlo:
                    lo[r] = mlo[r]; hi[r] = mhi[r]
                    # j-th pending function: above the previous anchored one (rank > its LOWEST possible rank) and
                    # below this one (rank < its HIGHEST possible rank): sound even when anchored windows overlap
                    for j, q in enumerate(pend):
                        lo[q] = max(lo[q], prev_lo + 1 + j); hi[q] = min(hi[q], mhi[r] - (len(pend) - j))
                    pend = []; prev_lo = mlo[r]
                else: pend.append(r)
            for j, q in enumerate(pend):
                lo[q] = max(lo[q], prev_lo + 1 + j); hi[q] = min(hi[q], self.M - 1 - (len(pend) - 1 - j))
        bad = int((lo > hi).sum())
        if bad: self.log(f'windows_from: {bad} unanchored windows empty (anchored windows inconsistent): widened to the full range')
        lo = np.where(lo > hi, 0, lo); hi = np.where(lo > hi, self.M - 1, hi)
        return lo, hi


def read_anchors(path):
    out = []
    for l in open(path):
        if l.startswith('#') or l.startswith('index'): continue
        p = l.rstrip('\n').split('\t')
        if len(p) < 3 or not p[0].isdigit(): continue
        out.append((int(p[0]), p[1], int(p[2])))
    return out


def _solve_once(a, anchors):
    t0 = time.time()
    s = Solver(a.module, holes=a.holes, exact_chain=a.positions); s.robust = a.robust
    print(f'{s.N} functions, {len(s.bands)} bands, M={s.M}, robust envelopes {s.robust}', flush=True)
    s.add_anchors(anchors, offsets=a.offsets or a.positions)
    print(f'{len(anchors)} anchors in {len(s.blocks)} chunks of {len(set(b["parent"] for b in s.blocks.values()))} objects', flush=True)
    if a.positions: s.check_anchors_zones(log=print)
    else: s.clean_anchors(log=print)
    if a.top:
        tops = [int(l.split()[0]) for l in open(a.top) if l.strip() and l.split()[0].isdigit()]
        s.add_top(tops, a.top_max)
        print(f'top block: {len(s.top_chunks)} merge-similar-functions bodies confined to the top {a.top_max} ordinals', flush=True)
    truth = {}
    if a.truth:
        for l in open(a.truth):
            p = l.rstrip('\n').split('\t')
            if p[0].isdigit() and p[1].lstrip('-').isdigit(): truth[int(p[0])] = int(p[1])

    # 1. band-adjacency test per chunk (residues)
    cand = {n: s.base_candidates(n) for n in s.blocks}
    res = {n: s.residues(c) for n, c in cand.items()}
    inconsistent = [n for n, c in cand.items() if not c.any()]
    for n in inconsistent: cand[n][:] = True
    pinned = collections.Counter(max(r) if r else 0 for r in res.values())
    print(f'chunks pinned modulo: {dict(sorted(pinned.items()))}; {len(inconsistent)} chunks with no consistent base (anchors wrong or incomplete)', flush=True)

    # 2. block order
    order, succ, reach, tied = s.block_order()
    print(f'block order: {len(order)} chunks, {sum(tied.values())} ties', flush=True)

    # 3. feasible bases: order-count bounds (band adjacency orders chunks only inside one digit zone, so these bounds
    #    are unsound across zones and are skipped in positions mode), chains, cut equations
    if not a.positions: cand = s.order_count_bounds(cand, reach)
    cand = s.propagate(cand, a.holes, slack=a.slack, log=print, tolerance=a.tolerance)
    if s.suspect:
        print(f'quarantined chunks: {len(s.suspect)} ({sum(s.blocks[n]["n"] for n in s.suspect)} anchors) — their members are reported unanchored', flush=True)
    mlo = {}; mhi = {}
    for n, blk in s.blocks.items():
        if n in s.suspect: continue
        feas = np.where(cand[n])[0]
        if len(feas) == 0: continue
        for r, o in blk['members']:
            rk = s.rank_of[feas + o]; mlo[r] = int(rk.min()); mhi[r] = int(rk.max())
    ords = {r: int(s.ord_of_rank[(mlo[r] + mhi[r]) // 2]) for r in mlo}
    lo, hi = s.windows_from(mlo, mhi)
    viol = sum(1 for band in s.bands for ra, rb in zip([r for r in band if r in ords], [r for r in band if r in ords][1:]) if s.rank_of[ords[rb]] <= s.rank_of[ords[ra]])
    dup = len(ords) - len(set(ords.values()))
    print(f'point estimates: {len(ords)} anchored ordinals, band-order violations {viol}, duplicates {dup}', flush=True)
    w = np.array([mhi[r] - mlo[r] + 1 for r in mlo])
    print(f'anchored rank windows: median {np.median(w):.0f}, exact {int((w == 1).sum())}, <=10 {int((w <= 10).sum())}, <=100 {int((w <= 100).sum())}', flush=True)
    wa = hi - lo + 1
    if getattr(s, 'chain_bad', None):
        bad = collections.Counter(s.blocks[pv]['parent'] for pv, n in s.chain_bad)
        print('inconsistent chain links per object:', dict(bad.most_common(12)))
        with open(a.out + '.badlinks.tsv', 'w') as f:
            f.write('object\tprev_chunk\tprev_members\tnext_chunk\tnext_members\n')
            for pv, n in s.chain_bad:
                f.write(f"{s.blocks[pv]['parent']}\t{pv}\t{','.join(str(s.funcs[r]) for r, o in s.blocks[pv]['members'])}\t{n}\t{','.join(str(s.funcs[r]) for r, o in s.blocks[n]['members'])}\n")
    print(f'ALL {s.N} functions, rank windows: <=1 {int((wa <= 1).sum())}, <=10 {int((wa <= 10).sum())}, <=50 {int((wa <= 50).sum())}, '
          f'<=100 {int((wa <= 100).sum())}, <=500 {int((wa <= 500).sum())}, median {np.median(wa):.0f}; chain links inconsistent: {getattr(s, "chain_inconsistent", 0)}', flush=True)
    B = {}; interval = {}
    for n in order:
        feas = np.where(cand[n])[0]
        if len(feas) == 0: B[n] = -1; interval[n] = (-1, -1, 0); continue
        B[n] = int(feas[len(feas) // 2]); interval[n] = (int(feas[0]), int(feas[-1]), len(feas))
    if truth:
        anc = [(r, ords[r]) for r in ords if s.funcs[r] in truth]
        inside = sum(1 for r in mlo if s.funcs[r] in truth and mlo[r] <= s.rank_of[truth[s.funcs[r]]] <= mhi[r])
        ex = sum(1 for r, o in anc if truth[s.funcs[r]] == o)
        errs = np.array([o - truth[s.funcs[r]] for r, o in anc])
        print(f'truth anchored: truth inside window {inside}/{len(mlo)}; point exact {ex}/{len(anc)} ({100*ex/len(anc):.1f}%), '
              f'|err| median {np.median(np.abs(errs)):.0f} mean {np.abs(errs).mean():.1f} max {np.abs(errs).max()}; within 10: {int((np.abs(errs) <= 10).sum())}, within 50: {int((np.abs(errs) <= 50).sum())}')
        win_ok = win_n = 0; widths = []
        for i, fi in enumerate(s.funcs):
            if i in ords or fi not in truth: continue
            rk = s.rank_of[truth[fi]]; win_n += 1; win_ok += int(lo[i] <= rk <= hi[i]); widths.append(int(hi[i] - lo[i] + 1))
        print(f'truth unanchored: inside window {win_ok}/{win_n}, median width {np.median(widths) if widths else 0:.0f}')

    # output
    with open(a.out, 'w') as f:
        f.write('index\tuses\tord_lo\tord_hi\tordinal\tblock\toffset\tkind\trank_lo\trank_hi' + ('\ttruth\tok\terr' if truth else '') + '\n')
        for i, fi in enumerate(s.funcs):
            rk_lo, rk_hi = int(lo[i]), int(hi[i])
            if rk_lo > rk_hi: rk_lo, rk_hi = 0, s.M - 1
            ordsel = s.ord_of_rank[rk_lo:rk_hi + 1]
            olo, ohi = int(ordsel.min()), int(ordsel.max())
            if i in ords:
                n, o = s.blk_of_row[i]; kind = 'anchored'; point = ords[i]; n = s.blocks[n]['parent']
            else:
                n, o, kind, point = '', '', 'window', int(s.ord_of_rank[(rk_lo + rk_hi) // 2])
            line = f'{fi}\t{s.u[fi]}\t{olo}\t{ohi}\t{point}\t{n}\t{o}\t{kind}\t{rk_lo}\t{rk_hi}'
            if truth:
                t = truth.get(fi)
                if t is None: line += '\t\t\t'
                elif i in ords: line += f'\t{t}\t{int(ords[i] == t)}\t{ords[i] - t}'
                else: line += f'\t{t}\t{int(olo <= t <= ohi)}\t'
            f.write(line + '\n')
    if a.blocks_out:
        with open(a.blocks_out, 'w') as f:
            f.write('order\tchunk\tobject\tbase\tlast\tmembers\tspan\tbase_lo\tbase_hi\tfeasible\tresidue_mod\tresidue\ttied\n')
            for i, n in enumerate(order):
                r = res[n]; mod = max(r) if r else ''; iv = interval[n]; blk = s.blocks[n]
                f.write(f'{i}\t{n}\t{blk["parent"]}\t{B[n]}\t{B[n] + blk["L"] - 1}\t{blk["n"]}\t{blk["L"]}\t{iv[0]}\t{iv[1]}\t{iv[2]}\t{mod}\t{r.get(mod, "") if r else ""}\t{int(tied[n])}\n')
    print(f'wrote {a.out} ({time.time() - t0:.0f}s)')
    victims = {s.funcs[r] for n in s.suspect for r, o in s.blocks[n]['members']}
    return s, ords, lo, hi, cand, order, victims


def solve(a):
    """Solve; with --repair R, drop the anchors of the smaller chunk of every inconsistent chain link (a misidentified
    function cuts its object's chain and lets the chunks beyond it float) and solve again, up to R times."""
    anchors = []
    for p in a.anchors: anchors += read_anchors(p)
    for rnd in range(a.repair + 1):
        res = _solve_once(a, anchors); victims = res[-1]
        if not victims or rnd == a.repair: break
        anchors = [x for x in anchors if x[0] not in victims]
        print(f'repair round {rnd + 1}: {len(victims)} anchors of inconsistent links dropped, {len(anchors)} anchors left', flush=True)
    if a.repair:
        with open(a.out + '.anchors.tsv', 'w') as f:
            f.write('index\tblock\tpos\n')
            for x in anchors: f.write(f'{x[0]}\t{x[1]}\t{x[2]}\n')
    return res[:-1]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['solve'])
    ap.add_argument('module')
    ap.add_argument('--anchors', action='append', default=[])
    ap.add_argument('--offsets', action='store_true', help="the pos column is the offset among the object's live functions (gaps allowed)")
    ap.add_argument('--positions', action='store_true', help="the pos column is the position in the object's FULL function list: rigid chunks of adjacent positions, exact chain gaps")
    ap.add_argument('--top', help='file of function indices that are merge-similar-functions bodies (highest ordinals)')
    ap.add_argument('--top-max', dest='top_max', type=int, default=600, help='upper bound on the number of such bodies')
    ap.add_argument('--holes', type=int, default=2)
    ap.add_argument('--repair', type=int, default=0, help='rounds of dropping the anchors of inconsistent chain links and re-solving')
    ap.add_argument('--robust', action='store_true', help='second-order band envelopes: a single wrong anchor cannot move a count by its band position, but the windows get far looser (skeleton: no window <= 100)')
    ap.add_argument('--tolerance', type=int, default=0, help='slack (functions) on each side of the cut equations, for wrong anchors no chain catches')
    ap.add_argument('--slack', type=float, default=0.5, help='allowed relative stretch of a chain link beyond its known gap')
    ap.add_argument('--truth')
    ap.add_argument('-o', '--out', required=True)
    ap.add_argument('--blocks', dest='blocks_out')
    a = ap.parse_args()
    solve(a)


if __name__ == '__main__':
    main()
