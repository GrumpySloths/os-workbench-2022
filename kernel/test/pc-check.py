import sys
import time

limit = int(sys.argv[1])
count, n = 0, 100000
while True:
    start = time.time()
    for ch in sys.stdin.read(n):
        if ch == '(': count += 1
        if ch == ')': count -= 1
        assert 0 <= count <= limit
    end = time.time()
    print(f'{n} Ok. time:{end-start}')
