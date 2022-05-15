/************************************************************************/
/*				multiproc2.c				*/
/*									*/
/*  Compilar i executar:						*/
/*     gcc -Wall multiproc2.c memoria.o winsuport2.o -o multiproc2 -lcurses  */
/*     ./multiproc2  num_procs  n_vegades  n_lletres			*/
/*									*/
/*	Executa tants processos com indica el parametre num_procs,	*/
/*	on cada proces visualitza un caracter identificatiu		*/
/*	(proc 0 -> 'a', proc 1 -> 'b', ...) tantes vegades com		*/
/*	indiqui el segon parametre n_vegades, esperant un temps		*/
/*	aleatori entre dues visualitzacions; el programa acaba quan	*/
/*	s'han escrit un total de n_lletres entre tots els fills.	*/
/************************************************************************/
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"


#define MAX_PROCS	10
#define MAX_VEGADES	50

		/* Variables Globals */
pid_t tpid[MAX_PROCS];		/* taula d'identificadors dels processos fill */


int main(int n_args, char * ll_args[])
{
  int i,n,t,t_total,n_proc,n_veg;
  int *p_lletres,id_lletres;
  int id_win,n_fil,n_col,mida_camp;
  char a1[20],a3[20],a4[20],a5[10],a6[10], fmis[80];
  void *p_win;

  if (n_args != 4)
  {	fprintf(stderr,"comanda: multiproc2 num_procs n_vegades n_lletres\n");
	exit(1);
  }
  n_proc = atoi(ll_args[1]);		/* convertir arguments a num. enter */
  n_veg = atoi(ll_args[2]);
  if (n_proc < 1) n_proc = 1;		/* i filtrar el seu valor */
  if (n_proc > MAX_PROCS) n_proc = MAX_PROCS;
  if (n_veg < 1) n_veg = 1;
  if (n_veg > MAX_VEGADES) n_veg = MAX_VEGADES;

  id_lletres = ini_mem(sizeof(int));	/* crear zona mem. compartida */
  p_lletres = map_mem(id_lletres);	/* obtenir adres. de mem. compartida */
  *p_lletres = atoi(ll_args[3]);	/* inicialitza variable compartida */
  if (*p_lletres < 1) *p_lletres = 1;
  if (*p_lletres > n_proc*n_veg) *p_lletres = n_proc*n_veg;
  sprintf(a3,"%i",id_lletres);	/* convertir identificador mem. en string */

  n_fil = n_proc+3;
  n_col = n_veg+2;
  mida_camp = win_ini(&n_fil,&n_col,'+',INVERS);
  if (mida_camp < 0)
  {			/* si no pot crear l'entorn de joc amb les curses */
    switch (mida_camp)
    {	case -1: fprintf(stderr,"camp de joc ja creat!\n"); break;
	case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n"); break;
	case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n"); break;
	case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n"); break;
    }
    elim_mem(id_lletres);	/* elimina zona de memoria compartida */
    exit(2);
  }
  id_win = ini_mem(mida_camp);	/* crear zona mem. compartida */
  p_win = map_mem(id_win);	/* obtenir adres. de mem. compartida */
  sprintf(a4,"%i",id_win);
  sprintf(a5,"%i",n_fil);	/* convertir mides camp en string */
  sprintf(a6,"%i",n_col);

  win_set(p_win,n_fil,n_col);		/* crea acces a finestra oberta */

  n = 0;
  for ( i = 0; i < n_proc; i++)
  {
    tpid[n] = fork();		/* crea un nou proces */
    if (tpid[n] == (pid_t) 0)		/* branca del fill */
    {
	sprintf(a1,"%i",(i+1));
	execlp("./mp_car2", "mp_car2", a1, ll_args[2], a3, a4, a5, a6, (char *)0);
	fprintf(stderr,"error: no puc executar el process fill \'mp_car2\'\n");
	exit(0);
    }
    else if (tpid[n] > 0) n++;		/* branca del pare */
  }
/* fprintf(stderr,"Pare: he creat %d processos fill, espero que acabin!\n\n",n);*/
      
  do
  { sprintf(fmis,"lletres = %d",*p_lletres);
    win_escristr(fmis);
    win_update();			/* actualitza visualitzacio CURSES */
    win_retard(20);
  } while (*p_lletres != 0);
      
  t_total = 0;
  for (i = 0; i < n; i++)
  {
    waitpid(tpid[i],&t,0);	/* espera finalitzacio d'un fill */
    t = t >> 8;			/* normalitza resultat	*/
/* 	fprintf(stderr,"el proces (%d) ha escrit %d lletres\n",tpid[i],t); */
    t_total += t;
  }
  win_fi();

  printf("\nJa han acabat tots els processos creats!\n");
  printf("Entre tots els processos han escrit %d lletres.\n",t_total);

  elim_mem(id_win);		/* elimina zones de memoria compartida */
  elim_mem(id_lletres);
  return(0);
}

