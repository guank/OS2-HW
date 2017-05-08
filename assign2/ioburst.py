import os
import glob
from random import shuffle, getrandbits, randint

for i in xrange(10):
    name = "rand" + str(i) + ".txt"
    f = open(name,mode="w", buffering=0)
    f.write(os.urandom(1024*1024))
    f.close()

files = glob.glob("*.txt")
shuffle(files)

for i in xrange(32):
    rw = bool(getrandbits(1))
    if(rw):
        f = open(files[i%len(files)], mode="r+",buffering=0)
        f.seek(randint(0,102400))
        f.read(512)
        f.close()
    else:
        f = open(files[i%len(files)], mode="w+",buffering=0)
        f.seek(randint(0,102400))
        f.write(os.urandom(1024))
        f.close()
