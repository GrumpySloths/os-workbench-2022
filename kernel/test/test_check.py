import sys
import time
malloclist=[]
def malloc_check(area:list)->bool:
    if len(malloclist)==0:
        malloclist.append(area)
        return True
    for section in malloclist:
        if area[0]>=section[0] and area[0]<section[1]:
            print("debug1")
            return False
        if area[1]>section[0] and area[1]<=section[1]:
            print("debug2")
            return False
    
    malloclist.append(area)
    return True
def free_check(area:list)->bool:
    if area in malloclist:
        malloclist.remove(area)
        return True
    return False

count=0
while True:
    line=sys.stdin.readline()
    if line.startswith("malloc"):
        tmp=line.split(",")
        area=[int(tmp[1],16),int(tmp[2],16)]
        size=int(tmp[3],16)
        print("malloc:",area)
        if area[0]%size!=0:
            print("malloc failure,align principle against\n")
            break
        if not malloc_check(area):
            print("malloc failure")
            break
    elif line.startswith("free"):
        tmp=line.split(",")
        area=[int(tmp[1],16),int(tmp[2],16)] 
        print("free:",area)
        if not free_check(area):
            print("free failure")
            break
    else:
        continue
    count+=1
    print("success count:",count)
        
    