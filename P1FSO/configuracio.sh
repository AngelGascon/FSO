#! /bin/bash
#./configuracio.sh -n nomFit -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 ,4,5,-1.0,0.3
nflag='false'
fflag='false'
cflag='false'
pflag='false'
Oflag='false'
mflag='false'
Iflag='false'
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
echo "argument -f $farg"
echo "argument -c $carg"
echo "argument -p $parg"
echo "argument -0 $Oarg"
echo "argument -m $marg"
echo "argument -1 $Iarg"
echo "arguments L4-8 $resta"
echo "fitxer = $file"
#-f param
if [ $farg -lt 10 ] || [ $farg -gt 120 ] || [ $fflag = 'false' ]
then    echo "Error al camp -f [10...120]"
        exit 1
#-c param
elif [ $carg -lt 10 ] || [ $carg -gt 36 ] || [ $cflag = 'false' ]
then    echo "Error al camp -c [10...36]"
        exit 2
#-p param
elif [ $parg -lt 8 ] || [ $parg -gt $(($farg-1)) ] || [ $pflag = 'false' ]
then    echo "Error al camp -p [8...(nFil - 1)]"
        exit 3
#-m param
elif [ $marg -lt 3 ] || [ $marg -gt $(($farg-1)) ] || [ $mflag = 'false' ]
then    echo "Error al camp -m [3...nFil-1]"
        exit 4
#if -0, -1 param exists
elif [ $Oflag = 'false' ] || [ $Iflag = 'false' ]
then    echo "Missing -0 -1 args"
        exit 5
fi
#-0, -1, resta  TODO
# Set comma as the delimiter
IFS=','
#-0 param checking
# Read the split words into an array
# based on space delimiter
read -ra newarr <<< "$Oarg"
x=0
for val in "${newarr[@]}";
do
    if [ $x -ne 2 ]
    then
        if ([ $val -lt 2 ] || [ $val -gt 118 ]) && [ $x -eq 0 ]
        then    echo "Error al camp -0 param0 [2...118]"
                exit 5
        elif ([ $val -lt 2 ] || [ $val -gt 35 ]) && [ $x -eq 1 ]
        then    echo "Error al camp -0 param1 [2...35]"
                exit 5
        fi
        x=$(($x+1))
    else
        echo "Too many args at -0"
        exit 6
    fi
done
######################################################################################



######################################################################################
printf "$farg $carg $parg\n$Oarg $marg\n$Iarg\n$resta\n" | tr ',' ' ' > $file   #tr , -> ''