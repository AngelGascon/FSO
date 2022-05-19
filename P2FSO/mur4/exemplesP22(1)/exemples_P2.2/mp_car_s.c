/************************************************************************/
/*				mp_car_s.c				*/
/*									*/
/*	Compilar i executar:						*/
/*	gcc -Wall mp_car_s.c memoria.o semafor.o missatge.o -o mp_car_s	*/
/*	./mp_car_s  n_car  n_vegades  id_lletres  id_sem  id_bustia	*/
/*									*/
/*	Escriu per la sortida estandard un caracter identificatiu	*/
/*      segons l'argument 'n_car' (1 -> 'a', 2 -> 'b', ...), tantes	*/
/*	vegades com indica el segon argument 'n_vegades', esperant	*/
/*	un temps aleatori entre dues visualitzacions; el programa	*/
/*	acaba quan s'han escrit tantes lletres com indiqui el		*/
/*	contingut de la zona de memoria compartida identificada pel	*/
/*	tercer argument 'id_lletres'.					*/
/*	A mes, utilitza el semafor indicat en 'id_sem' per realitzar	*/
/*	exclusio mutua a l'hora d'accedir a la variable compartida.	*/
/*	Finalment, totes les lletres que genera les envia a la bustia	*/
/*	especifica en el cinque argument.				*/
/*									*/
/*	Aquest programa s'executara com un proces fill del programa	*/
/*	'mp_sincro' (veure fitxer 'mp_sincro.c').			*/
/************************************************************************/
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "memoria.h"
#include "semafor.h"
#include "missatge.h"


int main(int n_args, char *ll_args[])
{
  int i,nc,nv;
  int *p_lletres, id_lletres;
  int id_sem, id_bustia;
  char mis[2];

  if (n_args < 6)
  {   fprintf(stderr,"proces: mp_car_s n_car n_vegades id_lletres id_sem id_bustia\n");
	exit(0);
  }
  nc = atoi(ll_args[1]);
  nv = atoi(ll_args[2]);

  id_lletres = atoi(ll_args[3]);
  p_lletres = map_mem(id_lletres);	/* obtenir adres. de mem. compartida */
  if (p_lletres == (int*) -1)
  {   fprintf(stderr,"proces (%d): error en identificador de memoria\n",(int)getpid());
	exit(0);
  }
  id_sem = atoi(ll_args[4]);		/* obtenir identificador de semafor */
  id_bustia = atoi(ll_args[5]);		/* obtenir identificador de bustia */

  srand(getpid());
  for (i=0; i < nv; i++)	/* per a totes les vegades (nv) */
  {
    sleep(1 + rand() % 3);	/* dormir entre 1 i 3 segons */
    waitS(id_sem);		/* tanca semafor */
    if (*p_lletres > 0)		/* si falten lletres */
    {
	sprintf(mis,"%c",('a'+nc-1));	/* genera marca del proces */
	sendM(id_bustia,mis,2);		/* envia marca al pare */
 	(*p_lletres)--;			/* una lletra menys */
 	signalS(id_sem); 		/* obre semafor */
     }
     else
     {
 	signalS(id_sem); 		/* obre semafor */
	exit(i);	/* provoca la sortida anticipada del proces */
     }
  }
  return(i);	/* retorna el numero de lletres que ha impres el proces */
}
