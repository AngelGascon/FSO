#! /bin/bash
rflag='false'
while getopts 'n:f:c:p:0:m:1:' opcio; do
    case "${opcio}" in
        n) echo "File: " ;file="${OPTARG}" ; echo $OPTIND;;
        f) echo "Linea1 -f" ;farg="${OPTARG}" ; echo $OPTIND;;
        c) echo "Linea1 -c" ;carg="${OPTARG}"; echo $OPTIND;;
        p) echo "Linea1 -p" ;parg="${OPTARG}"; echo $OPTIND;;
        0) echo "Linea2 -0" ;Oarg="${OPTARG}"; echo $OPTIND;;
        m) echo "Linea2 -m" ;marg="${OPTARG}"; echo $OPTIND;;
        1) echo "Linea3 -1" ;Iarg="${OPTARG}"; echo $OPTIND;;
        #r) echo "Resta" ;resta="${OPTARG}"; echo $OPTIND;;  #Realment ho volem aixi¿?
        *) error "Unexpected option ${opcio}" ;;
    esac
done
shift $(($OPTIND - 1))
resta=$*
echo "argument -f $farg"
echo "argument -c $carg"
echo "argument -p $parg"
echo "argument -0 $Oarg"
echo "argument -m $marg"
echo "argument -1 $Iarg"
echo "arguments L4-8 $resta"
echo "fitxer= $file"
printf "encara ens falta tractar els següents elements: %s\n$* \n"
#COMPROVACIONS I TRACTAMENT DE RESTA
printf "$farg $carg $parg\n$Oarg $marg\n$Iarg\n$resta\n" | tr ',' ' ' > $file   #tr canvia comes per espais