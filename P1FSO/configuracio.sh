#! /bin/bash
#./configuracioV2.sh -n nomFit -f 25 -c 30 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 4,5,-1.0,0.3
#if bc error -> sudo apt-get install bc 
file='false'
farg=-10
carg=-10
parg=-10
Oarg=-10
marg=-10
Iarg=-10
Oarg="-10,-10"
Iarg="-10,-10,-10,-10"
#Flags to check if the parametres exist
while getopts 'n:f:c:p:0:m:1:' opcio; do
    case "${opcio}" in
        n) echo "File: " ; file="${OPTARG}" ; echo $OPTIND;;
        f) echo "Linea1 -f" ; farg="${OPTARG}" ; echo $OPTIND;;
        c) echo "Linea1 -c" ; carg="${OPTARG}"; echo $OPTIND;;
        p) echo "Linea1 -p" ; parg="${OPTARG}"; echo $OPTIND;;
        0) echo "Linea2 -0" ; Oarg="${OPTARG}"; echo $OPTIND;;
        m) echo "Linea2 -m" ; marg="${OPTARG}"; echo $OPTIND;;
        1) echo "Linea3 -1" ; Iarg="${OPTARG}"; echo $OPTIND;;
        *) error "Unexpected option ${opcio}";
    esac
done
echo $Oarg
shift $(($OPTIND - 1))
resta=$*
#If user intro extra args (8 balls)
if [ "$resta" == "" ]
then
    rflag='false'
else
    rflag='true'
fi
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
if [ $file = 'false' ]
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
        if [ $elems -eq 0 ] && [ $farg -eq -10 ]
        then
            farg=$line
        #-c
        elif [ $elems -eq 1 ] && [ $carg -eq -10 ]
        then
            carg=$line
        #-p
        elif [ $elems -eq 2 ] && [ $parg -eq -10 ]
        then
            parg=$line
        #-0
        elif ([ $elems -eq 3 ] || [ $elems -eq 4 ]) && [ $Oarg = "-10,-10" ]
        then
            Oarg="${Oarg},$line"
        #-m
        elif [ $elems -eq 5 ] && [ $marg -eq -10 ]
        then
            marg=$line
        #-1
        elif ([ $elems -gt 5 ] && [ $elems -lt 10 ]) && [ $Iarg = "-10,-10,-10,-10" ]
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
######Check and asks parameters i error //TODO flags if non existing file
x=0
while [ $x -eq 0 ]
do
    ##### -0, -1 cuts $(echo $var | cut -d, -f1...)
    Opri=$(echo $Oarg | cut -d, -f1)
    Oseg=$(echo $Oarg | cut -d, -f2)
    Ipri=$(echo $Iarg | cut -d, -f1)
    Iseg=$(echo $Iarg | cut -d, -f2)
    Iter=$(echo $Iarg | cut -d, -f3)
    Iqat=$(echo $Iarg | cut -d, -f4)
    if ([ -z "$Opri" ] || [ -z "$Oseg" ])
    then 
        Opri=-10
        Oseg=-10
    elif ([ -z "$Ipri" ] || [ -z "$Iseg" ] || [ -z "$Iter" ] || [ -z "$Iqat" ])
    then 
        Ipri=-10
        Iseg=-10
        Iter=-10
        Iqat=-10
    fi
    ##### ! [[ $carg =~ ^[0-9]+$ ]] -> tests if its an integer
    ##### ! [[ $carg =~ ^[+-]?[0-9]*\.?[0-9]+$ ]] -> tests if its a float 
    #-f param
    if ! [[ $farg =~ ^[0-9]+$ ]] || [ $farg -lt 10 ] || [ $farg -gt 120 ]
    then    read -p "Error al camp -f [10...120]" farg
    #-c param
    elif ! [[ $carg =~ ^[0-9]+$ ]] || [ $carg -lt 10 ] || [ $carg -gt 36 ]
    then    read -p "Error al camp -c [10...36]" carg
    #-p param
    elif ! [[ $parg =~ ^[0-9]+$ ]] || [ $parg -lt 8 ] || [ $parg -gt $(($farg-1)) ]
    then    read -p "Error al camp -p [8...($farg - 1)]" parg
    #-m param
    elif ! [[ $marg =~ ^[0-9]+$ ]] || [ $marg -lt 3 ] || [ $marg -gt $(($farg-1)) ]
    then    read -p "Error al camp -m [3...$farg-1]" marg
    #-0 param
    elif ! [[ $Opri =~ ^[0-9]+$ ]] || [ $Opri -lt 2 ] || [ $Opri -gt 118 ]
    then    read -p "Error al camp -0 param0 [2...118]" Opri
            Oarg="$Opri,$Oseg"
    elif ! [[ $Oseg =~ ^[0-9]+$ ]] || [ $Oseg -lt 2 ] || [ $Oseg -gt 35 ]
    then    read -p "Error al camp -0 param1 [2...35]" Oseg
            Oarg="$Opri,$Oseg"
    #-1 param -> Iter Iqat real numbers
    elif ! [[ $Ipri =~ ^[0-9]+$ ]] || [ $Ipri -lt 2 ] || [ $Ipri -gt 118 ]
    then    read -p "Error al camp -1 param0 [2...118]" Ipri
            Iarg="$Ipri,$Iseg,$Iter,$Iqat"
    elif ! [[ $Iseg =~ ^[0-9]+$ ]] || [ $Iseg -lt 2 ] || [ $Iseg -gt 35 ]
    then    read -p "Error al camp -1 param1 [2...35]" Iseg
            Iarg="$Ipri,$Iseg,$Iter,$Iqat"
    elif ! [[ $Iter =~ ^[+-]?[0-9]*\.?[0-9]+$ ]] || [ $(echo "$Iter<-1.0" | bc) -eq 1 ] || [ $(echo "$Iter>1.0" | bc) -eq 1 ]
    then    read -p "Error al camp -1 param2 [-1.0 ... 1.0]" Iter
            Iarg="$Ipri,$Iseg,$Iter,$Iqat"
    elif ! [[ $Iqat =~ ^[+-]?[0-9]*\.?[0-9]+$ ]] || [ $(echo "$Iqat<-1.0" | bc) -eq 1 ] || [ $(echo "$Iqat>1.0" | bc) -eq 1 ]
    then    read -p "Error al camp -1 param3 [-1.0 ... 1.0]" Iqat
            Iarg="$Ipri,$Iseg,$Iter,$Iqat"
    else    x=1
    fi
