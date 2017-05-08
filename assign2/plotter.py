s = open("qemulog.txt").read()
xs = [x.replace('\r','').replace('SSTF DISPATCHING ','') for x in s.split('\n') if 'SSTF ADDING REQ' in x]
