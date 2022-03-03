#! /bin/bash
#./configuracio.sh -n nomFit -f 25 -c 30 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 4,5,-1.0,0.3
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
###################################
# Set comma as the delimiter
IFS=','
#-0 param checking
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
#-1 param checking
read -ra newarr <<< "$Iarg"
x=0
for val in "${newarr[@]}";
do
if [ $x -ne 4 ]
    then
        if [ $x -eq 0 ] && ([ $val -lt 2 ] || [ $val -gt 118 ])
        then    echo "Error al camp -1 param0 [2...118]"
                exit 5
        elif [ $x -eq 1 ] && ([ $val -lt 2 ] || [ $val -gt 35 ])
        then    echo "Error al camp -1 param1 [2...35]"
                exit 5
        #elif [ $x -eq 2 ] && ([ $(($val)) -lt -1 ] || [ $(($val)) -gt 1 ])
        #then    echo "Error al camp -1 param2 [-1.0...1.0] $(($val))"
        #        exit 5
        #elif [ $x -eq 3 ] && ([ $val -lt -1 ] || [ $val -gt 1 ])
        #then    echo "Error al camp -1 param3 [-1.0...1.0]"
        #        exit 5
        fi
        x=$(($x+1))
    else
        echo "Too many args at -1"
        exit 6
    fi
done
# resta checking
x=0     #max val per line (4)
y=0     #max lines (8)
IFS=' '
read -ra newarr2 <<< "$resta"
IFS=','
for args in "${newarr2[@]}";
do
        if [ $y -eq 8 ]
        then 
                echo "Max extra balls 8, too many args (lines) at resta"
                exit 6
        fi
        restat="${restat}\n$args"       #creates format to add at file of resta
        read -ra newarr <<< "$args"
        x=0
        for val in "${newarr[@]}";
        do
        if [ $x -ne 4 ]
        then
                if [ $x -eq 0 ] && ([ $val -lt 2 ] || [ $val -gt 118 ])
                then    echo "Error al camp resta param0 arg $y [2...118]"
                        exit 5
                elif [ $x -eq 1 ] && ([ $val -lt 2 ] || [ $val -gt 35 ])
                then    echo "Error al camp resta param1 arg $y [2...35]"
                        exit 5
                fi
                x=$(($x+1))
                else
                        echo "Too many args at resta arg $y"
                        exit 6
                fi
        done
        y=$(($y+1))
done
########################################################################################## floats checking missing TODO
printf "$farg $carg $parg\n$Oarg $marg\n$Iarg$restat" | tr ',' ' ' > $file   #tr (, -> '')