# Generate the asnparser-style headers of TSE's generated Modulo* types from the recovered schema.
import json, collections, re, sys
import os
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
SP = ROOT + '/build/out/asn1-schema'
OUT = sys.argv[1] if len(sys.argv) > 1 else SP + '/modules'
U = ROOT
d = json.load(open(SP + '/schema.json')); types = {int(k): v for k, v in d['types'].items()}
rtti = {c['name']: (c['bases'][0]['name'] if c.get('bases') else '') for c in json.load(open(U + '/ledger/classes.json'))['classes'] if c['name'].startswith('Modulo')}
code_names = set(l.strip() for l in open(SP + '/modulo-names-in-code.txt') if l.strip() and 'NamedNumber' not in l and 'value_type' not in l)
code_names |= set(re.sub('::NamedNumber$', '', l.strip()) for l in open(SP + '/modulo-names-in-code.txt') if l.strip().endswith('::NamedNumber'))
DATA_END = 1811674
BASEKIND = {'ASN1::ENUMERATED': 'ENUMERATED', 'ASN1::CHOICE': 'CHOICE', 'ASN1::GeneralString': 'STRING', 'ASN1::NumericString': 'STRING', 'ASN1::SEQUENCE': 'SEQUENCE'}

# ---- 1. module of every record: RTTI name, else position between RTTI-named neighbours (records of one module are contiguous)
def modname(c): return c.split('::')[0] if c and c.startswith('Modulo') else None
addrs = sorted(T for T in types if T < DATA_END)
known = [(T, modname(types[T].get('cxx'))) for T in addrs if modname(types[T].get('cxx'))]
def module_of(T):
    m = modname(types[T].get('cxx'))
    if m: return m
    before = [km for kt, km in known if kt < T]
    after = [km for kt, km in known if kt > T]
    b = before[-1] if before else None; a = after[0] if after else None
    if b == a or a is None: return b
    if b is None: return a
    # inside a module run but at its edge: prefer the module that references it
    for o, f in types[T]['via']:
        if o != 'root' and isinstance(o, int):
            om = modname(types[o].get('cxx'))
            if om in (a, b): return om
    return b
# BSS records (dynamic init): module of the owner field
for T, r in types.items():
    if T >= DATA_END:
        owners = [o for o, f in r['via'] if isinstance(o, int)]
        r['module'] = module_of(owners[0]) if owners else None
    else:
        r['module'] = module_of(T)

# ---- 2. names: RTTI first (done in schema), then the names the app code spells, then the owner field, capitalised
assigned = {r['cxx'] for r in types.values() if r.get('cxx', '').startswith('Modulo')}
def cap(s): return s[0].upper() + s[1:]
def short_of(T):
    r = types[T]
    c = r.get('cxx', '')
    if c.startswith('Modulo') and c.count('::') == 1: return c.split('::')[1], True
    if c.startswith('ASN1::'): return None, True
    return None, False
notes = {}
for T, r in types.items():
    if r['kind'] == 'SEQUENCE' and 'cxx' in r and r['cxx'].startswith('?::') and r.get('name') and r['name'] != 'value_type':
        r['cxx'] = f"{r['module']}::{r['name']}"
assigned |= {r['cxx'] for r in types.values() if r.get('cxx', '').startswith('Modulo')}
all_named = {n: n.split('::', 1)[1] for n in (set(rtti) | code_names) if n.count('::') == 1}
for T, r in sorted(types.items()):
    s, ok = short_of(T)
    if ok: continue
    kind = r['kind']
    owners = [(o, f) for o, f in r['via'] if isinstance(o, int) and f != 'element']
    fields = {f.lower() for o, f in owners}
    hits = [n for n, sn in all_named.items() if n not in assigned and sn.lower() in fields and BASEKIND.get(rtti.get(n, ''), kind) == kind]
    if len(hits) == 1:
        r['cxx'] = hits[0]; r['module'] = hits[0].split('::')[0]; assigned.add(hits[0]); notes[T] = 'named by exact field/RTTI match'
