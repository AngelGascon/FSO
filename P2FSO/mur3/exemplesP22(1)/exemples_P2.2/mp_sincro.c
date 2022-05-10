/************************************************************************/
/*				mp_sincro.c				*/
/*									*/
/*	Compilar i executar:						*/
/*	gcc -Wall mp_sincro.c memoria.o semafor.o missatge.o -o mp_sincro*/
/*	./mp_sincro  num_procs  n_vegades  n_lletres			*/
/*									*/
/*	Crea num_procs processos, on cada proces executara el fitxer	*/
/*	'mp_car_s' que ha d'estar accessible al mateix directori que    */
/*	el 'mp_sincro'.							*/
/*	Cada proces fill visualitzara un caracter identificatiu		*/
/*	(proc 1 -> 'a', proc 2 -> 'b', ...) tantes vegades com		*/
/*	indiqui l'argument n_vegades, esperant un temps	aleatori	*/
/*	entre dues visualitzacions.					*/
/*	El programa mp_sincro acabara la seva execucio quan s'hagin	*/
/*	escrit un total de n_lletres entre tots els processos fill.	*/
/*									*/
/*	Aquest programa esta basat amb 'multiproc.c', pero afegeix	*/
/*	mecanismes de sincronitzacio entre processos (semafor, bustia).	*/
/************************************************************************/
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "memoria.h"
#include "semafor.h"
#include "missatge.h"

#define MAX_PROCS	10
#define MAX_VEGADES	50
#define MAX_LLETRES	100

		/* Variables Globals */
pid_t tpid[MAX_PROCS];		/* taula d'identificadors dels processos fill */
 

int main(int n_args, char * ll_args[])
{
    int i,n,t,t_total,n_proc,n_veg;
    int *p_lletres,id_lletres;
    int id_sem, id_bustia;
    char a1[20],a2[20],a3[20],a4[20];
    
    if (n_args != 4)
    {   fprintf(stderr,"comanda: multiproc num_procs n_vegades n_lletres\n");
        exit(1);
    }
    n_proc = atoi(ll_args[1]);		/* convertir arguments a num. enter */
    n_veg = atoi(ll_args[2]);
    if (n_proc < 1) n_proc = 1;		/* i filtrar el seu valor */
    if (n_proc > MAX_PROCS) n_proc = MAX_PROCS;
    if (n_veg < 1) n_veg = 1;
    if (n_veg > MAX_VEGADES) n_veg = MAX_VEGADES;

    id_lletres = ini_mem(sizeof(int));         /* crear zona mem. compartida */
    p_lletres = map_mem(id_lletres);	/* obtenir adres. de mem. compartida */
    *p_lletres = atoi(ll_args[3]);	/* inicialitza variable compartida */
    if (*p_lletres < 1) *p_lletres = 1;
    if (*p_lletres > MAX_LLETRES) *p_lletres = MAX_LLETRES;
    sprintf(a2,"%i",id_lletres);    /* convertir identificador mem. en string */

    id_sem = ini_sem(1);	    /* crear semafor IPC inicialment obert */
    sprintf(a3,"%i",id_sem);	    /* convertir identificador sem. en string */

    id_bustia = ini_mis();	    /* crear bustia IPC */
    sprintf(a4,"%i",id_bustia);	    /* convertir identificador bustia en string */

    printf("PID del proces pare = %d\n",(int) getpid());
    n = 0;
    for ( i = 0; i < n_proc; i++)
    {
	tpid[n] = fork();		/* crea un nou proces */
	if (tpid[n] == (pid_t) 0)		/* branca del fill */
	{
	    sprintf(a1,"%i",(i+1));
	    execlp("./mp_car_s", "mp_car_s", a1, ll_args[2], a2, a3, a4, (char *)0);
	    fprintf(stderr,"error: no puc executar el process fill \'mp_car_s\'\n");
	    exit(0);
	}
	else if (tpid[n] > 0) n++;		/* branca del pare */
    }
    printf("He creat %d processos fill, espero que acabin!\n\n",n);

    t_total = 0;
    for (i = 0; i < n; i++)
    {
	waitpid(tpid[i],&t,0);	/* espera finalitzacio d'un fill */
	t = t >> 8;			/* normalitza resultat	*/
 	printf("el proces (%d) ha escrit %d lletres\n",(int)tpid[i],t);
 	t_total += t;
    }
    printf("\nJa han acabat tots els processos creats!\n");
    printf("Entre tots els processos han escrit %d lletres.\n",t_total);
    
    printf("\nLa sequencia de lletres enviades pels processos es la seguent:\n");
    for (i = 0; i < t_total; i++)
    {				/* per a totes les lletres enviades */
	receiveM(id_bustia,a1);		/* treu un missatge (string) de la bustia */
	printf("%c",a1[0]);		/* escriu el primer caracter */
    }
    printf("\n\n");

    elim_mis(id_bustia);	/* elimina bustia */
    elim_sem(id_sem);		/* elimina semafor */
    elim_mem(id_lletres);	/* elimina zona de memoria compartida */

    return(0);
}

