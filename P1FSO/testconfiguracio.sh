rm camp1.txt
echo "1 test: ./configuracio.sh -n camp1.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3"
./configuracio.sh -n camp1.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
echo "camp1.txt:"
cat camp1.txt
echo ""
echo "--------------------------------------"
echo "2 test: ./configuracio.sh -n camp1.txt -f 8 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3"
./configuracio.sh -n camp1.txt -f 8 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
echo "camp1.txt:"
cat camp1.txt
echo ""
echo "--------------------------------------"
echo "3 test: ./configuracio.sh -n camp1.txt -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3"
./configuracio.sh -n camp1.txt -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
echo "camp1.txt:"
cat camp1.txt
echo ""
echo "--------------------------------------"
echo "4 test: ./configuracio.sh -n camp1.txt -f 50 -c 80 -p 20"
./configuracio.sh -n camp1.txt -f 50 -c 80 -p 20
echo "camp1.txt:"
cat camp1.txt
echo ""
echo "--------------------------------------"
echo "5 test: ./configuracio.sh -n camp1.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 11,22,-2.0,0.3"
./configuracio.sh -n camp1.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 11,22,-2.0,0.3
echo "camp1.txt:"
cat camp1.txt
echo ""
echo "--------------------------------------"
echo "6 test: ./configuracio.sh -n camp1.txt"
./configuracio.sh -n camp1.txt 11,22,1.0,0.5 5,7,-1.0,0.3 5,3,-1.0,0.3 8,4,-1.0,0.3 50,20,-1.0,0.5 55,3,-1.0,0.3 1,3,-1.0,0.3 1,3,-1.0,0.3 1,3,-1.0,0.3 1,3,-1.0,0.3 1,3,-1.0,0.3 30,13,0.0,0.3
echo "camp1.txt:"
cat camp1.txt
echo ""
echo "--------------------------------------"