for T, r in sorted(types.items()):
    s, ok = short_of(T)
    if ok: continue
    kind = r['kind']; m = r['module']
    # (a) a SEQUENCE knows its own identifier
    if kind == 'SEQUENCE' and r.get('name') and r['name'] != 'value_type':
        r['cxx'] = f'{m}::{r["name"]}'; continue
    # (b) a name the app spells in this module, matching the field name
    owners = [(o, f) for o, f in r['via'] if isinstance(o, int) and f != 'element']
    fields = [f for o, f in owners]
    cands = [n for n in code_names | set(rtti) if n.startswith(m + '::') and n not in assigned]
    pick = None
    for n in cands:
        sn = n.split('::', 1)[1]
        want = BASEKIND.get(rtti.get(n, ''), None)
        if want and want != kind: continue
        if any(sn.lower() == f.lower() or sn.lower() == ('tipo' + f.lower()) for f in fields):
            pick = n; break
    if pick:
        r['cxx'] = pick; assigned.add(pick); notes[T] = 'name from the app code / RTTI by field match'; continue
    # (c) nested: Owner::field::value_type when the type is used by exactly one field of one owner
    if len(set(owners)) == 1:
        o, f = owners[0]
        r['cxx'] = f'{types[o].get("cxx", "?")}::{f}::value_type'; r['nested'] = True; continue
    # (d) element of a SEQUENCE_OF or shared: synthesize
    syn = cap(fields[0]) if fields else (r.get('name') or 'Tipo%d' % T)
    r['cxx'] = f'{m}::{syn}'; notes[T] = 'SYNTHESIZED name (field %s)' % fields

# ---- 3. C++ type spelling for a field/element record
def tag_hex(t): return '0x%08x' % (t & 0xffffffff)
def cxx_type(T):
    r = types[T]; c = r['cxx']
    if c.startswith('ASN1::') or c.startswith('Modulo'): return c
    return c
def constraint_str(r):
    if r.get('ctype', 0) == 0: return None
    return 'ASN1::SizeConstraint<ASN1::FixedConstraint, %d, %d>' % (r['lower'], r['upper'])

# ---- 4. emit per module
by_module = collections.defaultdict(list)
for T, r in types.items():
    m = modname(r['cxx']) or r['module']
    r['module'] = m
    if m and not r['cxx'].startswith('ASN1::'): by_module[m].append(T)

def emit_enum(r, ind, name):
    h = []
    h.append(f'{ind}class {name} : public ASN1::ENUMERATED')
    h.append(f'{ind}{{')
    h.append(f'{ind}    typedef ASN1::ENUMERATED Inherited;')
    h.append(f'{ind}protected:')
    h.append(f'{ind}    typedef Inherited::InfoType InfoType;')
    h.append(f'{ind}    {name}(const void* info) : Inherited(info) {{}}')
    h.append(f'{ind}public:')
    h.append(f'{ind}    {name}() : Inherited(&theInfo) {{}}')
    h.append(f'{ind}    enum NamedNumber {{')
    h.append(f'{ind}        unknownEnumeration_ = -1,')
    items = []
    for i, n in enumerate(r['names']):
        if n.startswith('<undefined>'): continue
        items.append(f'{ind}        {n}' + ('' if i == len([x for x in items]) and False else ''))
    # numbering: explicit when gaps exist
    lines = []; prev = -1
    for i, n in enumerate(r['names']):
        if n.startswith('<undefined>'): continue
        lines.append(f'{ind}        {n}' + (f' = {i}' if i != prev + 1 else '')); prev = i
    h.append(',\n'.join(lines))
    h.append(f'{ind}    }};')
    h.append('')
    for i, n in enumerate(r['names']):
        if n.startswith('<undefined>'): continue
        h.append(f'{ind}    bool is_{n}() const {{ return value == {n}; }}')
        h.append(f'{ind}    void set_{n}() {{ value = {n}; }}')
        h.append('')
    h.append(f'{ind}    {name}(const NamedNumber v) : Inherited(&theInfo) {{ setFromInt(v); }}')
    h.append(f'{ind}    {name} & operator=(const NamedNumber v) {{ setFromInt(v);  return *this; }}')
    h.append(f'{ind}    operator NamedNumber() const {{ return NamedNumber(asInt()); }}')
    h.append('')
    h.append(f'{ind}    bool operator == (NamedNumber rhs) const {{ return value == rhs; }}')
    h.append(f'{ind}    bool operator != (NamedNumber rhs) const {{ return value != rhs; }}')
    h.append(f'{ind}    bool operator <  (NamedNumber rhs) const {{ return value <  rhs; }}')
    h.append(f'{ind}    bool operator >  (NamedNumber rhs) const {{ return value >  rhs; }}')
    h.append(f'{ind}    bool operator <= (NamedNumber rhs) const {{ return value <= rhs; }}')
    h.append(f'{ind}    bool operator >= (NamedNumber rhs) const {{ return value >= rhs; }}')
    h.append(f'{ind}    bool operator == (const {name} &rhs) const {{ return value == rhs.value; }}')
    h.append(f'{ind}    bool operator != (const {name} &rhs) const {{ return value != rhs.value; }}')
    h.append('')
    h.append(f'{ind}    void swap ({name} & that) {{ Inherited::swap(that); }}')
    h.append(f'{ind}    {name} * clone() const')
    h.append(f'{ind}    {{ return static_cast<{name}*> (Inherited::clone()); }}')
    h.append(f'{ind}    static bool equal_type(const ASN1::AbstractData& type)')
    h.append(f'{ind}    {{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }}')
    h.append(f'{ind}    static const InfoType theInfo;')
    h.append(f'{ind}private:')
    h.append(f'{ind}    static const char * nameList[];')
    h.append(f'{ind}}}; // end class {name}')
    return h
