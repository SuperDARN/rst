import os, sys

os.system("make_fit -new rawacf_1.rawacf > fitacf_1.fitacf")
os.system("dmapdump -d fitacf_1.fitacf > fitacf_1.dump")
#os.system("dmapdump -d test2.fitacf > test2.dump")

f1 = open("verified_fitacf_1.dump","r")
f2 = open("fitacf_1.dump","r")

x = 0
while True:
	l1 = f1.readline()
	l2 = f2.readline()
	x += 1
	if not l1 or not l2: 
		break
	if "origin.time" in l1 and "origin.time" in l2:
		continue
	
	if l1 != l2:
		print "Mismatch in line: ", x
		print "l1 text:",l1.strip()
		print "l2 text:",l2.strip()
		print "Exiting..."
		sys.exit(0)

if not l1 and l2:
	print "files differ, l2 is longer"
	sys.exit(0)
if not l2 and l1:
	print "files differ, l1 is longer"
	sys.exit(0)

print "Testing output matches known samples. fitacf functionality verified"

