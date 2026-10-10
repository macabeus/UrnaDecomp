import sys, struct
mem=open('mem.bin','rb').read()
def cstr(a):
    e=mem.index(b'\0',a); return mem[a:e].decode('utf-8','replace')
needle=sys.argv[1].encode()+b'\0'
addrs=[i for i in range(len(mem)) if mem.startswith(needle,i) and (i==0 or mem[i-1]==0 or True)]
# exact string start: the pointer array points here; find pointers to any occurrence
for a in addrs:
    p=struct.pack('<I',a)
    hits=[i for i in range(0,len(mem)-3) if mem[i:i+4]==p and i%4==0]
    for h in hits:
        # walk back and forward while entries are pointers to printable strings
        def ok(x):
            v=struct.unpack_from('<I',mem,x)[0]
            if not(1024<=v<len(mem)): return False
            try:
                s=cstr(v); return 1<=len(s)<200 and all(32<=ord(c)<127 for c in s)
            except: return False
        lo=h
        while lo-4>=0 and ok(lo-4): lo-=4
        hi=h
        while hi+4<len(mem) and ok(hi+4): hi+=4
        n=(hi-lo)//4+1
        print(f'string @{a:#x} ptr@{h:#x} array {lo:#x}..{hi:#x} n={n}')
        if n<400:
            for x in range(lo,hi+1,4):
                print('  ',cstr(struct.unpack_from('<I',mem,x)[0]))