def emit_enum_cxx(r, cls):
    c = []
    c.append(f'const char * {cls}::nameList[] = {{')
    c.append('    "' + '",\n    "'.join(r['names']) + '"')
    c.append('  };')
    c.append('')
    c.append(f'const {cls}::InfoType {cls}::theInfo = {{')
    c.append('    ASN1::ENUMERATED::create,')
    c.append(f'    {tag_hex(r["tag"])},')
    c.append('    0,')
    c.append(f'    {r["extendable"]},')
    c.append(f'    {r["maxEnumValue"]}, nameList')
    c.append('};')
    c.append('')
    return c

STRING_PARENTS = {1148180: 'GeneralString', 1148904: 'NumericString'}
def string_base(r):
    b = r.get('base')
    if b: return b
    b = STRING_PARENTS.get(r.get('parent'))
    if b: return b
    pr = types.get(r.get('parent'))
    if pr and pr.get('base'): return pr['base']
    return 'GeneralString'
def emit_string(r, ind, name):
    base = 'ASN1::' + string_base(r)
    h = [f'{ind}class {name} : public {base}', f'{ind}{{', f'{ind}    typedef {base} Inherited;', f'{ind}protected:',
         f'{ind}    typedef Inherited::InfoType InfoType;', f'{ind}    {name}(const void* info) : Inherited(info) {{}}', f'{ind}public:',
         f'{ind}    {name}() : Inherited(&theInfo) {{}}',
         f'{ind}    {name}(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {{}}',
         f'{ind}    {name}(const char* str, const void* info = &theInfo) : Inherited(str, info) {{}}',
         f'{ind}    {name}(const {name}& that) : Inherited(that) {{}}',
         f'{ind}    {name}& operator=(const ASN1_STD string& that)', f'{ind}    {{ Inherited::operator=(that); return *this; }}',
         f'{ind}    {name}& operator=(const char* that)', f'{ind}    {{ Inherited::operator=(that); return *this; }}',
         f'{ind}    {name} * clone() const', f'{ind}    {{ return static_cast<{name}*> (Inherited::clone()); }}',
         f'{ind}    static bool equal_type(const ASN1::AbstractData& type)',
         f'{ind}    {{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }}',
         f'{ind}    static const InfoType theInfo;', f'{ind}}}; // end class {name}']
    return h
