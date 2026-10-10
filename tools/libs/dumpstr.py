import sys, re
data = open(sys.argv[1],'rb').read()
needle = sys.argv[2].encode()
before = int(sys.argv[3]); after = int(sys.argv[4])
for m in re.finditer(re.escape(needle), data):
    i = m.start()
    chunk = data[max(0,i-before):i+after]
    base = max(0,i-before)
    off = 0
    for part in chunk.split(b'\0'):
        if part:
            try: s = part.decode('utf-8')
            except: s = repr(part)
            print(f"{base+off:#x}\t{s}")
        off += len(part)+1
    print('----')
