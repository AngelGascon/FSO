#! /bin/bash
#./configuracio.sh -n nomFit -f 25 -c 30 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 4,5,-1.0,0.3
nflag='false'
fflag='false'
cflag='false'
pflag='false'
Oflag='false'
mflag='false'
Iflag='false'
Oarg=""
Iarg=""
#Flags to check if the parametres exist
while getopts 'n:f:c:p:0:m:1:' opcio; do
    case "${opcio}" in
        n) echo "File: " ;nflag='true'; file="${OPTARG}" ; echo $OPTIND;;
        f) echo "Linea1 -f" ;fflag='true'; farg="${OPTARG}" ; echo $OPTIND;;
        c) echo "Linea1 -c" ;cflag='true'; carg="${OPTARG}"; echo $OPTIND;;
        p) echo "Linea1 -p" ;pflag='true'; parg="${OPTARG}"; echo $OPTIND;;
        0) echo "Linea2 -0" ;Oflag='true'; Oarg="${OPTARG}"; echo $OPTIND;;
        m) echo "Linea2 -m" ;mflag='true'; marg="${OPTARG}"; echo $OPTIND;;
        1) echo "Linea3 -1" ;Iflag='true'; Iarg="${OPTARG}"; echo $OPTIND;;
        *) error "Unexpected option ${opcio}" ;;
    esac
done
shift $(($OPTIND - 1))
resta=$*
#If user intro extra args (8 balls)
if [ "$resta" == "" ]
then
    rflag='false'
else
    rflag='true'
fi
echo $rflag
echo "argument -f $farg"
echo "argument -c $carg"
echo "argument -p $parg"
echo "argument -0 $Oarg"
echo "argument -m $marg"
echo "argument -1 $Iarg"
echo "arguments l4-8 $resta"
echo "fitxer = $file"
##########################################################################################
#Missing -n file param
if [ $nflag == 'false' ]
then
    read -p "Enter file name: " file 
fi
#file name check
if [ -f "$file" ]
then
    #existing file / read file -> if a parameter is missing add variable
    #READING FILE
    elems=0
    elemsResta=0
    fileInfo=$(cat $file)
    for line in $fileInfo
    do
        #-f
        if [ $elems -eq 0 ] && [ $fflag = 'false' ]
        then
            farg=$line
        #-c
        elif [ $elems -eq 1 ] && [ $cflag = 'false' ]
        then
            carg=$line
        #-p
        elif [ $elems -eq 2 ] && [ $pflag = 'false' ]
        then
            parg=$line
        #-0
        elif ([ $elems -eq 3 ] || [ $elems -eq 4 ]) && [ $Oflag = 'false' ]
        then
            Oarg="${Oarg},$line"
        #-m
        elif [ $elems -eq 5 ] && [ $mflag = 'false' ]
        then
            marg=$line
        #-1
        elif ([ $elems -gt 5 ] && [ $elems -lt 10 ]) && [ $Iflag = 'false' ]
        then
            Iarg="${Iarg},$line"
        #Resta 8lines, will only read if user didn't pass extra args
        elif ([ $elems -gt 9 ] && [ $elems -lt 32 ]) && [ $rflag = 'false' ]
        then
            resta="${resta} $line"
        fi
        elems=$(($elems+1))
    done
fi
######Check and asks parameters i error
x=0
while [ $x -eq 0 ]
do
    ##### 0, 1 and resta divisions $(echo $algu | cut -d, -f1)
    Opri=$(echo $Oarg | cut -d, -f1)
    Oseg=$(echo $Oarg | cut -d, -f2)
    Ipri=$(echo $Iarg | cut -d, -f1)
    Iseg=$(echo $Iarg | cut -d, -f2)
    Iter=$(echo $Iarg | cut -d, -f3)
    Iqat=$(echo $Iarg | cut -d, -f4)
    #####
    #-f param
    if [ $farg -lt 10 ] || [ $farg -gt 120 ] || [ fflag = 'false' ]
    then    read -p "Error al camp -f [10...120]" farg
    #-c param
    elif [ $carg -lt 10 ] || [ $carg -gt 36 ]
    then    read -p "Error al camp -c [10...36]" carg
    #-p param
    elif [ $parg -lt 8 ] || [ $parg -gt $(($farg-1)) ]
    then    read -p "Error al camp -p [8...(nFil - 1)]" parg
    #-m param
    elif [ $marg -lt 3 ] || [ $marg -gt $(($farg-1)) ]
    then    read -p "Error al camp -m [3...nFil-1]" marg
    #-0 param
    elif ([ $Opri -lt 2 ] || [ $Opri -gt 118 ])
    then    read -p "Error al camp -0 param0 [2...118]" Opri
            Oarg="$Opri,$Oseg"
    elif ([ $Oseg -lt 2 ] || [ $Oseg -gt 35 ])
    then    read -p "Error al camp -0 param1 [2...35]" Oseg
            Oarg="$Opri,$Oseg"
    #-1 param
    elif ([ $Ipri -lt 2 ] || [ $Ipri -gt 118 ])
    then    read -p "Error al camp -1 param0 [2...118]" Ipri
            Iarg="$Ipri,$Iseg,$Iter,$Iqat"
    elif ([ $Iseg -lt 2 ] || [ $Iseg -gt 35 ])
    then    read -p "Error al camp -1 param1 [2...35]" Iseg
            Iarg="$Ipri,$Iseg,$Iter,$Iqat"
    ##TODO check reals, Iter Iqat
    else    x=1
    fi
done
##Checking resta

##
echo $Iarg
echo $resta
echo $Oarg
########################################################################################## floats checking missing TODO
printf "$farg $carg $parg\n$Oarg $marg\n$Iarg$restat" | tr ',' ' ' > $file   #tr (, -> '')