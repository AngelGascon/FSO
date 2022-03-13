#!/usr/bin/env python3
#   python3 pyconfiguracio.py -n nomFit -f 25 -c 30 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 4,5,-1.0,0.3
import getopt
import sys
from os.path import exists as file_exists
#Defined to check -1 and resta args later
def isfloat(num):
    try:
        float(num)
        return True
    except ValueError:
        return False
argsflag='True'
filearg='False'
farg=-10
carg=-10
parg=-10
Oarg=-10
marg=-10
Iarg=-10
Oarg='-10,-10'
Iarg='-10,-10,-10,-10'
argv = sys.argv[1:]
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
    #getopts treats args as strings -> int()/float() casting needed prob
print ("opts: "+str(opts)+"\nargs: "+str(args))
if filearg == 'False':
    filearg = input("Enter file name: ")
if len(args) == 0:
    argsflag = 'False'
##Reading file and adding missing args
if file_exists(filearg):
    fit = open(filearg, "r")
    elems=0
    for line in fit:
        currentLine = line.split(" ")
        if elems == 0 and farg == -10:
            farg = currentLine[0]
        if elems == 0 and carg == -10:
            carg = currentLine[1]
        if elems == 0 and parg == -10:
            parg = currentLine[2].rstrip()
        if elems == 1 and Oarg == '-10,-10':
            Oarg = currentLine[0]+','+currentLine[1]
        if elems == 1 and marg == -10:
            marg = currentLine[2].rstrip()
        if elems == 2 and Iarg == '-10,-10,-10,-10':
            Iarg = currentLine[0]+','+currentLine[1]+','+currentLine[2]+','+currentLine[3]
        if argsflag == 'False' and elems > 2 and elems < 11:#8 max
            args.append(currentLine[0]+','+currentLine[1]+','+currentLine[2]+','+currentLine[3].rstrip())
        elems = elems+1
##########Checking args
x=0
while(x==0):
    Osplited=Oarg.split(',')
    Isplitted=Iarg.split(',')
    #-f param
    if farg.isnumeric()==False or int(farg)<10 or int(farg)>120:
        farg = input("Error al camp -f [10...120] ")
    #-c param
    elif carg.isnumeric()==False or int(carg)<10 or int(carg)>36:
        carg = input("Error al camp -c [10...36] ")
    #-p param
    elif parg.isnumeric()==False or int(parg)<8 or int(parg)>int(farg)-1:
        parg = input("Error al camp -p [8..."+str(int(farg)-1)+"] ")
    #-m param
    elif marg.isnumeric()==False or int(marg)<3 or int(marg)>int(farg)-1:
        marg = input("Error al camp -m [3..."+str(int(farg)-1)+"] ")
    #-0 param
    elif Osplited[0].isnumeric()==False or int(Osplited[0])<2 or int(Osplited[0])>118:
        Osplited[0] = input("Error al camp -0 param0 [2...118]")
        Oarg=Osplited[0]+","+Osplited[1]
    elif Osplited[1].isnumeric()==False or int(Osplited[1])<2 or int(Osplited[0])>35:
        Osplited[1] = input("Error al camp -0 param1 [2...35]")
        Oarg=Osplited[0]+","+Osplited[1]
    #-1 param
    elif Isplitted[0].isnumeric==False or int(Isplitted[0])<2 or int(Isplitted[0])>118:
        Isplitted[0] = input("Error al camp -1 param0 [2...118]")
        Iarg=Isplitted[0]+","+Isplitted[1]+","+Isplitted[2]+","+Isplitted[3]
    elif Isplitted[1].isnumeric()==False or int(Isplitted[1])<2 or int(Isplitted[0])>35:
        Isplitted[1] = input("Error al camp -1 param1 [2...35]")
        Iarg=Isplitted[0]+","+Isplitted[1]+","+Isplitted[2]+","+Isplitted[3]
    elif isfloat(Isplitted[2])==False or float(Isplitted[2])<-1.0 or float(Isplitted[2])>1.0:
        Isplitted[2] = input("Error al camp -1 param2 [-1.0 ... 1.0]")
        Iarg=Isplitted[0]+","+Isplitted[1]+","+Isplitted[2]+","+Isplitted[3]
    elif isfloat(Isplitted[3])==False or float(Isplitted[3])<-1.0 or float(Isplitted[3])>1.0:
        Isplitted[3] = input("Error al camp -1 param3 [-1.0 ... 1.0]")
        Iarg=Isplitted[0]+","+Isplitted[1]+","+Isplitted[2]+","+Isplitted[3]
    else:
        x=1
##########Checking resta
x=0
while(x<len(args) and x<8):
    restaSplitted=args[x].split(',')
    y=0
    while(y==0):
        if restaSplitted[0].isnumeric==False or int(restaSplitted[0])<2 or int(restaSplitted[0])>118:
            restaSplitted[0] = input("Error a resta camp "+str(x)+" param0 [2...118]")
            args[x]=restaSplitted[0]+","+restaSplitted[1]+","+restaSplitted[2]+","+restaSplitted[3]
        elif restaSplitted[1].isnumeric()==False or int(restaSplitted[1])<2 or int(restaSplitted[0])>35:
            restaSplitted[1] = input("Error al camp "+str(x)+" param1 [2...35]")
            args[x]=restaSplitted[0]+","+restaSplitted[1]+","+restaSplitted[2]+","+restaSplitted[3]
        elif isfloat(restaSplitted[2])==False or float(restaSplitted[2])<-1.0 or float(restaSplitted[2])>1.0:
            restaSplitted[2] = input("Error al camp "+str(x)+" param2 [-1.0 ... 1.0]")
            args[x]=restaSplitted[0]+","+restaSplitted[1]+","+restaSplitted[2]+","+restaSplitted[3]
        elif isfloat(restaSplitted[3])==False or float(restaSplitted[3])<-1.0 or float(restaSplitted[3])>1.0:
            restaSplitted[3] = input("Error al camp "+str(x)+" param3 [-1.0 ... 1.0]")
            args[x]=restaSplitted[0]+","+restaSplitted[1]+","+restaSplitted[2]+","+restaSplitted[3]
        else:
            y=1
    x=x+1
########## Write file
with open(filearg, 'w') as the_file:
    the_file.write(farg+" "+carg+" "+" "+parg+"\n"+Oarg.replace(","," ")+" "+marg+"\n"+Iarg.replace(","," "))