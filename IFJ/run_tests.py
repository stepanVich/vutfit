#!/bin/python

import os, subprocess

files = []

for test in os.listdir("testy"):
    if test.endswith (".txt"):
        files.append(test)

files.sort ()

def checkCode (code, ret):
    if ret == code:
        return "ano\t"
    else:
        return "NE - (" + str(ret) + ")\t"

print ("no.\ttest\t\treturncode\tstdout")
print ("=================================================")

counter = 1

for test in files:
    print '%3d\t' % (counter),
    counter += 1

    if test[:3] == "cor":
        code = 0
    elif test[:4] == "wlex":
        code = 1
    elif test[:4] == "wsyn":
        code = 2
    elif test[:4] == "wsem":
        if test[4] == "3":
            code = 3
        elif test[4] == "4":
            code = 4
        elif test[4] == "6":
            code = 6
        else:
            print (test + "\t error")
    elif test[:4] == "wint":
	if test[4] == "7":
	    code = 7
	elif test[4] == "8":
	    code = 8
	elif test[4] == "9":
	    code = 9
	elif test[4] == "1":
	    code = 10
        else:
            print (test + "\t error")
    else:
        print (test + "\t error")

    if os.path.exists ("testy/" + test[:-4] + ".stdin"):
        vstup = open ("testy/" + test[:-4] + ".stdin")
    else:
        vstup = None

    if test[:3] == "cor":
        out = open ("testy/" + test[:5] + "_test.stdout", "w")
        run  = subprocess.call (["./interpret", "testy/" + test], stdout=out, stdin=vstup, stderr=subprocess.PIPE)
        out.close()
        print (test + "\t" + str(code) + " - "),
        print (checkCode (code, run)),
        diff = subprocess.call (["diff", "testy/" + test[:5] + ".stdout", "testy/" + test[:5] + "_test.stdout"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if diff == 0:
            print ("ano")
        else:
            print ("NE")
    else:
        run  = subprocess.call (["./interpret", "testy/" + test], stdin=vstup, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
        print (test + "\t" + str(code) + " - "),
        print (checkCode (code, run)),
        print ("-")
