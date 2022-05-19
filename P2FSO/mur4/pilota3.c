#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "memoria.h"
#include "winsuport2.h"		/* incloure definicions de funcions propies */
#include "semafor.h"
#include "missatge.h"

#define BLKCHAR 'B'
#define FRNTCHAR 'A'

int c_pal, m_pal, n_col, n_fil, *nblocs, *novaPil, retard;
int id_busties, *p_busties, *p_final, index2, numPilotes;
float *pos_c, *pos_f, *vel_f, *vel_c;
int fora=0;

// --- sem ---
int id_sem_curses, id_sem_tauler;

void* readBustia(void* res) {
	
	//char mstg[20];
	float mstg = 0;
	while (fora == 0) {	// block & unblock
		receiveM(p_busties[index2], &mstg);

		if(*vel_c<mstg){
			*vel_c = mstg * 1.25;
		}else if(*vel_c>mstg){
			*vel_c = mstg;
		}

		if(*vel_c>1.0){
			*vel_c=1.0;
		}else if(*vel_c<-1.0){
			*vel_c=-1.0;
		}
	}
	return NULL;
}


float control_impacte2(int c_pil, float velc0) {
	int distApal;
	float vel_c;

	distApal = c_pil - c_pal;//c_pal
	if (distApal >= 2*(m_pal)/3)	/* costat dreta */
		vel_c = 0.5;
	else if (distApal <= m_pal/3)	/* costat esquerra */
		vel_c = -0.5;
	else if (distApal == m_pal/2)	/* al centre */
		vel_c = 0.0;
	else /*: rebot normal */
		vel_c = velc0;
	return vel_c;
}

/* Si hi ha una col.lisió pilota-bloci esborra el bloc */
void comprovar_bloc(int f, int c)
{
	int col;
	char quin = win_quincar(f, c);

	if (quin == BLKCHAR || quin == FRNTCHAR) {
		col = c;
		while (win_quincar(f, col) != ' ') {
			win_escricar(f, col, ' ', NO_INV);
			col++;
		}
		col = c - 1;
		while (win_quincar(f, col) != ' ') {
			win_escricar(f, col, ' ', NO_INV);
			col--;
		}

		/* generar nova pilota ? TODO*/
        if (quin == BLKCHAR) *novaPil=1;

		*nblocs = *nblocs-1;
	}
}

