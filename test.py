import subprocess
import time


argList = ["simple_testcase1","simple_testcase2","simple_testcase3","simple_testcase4","simple_testcase5","error_testcase1","error_testcase2","error_testcase3","error_testcase4","advanced_testcase1","advanced_testcase2","advanced_testcase3","advanced_testcase4"]

# simplearglist = ["simple_testcase1","simple_testcase2","simple_testcase3","simple_testcase4","simple_testcase5"]
# errorarglist = ["error_testcase1","error_testcase2","error_testcase3","error_testcase4"]
# advancedarglist = ["advanced_testcase1","advanced_testcase2","advanced_testcase3","advanced_testcase4"]
#ar = "NAME=simple_testcase2 "

#print 'running make'


for args in argList:
	cmd = 'make '# NAME='+ args 
	cmd = cmd+" "+"NAME="+args
	print(args)
	if args != "advanced_testcase1" or args != "advanced_testcase2":
		raw_input("")
		subprocess.call(cmd,shell =True)
		subprocess.call("./Prog4Test")
	
	print ("////////////////////////////////////////////////////////////////////////\n\n")
	