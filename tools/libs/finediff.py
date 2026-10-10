"""Version bounds from string literals: the literals each tag of a library's git repo adds or drops that the target
holds. usage: python3 -I tools/libs/finediff.py <repo> <subdir,…> <tag,…> [<other source dir>…]; run from a scratch
dir holding strings4.txt (strings -n 4 target/vota_web_wasm.wasm), emscripten/system/lib/ (copied out of the pinned
image) and src_zlib/ (zlib 1.3.1): literals of those and of the vendored SQLite and OpenSSL are excluded."""
import subprocess,re,sys,os
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
repo,sub,tags=sys.argv[1],sys.argv[2].split(','),sys.argv[3].split(',')
exts=('.c','.cpp','.h','.hpp','.cc','.inc','.inl','.cxx')
bins=set(l.rstrip('\n') for l in open('strings4.txt',encoding='utf-8',errors='replace'))
lit=re.compile(r'"((?:[^"\\\n]|\\.){4,})"')
# other libs' literals to exclude
def lits_dir(roots):
    s=set()
    for root in roots:
        for dp,dn,fn in os.walk(root):
            for f in fn:
                if f.endswith(exts):
                    try: src=open(os.path.join(dp,f),encoding='utf-8',errors='replace').read()
                    except: continue
                    for m in lit.finditer(src):
                        if '\\' not in m.group(1): s.add(m.group(1))
    return s
TP=ROOT+'/third_party'
others=lits_dir([TP+'/sqlite',TP+'/openssl/crypto',TP+'/openssl/providers','emscripten/system/lib','src_zlib']+[x for x in sys.argv[4:]])
sets={}
for t in tags:
    files=subprocess.run(['git','-C',repo,'ls-tree','-r','--name-only',t]+sub,capture_output=True,text=True).stdout.split()
    s=set()
    for f in files:
        if not f.endswith(exts): continue
        src=subprocess.run(['git','-C',repo,'show',f'{t}:{f}'],capture_output=True,text=True,errors='replace').stdout
        for m in lit.finditer(src):
            v=m.group(1)
            if '\\' not in v: s.add(v)
    sets[t]=s-others
    print(t,'own literals',len(sets[t]),'in binary',len(sets[t]&bins))
for i in range(len(tags)-1):
    a,b=tags[i],tags[i+1]
    oa=(sets[a]-sets[b])&bins; ob=(sets[b]-sets[a])&bins
    print(f'{a}->{b}: only-{a}:{sorted(oa)}  only-{b}:{sorted(ob)}')
