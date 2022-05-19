all : missatge.o semafor.o memoria.o mp_car_s mp_sincro winsuport2.o mp_car2 multiproc2 mp_car multiproc

multifil : multifil.c
	gcc -Wall multifil.c -o multifil -lpthread

mf_mutex : mf_mutex.c
	gcc -Wall mf_mutex.c -o mf_mutex -lpthread

multiproc : multiproc.c memoria.o semafor.o missatge.o
	gcc -Wall multiproc.c memoria.o semafor.o missatge.o -o multiproc -lcurses

mp_car : mp_car.c memoria.o semafor.o missatge.o
	gcc -Wall mp_car.c memoria.o semafor.o missatge.o -o mp_car -lcurses

multiproc2 : multiproc2.c winsuport2.o memoria.o semafor.o missatge.o
	gcc -Wall multiproc2.c winsuport2.o memoria.o semafor.o missatge.o -o multiproc2 -lcurses

mp_car2 : mp_car2.c winsuport2.o memoria.o semafor.o missatge.o
	gcc -Wall mp_car2.c winsuport2.o memoria.o semafor.o missatge.o -o mp_car2 -lcurses

winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

mp_sincro : mp_sincro.c memoria.o semafor.o missatge.o
	gcc -Wall mp_sincro.c memoria.o semafor.o missatge.o -o mp_sincro

mp_car_s : mp_car_s.c memoria.o semafor.o missatge.o
	gcc -Wall mp_car_s.c memoria.o semafor.o missatge.o -o mp_car_s

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o 

semafor.o : semafor.c semafor.h
	gcc -c -Wall semafor.c -o semafor.o 

missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o

clean:
	rm missatge.o memoria.o semafor.o mp_car_s mp_sincro winsuport2.o mp_car multiproc mp_car2 multiproc2 
