"""Split a prettier-formatted glue into top-level statements keyed by defined name; report names defined in both files whose text differs,
and names only in one file. usage: jsdefs.py a.js b.js"""
import sys,re
def split(path):
    lines=open(path).read().split('\n')
    stmts=[]; cur=[]
    for ln in lines:
        if ln and not ln[0].isspace() and cur and not ln.startswith('}') and not ln.startswith(')') and not ln.startswith(']'):
            stmts.append('\n'.join(cur)); cur=[]
        cur.append(ln)
    if cur: stmts.append('\n'.join(cur))
    d={}; anon=0
    for s in stmts:
        m=re.match(r'(?:var|let|const|function|class|async function)\s+([A-Za-z_$][\w$]*)',s)
        if m: k=m.group(1)
        else:
            m=re.match(r'(Module\["[^"]+"\]|[A-Za-z_$][\w$.]*)\s*=',s)
            k=m.group(1) if m else f'<anon{anon}>'; anon+=1
        if k in d: k=k+'#dup'
        d[k]=s
    return d
a=split(sys.argv[1]); b=split(sys.argv[2])
both=[k for k in a if k in b]
diff=[k for k in both if a[k]!=b[k]]
print(f'defs A={len(a)} B={len(b)} common={len(both)} differing={len(diff)}')
print('only in A:', [k for k in a if k not in b])
print('only in B:', [k for k in b if k not in a])
for k in diff:
    print('='*20,k); print('--- A'); print(a[k][:1500]); print('--- B'); print(b[k][:1500])
