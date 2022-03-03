rm camp1.txt
echo "1 test"
./configuracioV2.sh -n camp1.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
cat camp1.txt
echo "2 test"
./configuracioV2.sh -n camp1.txt -f 8 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
cat camp1.txt
echo "3 test"
./configuracioV2.sh -n camp1.txt -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
cat camp1.txt
echo "4 test"
./configuracioV2.sh -n camp1.txt -f 50 -c 80 -p 20
cat camp1.txt
echo "5 test"
./configuracioV2.sh -n camp1.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 11,22,-1.0,0.3
cat camp1.txt