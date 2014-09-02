import os, sys

os.system("dmapdump -d test.fitacf > test.dump")
os.system("dmapdump -d test2.fitacf > test2.dump")

f1 = open("test.dump","r")
f2 = open("test2.dump","r")

while True:
	l1 = f1.readline()
	l2 = f2.readline()
	if not l1 or not l2: 
		break
	if "origin.time" in l1 and "origin.time" in l2:
		continue
	if l1 != l2:
		print "l1",l1.strip()
		print "l2",l2.strip()
		print
		sys.exit(0)

if not l1 and l2:
	print "files differ, l2 is longer"
	sys.exit(0)
if not l2 and l1:
	print "files differ, l1 is longer"
	sys.exit(0)

print "files are the same!"

