import sys
mem=open('mem.bin','rb').read()
for t in sys.argv[1:]:
    n=t.encode()+b'\0'
    i=0
    while True:
        i=mem.find(n,i)
        if i<0: break
        s=i
        while s>0 and mem[s-1]!=0: s-=1
        print(f"{t}\taddr={i}\tfullstring={mem[s:i+len(n)-1].decode('latin1')!r}")
        i+=1
