#!/usr/bin/env python
import getopt
import sys

filearg=False
farg=-10
carg=-10
parg=-10
Oarg=-10
marg=-10
Iarg=-10
Oarg="-10,-10"
Iarg="-10,-10,-10,-10"

argv = sys.argv[1:]

try:
    opts, args = getopt.getopt(argv, "n:f:c:p:0:m:1:")
    #args is resta in bash
    for opt, arg in opts:
        if opt in ['-n']:
            filearg = arg
        if opt in ['-f']:
            farg = arg
        if opt in ['-c']:
            carg = arg
        if opt in ['-p']:
            parg = arg
        if opt in ['-0']:
            Oarg = arg
        if opt in ['-m']:
            marg = arg
        if opt in ['-1']:
            Iarg = arg
    print ("opts: "+str(opts)+"\nargs: "+str(args))
    print (filearg, args)
except:
    print("Incorrect args")
