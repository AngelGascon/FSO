if [ $(echo "0<-1.0" | bc) -eq 1 ]
    then    echo hi
fi
echo "-2.0<-1.0" | bc