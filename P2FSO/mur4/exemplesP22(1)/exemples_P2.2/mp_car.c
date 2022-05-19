/************************************************************************/
/*				mp_car.c				*/
/*									*/
/*	Compilar:							*/
/*		$ gcc  -Wall mp_car.c memoria.o -o mp_car1		*/
/*									*/
/*	Escriu per la sortida estandard un caracter identificatiu	*/
/*      segons l'argument 'n_car' (1 -> 'a', 2 -> 'b', ...), tantes	*/
/*	vegades com indica el segon argument 'n_vegades', esperant	*/
/*	un temps aleatori entre dues visualitzacions; el programa	*/
/*	acaba quan s'han escrit tantes lletres com indiqui el		*/
/*	contingut de la zona de memoria compartida identificada pel	*/
/*	tercer argument 'id_lletres'.					*/
/*									*/
/*	Aquest programa s'executara com un proces fill del programa	*/
/*	'multiproc' (veure fitxer 'multiproc.c').			*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"


int main(int n_args, char *ll_args[])
{
  int i,nc,nv;
  int *p_lletres, id_lletres;

  if (n_args < 3)
  {   fprintf(stderr,"proces: mp_car  n_car  n_vegades  id_lletres\n");
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

  setbuf(stdout,NULL);	/* anular buffer de sortida estandard */
  srand(getpid());

  for (i=0; i < nv; i++)	/* per a totes les vegades (nv) */
  {
    if (*p_lletres > 0)			/* si falten lletres */
    {
	printf("%c",('a'+nc-1));	/* escriure marca del proces */
	sleep(1 + rand() % 3);			/* dormir entre 1 i 3 segons */
 	(*p_lletres)--;				/* una lletra menys */
     }
     else exit(i);	/* sino, provoca la sortida anticipada del proces */
  }
  return(i);	/* retorna el numero de lletres que ha impres el proces */
}