def emit_string_cxx(r, cls):
    base = 'ASN1::' + string_base(r)
    return [f'const {cls}::InfoType {cls}::theInfo = {{', f'    ASN1::AbstractString::create,', f'    {tag_hex(r["tag"])},', f'    &{base}::theInfo,',
            f'    {"ASN1::FixedConstraint" if r["ctype"] == 2 else ("ASN1::ExtendableConstraint" if r["ctype"] == 1 else "ASN1::Unconstrained")}, {r["lower"]}, {r["upper"]},',
            f'    {base}::theInfo.characterSet, {base}::theInfo.characterSetSize, {base}::theInfo.canonicalSetBits,',
            f'    {base}::theInfo.charSetUnalignedBits, {base}::theInfo.charSetAlignedBits', '};', '']

def emit_seqof(r, ind, name):
    el = cxx_type(r['element'])
    cons = constraint_str(r)
    base = f'ASN1::SEQUENCE_OF<{el}' + (f', {cons}' if cons else '') + '>'
    h = [f'{ind}class {name} : public {base}', f'{ind}{{', f'{ind}    typedef {base} Inherited;', f'{ind}protected:',
         f'{ind}    typedef Inherited::InfoType InfoType;', f'{ind}    {name}(const void* info) : Inherited(info) {{}}', f'{ind}public:',
         f'{ind}    {name}(size_type n=0) : Inherited(n) {{}}',
         f'{ind}    {name}(size_type n, const {el}& val) : Inherited(n, val) {{}}',
         f'{ind}    {name}(const_iterator first, const_iterator last) : Inherited(first, last) {{}}',
         f'{ind}    {name}(const {name}& that) : Inherited(that) {{}}',
         f'{ind}    {name} * clone() const', f'{ind}    {{ return static_cast<{name}*> (Inherited::clone()); }}',
         f'{ind}    static bool equal_type(const ASN1::AbstractData& type)',
         f'{ind}    {{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }}',
         f'{ind}    static const InfoType theInfo;', f'{ind}}}; // end class {name}']
    return h
def emit_seqof_cxx(r, cls):
    el = cxx_type(r['element'])
    c = [f'const {cls}::InfoType {cls}::theInfo = {{', '    ASN1::SEQUENCE_OF_Base::create,', f'    {tag_hex(r["tag"])},', '    0,']
    if r['ctype']: c.append(f'    ASN1::FixedConstraint, {r["lower"]}, {r["upper"]},')
    else: c.append('    ASN1::Unconstrained, 0, UINT_MAX,')
    c.append(f'    &{el}::theInfo,')
    c.append(f'    "{r["name"]}"' if r.get('name') else '    0')
    c.append('};'); c.append('')
    return c

def field_typename(fr):
    """the value_type spelling of a field's record (nested classes are generated in place)"""
    c = fr['cxx']
    if fr.get('nested'): return None   # generated inside the component class
    return c

def emit_component(ownerT, f, ind, hdr, cxx_out):
    fr = types[f['info']]
    comp = f['name']
    hdr.append('')
    hdr.append(f'{ind}class {comp}')
    hdr.append(f'{ind}{{')
    hdr.append(f'{ind}public:')
    if fr.get('nested'):
        emit_type(f['info'], ind + '    ', hdr, cxx_out, 'value_type')
    else:
        hdr.append(f'{ind}    typedef {fr["cxx"]} value_type;')
    hdr.append(f'{ind}    typedef value_type&        reference;')
    hdr.append(f'{ind}    typedef const value_type&  const_reference;')
    hdr.append(f'{ind}    typedef value_type*        pointer;')
    hdr.append(f'{ind}    typedef const value_type*  const_pointer;')
    hdr.append(f'{ind}}}; // end class {comp}')
    hdr.append('')

def prim_type(fr, comp=''):
    k = fr['kind']
    if k == 'INTEGER': return 'ASN1::INTEGER::int_type'
    if k == 'BOOLEAN': return 'bool'
    if k == 'ENUMERATED': return f'{fr["cxx"]}::NamedNumber' if not fr.get('nested') else f'{comp}::value_type::NamedNumber'
    if k == 'STRING': return 'const ASN1_STD string&'
    if k == 'OCTET_STRING': return 'const ASN1::OCTET_STRING&'
    return None

