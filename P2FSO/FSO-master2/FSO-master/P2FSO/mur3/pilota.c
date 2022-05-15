#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "memoria.h"
#include "winsuport2.h"		/* incloure definicions de funcions propies */
#include "mur3.c"

int main(int n_args, char *ll_args[]) {

    /*
	a1 -> f_pil
	a2 -> c_pil
	a3 -> pos_c
	a4 -> pos_f
	a5 -> vel_f
	a6 -> vel_c
	*/

	// inicialitzar variables
	int *f_pil, *c_pil, *pos_c, *pos_f, *vel_f, *vel_c, *index;
	int id_f_pil, id_c_pil, id_pos_c, id_pos_f, id_vel_f, id_vel_c, id_index, n_fil;
	
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	int f_h, c_h;
	char rh, rv, rd;
	int fora=0;

	id_f_pil = atoi(ll_args[2]);
	f_pil = map_mem(id_f_pil);

	id_c_pil = atoi(ll_args[3]);
	c_pil = map_mem(id_c_pil);

	id_pos_c = atoi(ll_args[4]);
	pos_c = map_mem(id_pos_c);

	id_pos_f = atoi(ll_args[5]);
	pos_f = map_mem(id_pos_f);

	id_vel_f = atoi(ll_args[6]);
	vel_f = map_mem(id_vel_f);

	id_vel_c = atoi(ll_args[7]);
	vel_c = map_mem(id_vel_c);

	id_index = atoi(ll_args[8]);
	index = map_mem(id_index);

	n_fil = atoi(ll_args[9]);

	do{
		f_h = pos_f + *vel_f;	/* posicio hipotetica de la pilota (entera) */
		c_h = pos_c + *vel_c;
		rh = rv = rd = ' ';
		pthread_mutex_lock(&mutex);
		if ((f_h != f_pil) || (c_h != c_pil)) {
		/* si posicio hipotetica no coincideix amb la posicio actual */
			if (f_h != f_pil) {	/* provar rebot vertical */
				rv = win_quincar(f_h, c_pil);	/* veure si hi ha algun obstacle */
				if (rv != ' ') {	/* si hi ha alguna cosa */
					comprovar_bloc(f_h, c_pil, index);
					if (rv == '0')	/* col.lisió amb la paleta? */
						//control_impacte();
						vel_c = control_impacte2(c_pil, *vel_c);
					vel_f = -*vel_f;	/* canvia sentit velocitat vertical */
					f_h = pos_f + *vel_f;	/* actualitza posicio hipotetica */
				}
			}
			if (c_h != c_pil) {	/* provar rebot horitzontal */
				rh = win_quincar(f_pil, c_h);	/* veure si hi ha algun obstacle */
				if (rh != ' ') {	/* si hi ha algun obstacle */
					comprovar_bloc(f_pil, c_h, index);
					/* TODO?: tractar la col.lisio lateral amb la paleta */
					vel_c = -*vel_c;	/* canvia sentit vel. horitzontal */
					c_h = pos_c + *vel_c;	/* actualitza posicio hipotetica */
				}
			}
			if ((f_h != f_pil) && (c_h != c_pil)) {	/* provar rebot diagonal */
				rd = win_quincar(f_h, c_h);
				if (rd != ' ') {	/* si hi ha obstacle */
					comprovar_bloc(f_h, c_h, index);
					vel_f = -*vel_f;
					vel_c = -*vel_c;	/* canvia sentit velocitats */
					f_h = pos_f + *vel_f;
					c_h = pos_c + *vel_c;	/* actualitza posicio entera */
				}
			}
			/* mostrar la pilota a la nova posició */
			if (win_quincar(f_h, c_h) == ' ') {	/* verificar posicio definitiva *//* si no hi ha obstacle */
				win_escricar(f_pil, c_pil, ' ', NO_INV);	/* esborra pilota */
				pos_f += *vel_f;
				pos_c += *vel_c;
				f_pil = f_h;
				c_pil = c_h;	/* actualitza posicio actual */
				if (f_pil != n_fil - 1)	/* si no surt del taulell, */
					win_escricar(f_pil, c_pil, '1', INVERS);	/* imprimeix pilota */
				else
					fora = 1;
			}
		} else {	/* posicio hipotetica = a la real: moure */
			pos_f += *vel_f;
			pos_c += *vel_c;
		}
		//fiPilota = (nblocs==0 || fora);
		pthread_mutex_unlock(&mutex);
		//win_retard(retard);
		win_retard(100); // per defecte tindrà un valor de 100
	}while(1);

	return((void *) 1);
	//return (nblocs==0 || fora);
}