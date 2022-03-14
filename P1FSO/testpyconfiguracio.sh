rm camp1Py.txt
echo "1 test: python pyconfiguracio.py -n camp1Py.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3"
python pyconfiguracio.py -n camp1Py.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
echo "camp1Py.txt:"
cat camp1Py.txt
echo ""
echo "--------------------------------------"
echo "2 test: python pyconfiguracio.py -n camp1Py.txt -f 8 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3"
python pyconfiguracio.py -n camp1Py.txt -f 8 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
echo "camp1Py.txt:"
cat camp1Py.txt
echo ""
echo "--------------------------------------"
echo "3 test: python pyconfiguracio.py -n camp1Py.txt -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3"
python pyconfiguracio.py -n camp1Py.txt -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
echo "camp1Py.txt:"
cat camp1Py.txt
echo ""
echo "--------------------------------------"
echo "4 test: python pyconfiguracio.py -n camp1Py.txt -f 50 -c 80 -p 20"
python pyconfiguracio.py -n camp1Py.txt -f 50 -c 80 -p 20
echo "camp1Py.txt:"
cat camp1Py.txt
echo ""
echo "--------------------------------------"
echo "5 test: python pyconfiguracio.py -n camp1Py.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 11,22,-2.0,0.3"
python pyconfiguracio.py -n camp1Py.txt -f 25 -c 40 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3 11,22,-2.0,0.3
echo "camp1Py.txt:"
cat camp1Py.txt
echo ""
echo "--------------------------------------"
echo "6 test: python pyconfiguracio.py -n camp1Py.txt"
python pyconfiguracio.py -n camp1Py.txt
echo "camp1Py.txt:"
cat camp1Py.txt
echo ""
echo "--------------------------------------"