def emit_sequence(T, r, ind, name, hdr, cxx_out, cls):
    n = r['numFields'] + r['knownExtensions']
    hdr.append(f'{ind}class {name} : public ASN1::SEQUENCE')
    hdr.append(f'{ind}{{')
    hdr.append(f'{ind}    typedef ASN1::SEQUENCE Inherited;')
    hdr.append(f'{ind}protected:')
    hdr.append(f'{ind}    typedef Inherited::InfoType InfoType;')
    hdr.append(f'{ind}    {name}(const void* info) : Inherited(info) {{}}')
    hdr.append(f'{ind}public:')
    hdr.append(f'{ind}    {name}() : Inherited(&theInfo) {{}}')
    hdr.append(f'{ind}    {name}(const {name} & that) : Inherited(that) {{}}')
    hdr.append('')
    hdr.append(f'{ind}    {name}& operator=(const {name}& that)')
    hdr.append(f'{ind}    {{ Inherited::operator=(that); return *this; }}')
    hdr.append('')
    opt = [(i, f) for i, f in enumerate(r['fields']) if f['optional_id'] != -1 or i >= r['numFields']]
    if opt:
        hdr.append(f'{ind}    enum OptionalFields {{')
        hdr.append(',\n'.join(f'{ind}      e_{f["name"]}' for i, f in opt))
        hdr.append(f'{ind}    }};')
    for i, f in enumerate(r['fields']):
        emit_component(T, f, ind + '    ', hdr, cxx_out)
        fr = types[f['info']]
        comp = f['name']; pt = prim_type(fr, comp)
        optional = f['optional_id'] != -1 or i >= r['numFields']
        var = f'*static_cast<{comp}::pointer>(fields[{i}])'; cvar = f'*static_cast<{comp}::const_pointer>(fields[{i}])'
        if not optional:
            hdr.append(f'{ind}    {comp}::const_reference get_{comp} () const {{ return {cvar}; }}')
            hdr.append(f'{ind}    {comp}::reference ref_{comp} () {{ return {var}; }}')
            hdr.append(f'{ind}    {comp}::reference set_{comp} () {{ return {var}; }}')
            if pt:
                hdr.append(f'{ind}    {comp}::reference set_{comp} ({pt} v)')
                hdr.append(f'{ind}    {{ return {var} = v; }}')
        else:
            en = f'e_{comp}'
            hdr.append(f'{ind}    {comp}::const_reference get_{comp} () const')
            hdr.append(f'{ind}    {{')
            hdr.append(f'{ind}      assert(hasOptionalField({en}));')
            hdr.append(f'{ind}      return {cvar};')
            hdr.append(f'{ind}    }}')
            hdr.append(f'{ind}    {comp}::reference ref_{comp} ()')
            hdr.append(f'{ind}    {{')
            hdr.append(f'{ind}      assert(hasOptionalField({en}));')
            hdr.append(f'{ind}      return {var};')
            hdr.append(f'{ind}    }}')
            hdr.append(f'{ind}    {comp}::reference set_{comp} ()')
            hdr.append(f'{ind}    {{')
            hdr.append(f'{ind}      includeOptionalField( {en}, {i});')
            hdr.append(f'{ind}      return {var};')
            hdr.append(f'{ind}    }}')
            if pt:
                hdr.append(f'{ind}    {comp}::reference set_{comp} ({pt} v)')
                hdr.append(f'{ind}    {{')
                hdr.append(f'{ind}      includeOptionalField( {en}, {i});')
                hdr.append(f'{ind}      return {var} = v;')
                hdr.append(f'{ind}    }}')
            hdr.append(f'{ind}    void omit_{comp} () {{ removeOptionalField( {en}); }}')
            hdr.append(f'{ind}    bool {comp}_isPresent () const {{ return hasOptionalField( {en}); }}')
    hdr.append(f'{ind}    void swap({name}& that) {{ Inherited::swap(that); }}')
    hdr.append(f'{ind}    {name} * clone() const')
    hdr.append(f'{ind}    {{ return static_cast<{name}*> (Inherited::clone()); }}')
    hdr.append(f'{ind}    static const Inherited::InfoType theInfo;')
    hdr.append('')
    hdr.append(f'{ind}private:')
    if n:
        hdr.append(f'{ind}    static const void* fieldInfos[{n}];')
        hdr.append(f'{ind}    static int fieldIds[{n}];')
        if isinstance(r['tags'], list): hdr.append(f'{ind}    static unsigned fieldTags[{n}];')
        hdr.append(f'{ind}    static const char* fieldNames[{n}];')
    hdr.append(f'{ind}}}; // end class {name}')
    hdr.append('')
    # cxx
    c = cxx_out
    if n:
        c.append(f'const void* {cls}::fieldInfos[{n}] = {{')
        c.append(',\n'.join(f'    &{f["name"]}::value_type::theInfo' for f in r['fields']))
        c.append('};'); c.append('')
        c.append(f'int {cls}::fieldIds[{n}] = {{')
        c.append(',\n'.join(f'    {f["optional_id"]}' for f in r['fields']))
        c.append('};'); c.append('')
        if isinstance(r['tags'], list):
            c.append(f'unsigned {cls}::fieldTags[{n}] = {{')
            c.append(',\n'.join(f'    {tag_hex(t)}' for t in r['tags']))
            c.append('};'); c.append('')
        c.append(f'const char* {cls}::fieldNames[{n}] = {{')
        c.append(',\n'.join(f'    "{f["name"]}"' for f in r['fields']))
        c.append('};'); c.append('')
    tags = 'NULL' if r['tags'] is None or n == 0 else (f'&{cls}::defaultTag' if r['tags'] == 'default' else f'{cls}::fieldTags')
    c.append(f'const {cls}::InfoType {cls}::theInfo = {{')
    c.append(f'    {cls}::create,')
    c.append(f'    {tag_hex(r["tag"])},')
    c.append('    0,')
    c.append('    ' + ('true' if r['extendable'] else 'false') + ',')
    c.append(f'    {cls}::fieldInfos,\n    {cls}::fieldIds,' if n else '    NULL, NULL,')
    c.append(f'    {r["numFields"]}, {r["knownExtensions"]}, {r["numOptional"]},')
    c.append('    NULL,' if not r['nonOptionalExtensions'] else f'    {cls}::nonOptionalExtensions,')
    c.append(f'    {tags},')
    c.append(f'    {cls}::fieldNames,' if n else '    NULL,')
    c.append(f'    "{r["name"]}"')
    c.append('};'); c.append('')