done
##Checking resta
y=0 #max lines (8)
#Set delimiter
IFS=' '
read -ra newarr <<< "$resta"
restat=""
for args in "${newarr[@]}";
do
        #Divide resta args
        Ipri=$(echo $args | cut -d, -f1)
        Iseg=$(echo $args | cut -d, -f2)
        Iter=$(echo $args | cut -d, -f3)
        Iqat=$(echo $args | cut -d, -f4)
        if ([ -z "$Ipri" ] || [ -z "$Iseg" ] || [ -z "$Iter" ] || [ -z "$Iqat" ])
        then 
            Ipri=-10
            Iseg=-10
            Iter=-10
            Iqat=-10
        fi
        #Check resta args
        x=0
        while [ $x -eq 0 ]
        do
            if ! [[ $farg =~ ^[0-9]+$ ]] || [ $Ipri -lt 2 ] || [ $Ipri -gt 118 ]
            then    read -p "Error a resta param0 del camp$y [2...118]" Ipri
                    args="$Ipri,$Iseg,$Iter,$Iqat"
            elif ! [[ $farg =~ ^[0-9]+$ ]] || [ $Iseg -lt 2 ] || [ $Iseg -gt 35 ]
            then    read -p "Error a resta param1 del camp$y [2...35]" Iseg
                    args="$Ipri,$Iseg,$Iter,$Iqat"
            elif ! [[ $Iqat =~ ^[+-]?[0-9]*\.?[0-9]+$ ]] || [ $(echo "$Iter<-1.0" | bc) -eq 1 ] || [ $(echo "$Iter>1.0" | bc) -eq 1 ]
            then    read -p "Error a resta param2 del camp$y [-1.0 ... 1.0]" Iter
                    args="$Ipri,$Iseg,$Iter,$Iqat"
            elif ! [[ $Iqat =~ ^[+-]?[0-9]*\.?[0-9]+$ ]] || [ $(echo "$Iqat<-1.0" | bc) -eq 1 ] || [ $(echo "$Iqat>1.0" | bc) -eq 1 ]
            then    read -p "Error a resta param3 del camp$y [-1.0 ... 1.0]" Iqat
                    args="$Ipri,$Iseg,$Iter,$Iqat"
            else    x=1
            fi
        done
        restat="${restat}\n$args"
        y=$(($y+1))
        if [ $y -eq 8 ]
        then
            break
        fi
done
##########################################################################################
printf "$farg $carg $parg\n$Oarg $marg\n$Iarg$restat" | tr ',' ' ' > $file   #tr (, -> '')