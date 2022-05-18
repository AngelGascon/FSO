mur0 : mur0.c winsuport.o winsuport.h
	gcc -Wall mur0.c winsuport.o -o mur0 -lcurses

mur1 : mur1.c winsuport.o winsuport.h
	gcc -Wall mur1.c winsuport.o -o mur1 -lcurses -lpthread

mur2 : mur2.c winsuport.o winsuport.h
	gcc -Wall mur2.c winsuport.o -o mur2 -lcurses -lpthread

mur3 : mur3.c winsuport2.o winsuport2.h memoria.o memoria.h
	gcc -Wall -g mur3.c winsuport2.o memoria.o -o mur3 -lcurses -lpthread

winsuport.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o

pilota3 : pilota3.c winsuport2.o winsuport2.h memoria.o memoria.h
	gcc -Wall -g pilota3.c winsuport2.o memoria.o -ggdb -o pilota3 -lcurses -lpthread

clean : 
	rm -f winsuport.o mur0 mur1 mur2 mur3 pilota3