def emit_choice(T, r, ind, name, hdr, cxx_out, cls):
    n = r['totalChoices']
    hdr.append(f'{ind}class {name} : public ASN1::CHOICE')
    hdr.append(f'{ind}{{')
    hdr.append(f'{ind}    typedef ASN1::CHOICE Inherited;')
    hdr.append(f'{ind}protected:')
    hdr.append(f'{ind}    typedef Inherited::InfoType InfoType;')
    hdr.append(f'{ind}    {name}(const void* info) : Inherited(info) {{}}')
    hdr.append(f'{ind}public:')
    hdr.append(f'{ind}    {name}() : Inherited(&theInfo) {{}}')
    hdr.append(f'{ind}    enum Choice_Ids {{')
    hdr.append(f'{ind}      unknownSelection_ = -2,')
    hdr.append(f'{ind}      unselected_ = -1,')
    hdr.append(',\n'.join(f'{ind}      {c["name"]}{i} = {i}' for i, c in enumerate(r['choices'])))
    hdr.append(f'{ind}    }};')
    for i, ch in enumerate(r['choices']):
        fr = types[ch['info']]; comp = ch['name']
        hdr.append('')
        hdr.append(f'{ind}    class {comp}')
        hdr.append(f'{ind}    {{')
        hdr.append(f'{ind}    public:')
        hdr.append(f'{ind}        enum Id {{ eid = {i} }};')
        if fr.get('nested'): emit_type(ch['info'], ind + '        ', hdr, cxx_out, 'value_type')
        else: hdr.append(f'{ind}        typedef {fr["cxx"]} value_type;')
        hdr.append(f'{ind}        typedef value_type&        reference;')
        hdr.append(f'{ind}        typedef const value_type&  const_reference;')
        hdr.append(f'{ind}        typedef value_type*        pointer;')
        hdr.append(f'{ind}        typedef const value_type*  const_pointer;')
        hdr.append(f'{ind}    }}; // end class {comp}')
        hdr.append('')
        hdr.append(f'{ind}    {comp}::const_reference get_{comp} () const')
        hdr.append(f'{ind}    {{')
        hdr.append(f'{ind}        assert(currentSelection() == {comp}::eid);')
        hdr.append(f'{ind}        return *static_cast<{comp}::const_pointer>(choice.get());')
        hdr.append(f'{ind}    }}')
        hdr.append(f'{ind}    {comp}::reference ref_{comp} ()')
        hdr.append(f'{ind}    {{')
        hdr.append(f'{ind}        assert(currentSelection() == {comp}::eid);')
        hdr.append(f'{ind}        return *static_cast<{comp}::pointer>(choice.get());')
        hdr.append(f'{ind}    }}')
        hdr.append(f'{ind}    {comp}::reference select_{comp} ()')
        hdr.append(f'{ind}    {{')
        hdr.append(f'{ind}        return *static_cast<{comp}::pointer>(setSelection({comp}::eid, ASN1::AbstractData::create(&{comp}::value_type::theInfo)));')
        hdr.append(f'{ind}    }}')
        hdr.append(f'{ind}    bool {comp}_isSelected () const {{ return currentSelection() == {comp}::eid; }}')
    hdr.append('')
    hdr.append(f'{ind}    {name}(const {name} & that) : Inherited(that) {{}}')
    hdr.append(f'{ind}    {name} & operator=(const {name} & that)')
    hdr.append(f'{ind}    {{ Inherited::operator=(that); return *this; }}')
    hdr.append(f'{ind}    void swap({name} & that) {{ Inherited::swap(that); }}')
    hdr.append(f'{ind}    {name} * clone() const')
    hdr.append(f'{ind}    {{ return static_cast<{name}*> (Inherited::clone()); }}')
    hdr.append(f'{ind}    static bool equal_type(const ASN1::AbstractData& type)')
    hdr.append(f'{ind}    {{ return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }}')
    hdr.append(f'{ind}    static const InfoType theInfo;')
    hdr.append(f'{ind}private:')
    hdr.append(f'{ind}    static const void* selectionInfos[{n}];')
    if r['tags']: hdr.append(f'{ind}    static unsigned selectionTags[{n}];')
    hdr.append(f'{ind}    static const char* selectionNames[{n}];')
    hdr.append(f'{ind}}}; // end class {name}')
    hdr.append('')
    c = cxx_out
    c.append(f'const void* {cls}::selectionInfos[{n}] = {{')
    c.append(',\n'.join(f'    &{ch["name"]}::value_type::theInfo' for ch in r['choices']))
    c.append('};'); c.append('')
    if r['tags']:
        c.append(f'unsigned {cls}::selectionTags[{n}] = {{')
        c.append(',\n'.join(f'    {tag_hex(t)}' for t in r['tags']))
        c.append('};'); c.append('')
    c.append(f'const char* {cls}::selectionNames[{n}] = {{')
    c.append(',\n'.join(f'    "{ch["name"]}"' for ch in r['choices']))
    c.append('};'); c.append('')
    c.append(f'const {cls}::InfoType {cls}::theInfo = {{')
    c.append(f'    {cls}::create,')
    c.append(f'    {tag_hex(r["tag"])},')
    c.append('    0,')
    c.append('    ' + ('true' if r['extendable'] else 'false') + ',')
    c.append(f'    {cls}::selectionInfos,')
    c.append(f'    {r["numChoices"]}, {r["totalChoices"]},')
    c.append(f'    {cls}::selectionTags,' if r['tags'] else '    NULL,')
    c.append(f'    {cls}::selectionNames')
    c.append('};'); c.append('')