int main(int n_args, char *ll_args[]) {

    /*
	a1 -> *f_pil[]
	a2 -> *c_pil[]
	a3 -> *pos_c[]
	a4 -> *pos_f[]
	a5 -> *vel_f[]
	a6 -> *vel_c[]
	a7 -> c_pal
	a8 -> m_pal
	a9 -> novaPil[]
	a10 -> nblocs
	a11 -> n_fil
	a12 -> n_col
	a13 -> id_win
	*/

	int id_f_pil, id_c_pil, id_novaPil, id_nblocs, id_win;
	int *f_pil, *c_pil, *fiPilota;
	int id_pos_c, id_pos_f, id_vel_f, id_vel_c;
	void* p_win;

	pthread_t bustiaVel;

	// inicialitzar variables
	id_f_pil = atoi(ll_args[1]);
	f_pil = map_mem(id_f_pil);
	
	id_c_pil = atoi(ll_args[2]);
	c_pil = map_mem(id_c_pil);

	id_pos_c = atoi(ll_args[3]);
	pos_c = map_mem(id_pos_c);

	id_pos_f = atoi(ll_args[4]);
	pos_f = map_mem(id_pos_f);

	id_vel_f = atoi(ll_args[5]);
	vel_f = map_mem(id_vel_f);

	id_vel_c = atoi(ll_args[6]);
	vel_c = map_mem(id_vel_c);

	c_pal = atoi(ll_args[7]);
	m_pal = atoi(ll_args[8]);

	id_novaPil = atoi(ll_args[9]);
	novaPil = map_mem(id_novaPil);

	id_nblocs = atoi(ll_args[10]);
	nblocs = map_mem(id_nblocs);

	n_fil = atoi(ll_args[11]);
	n_col = atoi(ll_args[12]);

	id_win = atoi(ll_args[13]);
    p_win = map_mem(id_win);

	fiPilota = map_mem(atoi(ll_args[14]));

	retard = atoi(ll_args[15]);

	// --- sem ---
	id_sem_curses = atoi(ll_args[16]);
	id_sem_tauler = atoi(ll_args[17]);

	// --- bustia ---
	id_busties = atoi(ll_args[18]);
	p_busties = map_mem(id_busties);

	index2 = atoi(ll_args[19]);
	numPilotes = atoi(ll_args[20]);

	pthread_create(&bustiaVel, NULL, readBustia, NULL);


	//fprintf(stderr,"inici 2 ->%i %i %i %i %i %i %i %i %i %i %i %i %i", id_f_pil, id_c_pil, id_pos_c, id_pos_f, id_vel_f, id_vel_c, c_pal, m_pal, id_novaPil, nblocs, n_fil, n_col, id_win);
	waitS(id_sem_curses);
	win_set(p_win,n_fil,n_col);
	signalS(id_sem_curses);

	int f_h, c_h;
	char rh, rv, rd;
	do{
		f_h = *pos_f + *vel_f;	/* posicio hipotetica de la pilota (entera) */
		c_h = *pos_c + *vel_c;
		rh = rv = rd = ' ';
		//fprintf(stderr,"inici %i %i", f_h, c_h);
		if ((f_h != *f_pil) || (c_h != *c_pil)) {
		/* si posicio hipotetica no coincideix amb la posicio actual */
			waitS(id_sem_tauler);
			waitS(id_sem_curses);
			if (f_h != *f_pil) {	/* provar rebot vertical */
				rv = win_quincar(f_h, *c_pil);	/* veure si hi ha algun obstacle */
				if (rv != ' ') {	/* si hi ha alguna cosa */
					comprovar_bloc(f_h, *c_pil);
					if (rv == '0')	/* col.lisió amb la paleta? */
						//control_impacte();
						*vel_c = control_impacte2(*c_pil, *vel_c);
					*vel_f = -*vel_f;	/* canvia sentit velocitat vertical */
					f_h = *pos_f + *vel_f;	/* actualitza posicio hipotetica */
				}
			}
			signalS(id_sem_curses);
			signalS(id_sem_tauler);

			waitS(id_sem_tauler);
			waitS(id_sem_curses);
			if (c_h != *c_pil) {	/* provar rebot horitzontal */
				rh = win_quincar(*f_pil, c_h);	/* veure si hi ha algun obstacle */
				if (rh != ' ') {	/* si hi ha algun obstacle */
					comprovar_bloc(*f_pil, c_h);
					/* TODO?: tractar la col.lisio lateral amb la paleta */
					*vel_c = -*vel_c;	/* canvia sentit vel. horitzontal */
					c_h = *pos_c + *vel_c;	/* actualitza posicio hipotetica */
				}
			}
			signalS(id_sem_curses);
			signalS(id_sem_tauler);

			waitS(id_sem_tauler);
			waitS(id_sem_curses);
			if ((f_h != *f_pil) && (c_h != *c_pil)) {	/* provar rebot diagonal */
				rd = win_quincar(f_h, c_h);
				if (rd != ' ') {	/* si hi ha obstacle */
					comprovar_bloc(f_h, c_h);
					*vel_f = -*vel_f;
					*vel_c = -*vel_c;	/* canvia sentit velocitats */
					f_h = *pos_f + *vel_f;
					c_h = *pos_c + *vel_c;	/* actualitza posicio entera */
				}
			}
			signalS(id_sem_curses);
			signalS(id_sem_tauler);

			waitS(id_sem_tauler);
			waitS(id_sem_curses);
			/* mostrar la pilota a la nova posició */
			if (win_quincar(f_h, c_h) == ' ') {	/* verificar posicio definitiva *//* si no hi ha obstacle */
				win_escricar(*f_pil, *c_pil, ' ', NO_INV);	/* esborra pilota */
				*pos_f += *vel_f;
				*pos_c += *vel_c;
				*f_pil = f_h;
				*c_pil = c_h;	/* actualitza posicio actual */
				if (*f_pil != n_fil - 1){
					win_escricar(*f_pil, *c_pil, '1', INVERS);	/* imprimeix pilota */
				}	/* si no surt del taulell, */
				else
					fora = 1;
			}
			signalS(id_sem_curses);
			signalS(id_sem_tauler);
		} else {	/* posicio hipotetica = a la real: moure */
			*pos_f += *vel_f;
			*pos_c += *vel_c;
		}
		win_retard(retard);
		waitS(id_sem_curses);
		win_update();
		signalS(id_sem_curses);
	}while(!(*nblocs==0) && !fora);

	
	for (int i = 0; i < numPilotes; i++)
	{
		if(i!=index2) {		// pilota3: memoria.c:84: map_mem: Assertion `adr != (void*)-1' failed.
			sendM(p_busties[i], vel_c, sizeof(float));
		}
	}
	

	*fiPilota = *fiPilota+1;

	return(1);
}