/************************************************************************/
/*				mp_car2.c				*/
/*									*/
/*   Compilar:								*/
/*      gcc -Wall mp_car2.c memoria.o winsuport2.o -o mp_car2 -lcurses	*/
/*									*/
/*	accedeix a una finestra de CURSES previament creada (des del    */
/*      proces 'multiproc2.c', per exemple), i visualitza un caracter   */
/*      identificatiu per la fila indicada per argument (fila 1 -> 'a',	*/
/*	fila 2 -> 'b', ...) tantes vegades com el tercer argument	*/
/*	n_vegades, esperant un temps aleatori entre dues		*/
/*	visualitzacions; el proces acaba quan s'han escrit un total de  */
/*      n_lletres (variable global) entre tots els processos.		*/
/*									*/
/*	Aquest programa s'executara com un proces fill del programa	*/
/*	'multiproc2' (veure fitxer 'multiproc2.c').			*/
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"

#define MAX_VEGADES	50


/* escriure una marca corresponent al una fila d'una finestra de CURSES  */
/* en intervals aleatoris entre 0.1 i 0.3 segons, les vegades que indiqui el */
/* tercer argument */
int main(int n_args, char *ll_args[])
{
  int i,fil,n_v, id_win,n_fil,n_col;
  int *p_lletres, id_lletres;
  void *p_win;

  if (n_args < 7)
  {   fprintf(stderr,"\tproces (%d): mp_car2 fila n_veg id_lletres id_win n_fil n_col\n",(int) getpid());
	exit(0);
  }
  fil = atoi(ll_args[1]);
  n_v = atoi(ll_args[2]);
  if (n_v > MAX_VEGADES) n_v = MAX_VEGADES;

  id_lletres = atoi(ll_args[3]);
  p_lletres = map_mem(id_lletres);	/* obtenir adres. de mem. compartida */
  if (p_lletres == (int*) -1)
  {   fprintf(stderr,"proces (%d): error en identificador de memoria\n",(int)getpid());
	exit(0);
  }

  id_win = atoi(ll_args[4]);
  p_win = map_mem(id_win);
  if (p_win == (int*) -1)
  {   fprintf(stderr,"proces (%d): error en identificador de finestra\n",(int)getpid());
	exit(0);
  }
  n_fil = atoi(ll_args[5]);		/* obtenir dimensions del camp de joc */
  n_col = atoi(ll_args[6]);

  win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */

  srand(getpid());
  for (i=0; i < n_v; i++)	/* per a totes les vegades (n_v) */
  {
    win_retard((1 + rand() % 3)*100);	/* dormir entre 0.1 i 0.3 segons */
    if (*p_lletres > 0)			/* si falten lletres */
    {
	win_escricar(fil,i+1,'a'+fil-1,NO_INV);	/* escriure marca del proces */
 	(*p_lletres)--;				/* una lletra menys */
     }
     else exit(i);	/* provoca sortida del proces */
  }
  return(i);	/* retorna el numero de lletres que ha impres el proces */
}