def emit_type(T, ind, hdr, cxx_out, name=None):
    r = types[T]
    cls = r['cxx']
    short = name or cls.split('::')[-1]
    hdr.append(f'{ind}//')
    hdr.append(f'{ind}// {cls}' + (f'   // {notes[T]}' if T in notes else ''))
    hdr.append(f'{ind}//')
    hdr.append('')
    k = r['kind']
    if k == 'SEQUENCE': emit_sequence(T, r, ind, short, hdr, cxx_out, cls)
    elif k == 'CHOICE': emit_choice(T, r, ind, short, hdr, cxx_out, cls)
    elif k == 'ENUMERATED':
        hdr.extend(emit_enum(r, ind, short)); hdr.append(''); cxx_out.extend(emit_enum_cxx(r, cls))
    elif k == 'STRING':
        hdr.extend(emit_string(r, ind, short)); hdr.append(''); cxx_out.extend(emit_string_cxx(r, cls))
    elif k == 'SEQUENCE_OF':
        hdr.extend(emit_seqof(r, ind, short)); hdr.append(''); cxx_out.extend(emit_seqof_cxx(r, cls))
    else:
        hdr.append(f'{ind}// unhandled kind {k} for {cls}')

import os
os.makedirs(OUT, exist_ok=True)
summary = []
for mod in sorted(by_module):
    Ts = [T for T in by_module[mod] if not types[T].get('nested')]
    Ts.sort(key=lambda T: (T >= DATA_END, T))
    # dependencies first (a field/element/selection type of this module must be declared before its user)
    def deps(T):
        r = types[T]; out = []
        def one(x):
            xr = types[x]
            if xr.get('nested'):
                out.extend(deps(x))
            elif xr['module'] == mod and x != T: out.append(x)
        for f in r.get('fields', []) + r.get('choices', []): one(f['info'])
        if r['kind'] == 'SEQUENCE_OF': one(r['element'])
        return out
    ordered = []; seen = set()
    def visit(T):
        if T in seen: return
        seen.add(T)
        for x in deps(T):
            if x in by_module[mod] and not types[x].get('nested'): visit(x)
        ordered.append(T)
    for T in Ts: visit(T)
    Ts = ordered
    # imports: other modules referenced
    imports = set()
    def walk_refs(T):
        r = types[T]
        refs = [f['info'] for f in r.get('fields', []) + r.get('choices', [])]
        if r['kind'] == 'SEQUENCE_OF': refs.append(r['element'])
        for x in refs:
            xr = types[x]
            if xr.get('nested'): walk_refs(x); continue
            m2 = modname(xr['cxx'])
            if m2 and m2 != mod: imports.add(m2)
    for T in by_module[mod]: walk_refs(T)
    hdr = [f'#ifndef __{mod.upper()}_H', f'#define __{mod.upper()}_H', '', '#include "asn1.h"', '']
    for im in sorted(imports): hdr.append(f'#include "{im}.h"')
    if imports: hdr.append('')
    hdr.append(f'namespace {mod} {{')
    hdr.append('')
    cxx = [f'#include "{mod}.h"', '', f'namespace {mod}{{', '']
    for T in Ts:
        emit_type(T, '', hdr, cxx)
    hdr.append(f'}} // namespace {mod}')
    hdr.append('')
    hdr.append(f'#endif // __{mod.upper()}_H')
    cxx.append(f'}} // namespace {mod}')
    open(f'{OUT}/{mod}.h', 'w', encoding='latin-1').write('\n'.join(hdr) + '\n')
    open(f'{OUT}/{mod}.cxx', 'w', encoding='latin-1').write('\n'.join(cxx) + '\n')
    summary.append((mod, len(Ts), sorted(imports)))
for s in summary: print(s)
print('synthesized/uncertain names:')
for T, n in notes.items(): print('  ', types[T]['cxx'], '-', n)
json.dump({str(k): v for k, v in types.items()}, open(SP + '/schema-named.json', 'w'), indent=1, ensure_ascii=False)
