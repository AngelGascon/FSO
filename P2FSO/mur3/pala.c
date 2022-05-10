#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "memoria.h"
#include "winsuport.h"		/* incloure definicions de funcions propies */

int main(int n_args, char *ll_args[]){
    
    //map_mem ll_args to variables
    int f_pal = map_mem(ll_args[0]);
	int c_pal = map_mem(ll_args[1]);
	int m_pal = map_mem(ll_args[2]);
	int dirPaleta = map_mem(ll_args[3]);
	int tecla = map_mem(ll_args[4]);
	

	do{
		tecla = win_gettec();//s'ha d'executar dess del procés pare, MAIN!!
		//pthread_mutex_lock(&mutex);
		if (tecla != 0) {
			if ((tecla == TEC_DRETA)
				&& ((c_pal + m_pal) < n_col - 1)) {
					win_escricar(f_pal, c_pal, ' ', NO_INV);	/* esborra primer bloc */
					c_pal++;	/* actualitza posicio */
					win_escricar(f_pal, c_pal + m_pal - 1, '0', INVERS);	/*esc. ultim bloc */
			}
			if ((tecla == TEC_ESQUER) && (c_pal > 1)) {
					win_escricar(f_pal, c_pal + m_pal - 1, ' ', NO_INV);	/*esborra ultim bloc */
					c_pal--;	/* actualitza posicio */
					win_escricar(f_pal, c_pal, '0', INVERS);	/* escriure primer bloc */
			}
			if (tecla == TEC_RETURN){
				fiPala = 1;	/* final per pulsacio RETURN */
				fiPilota = 1;
			}
				
			dirPaleta = tecla;	/* per a afectar al moviment de les pilotes */
		}
		//pthread_mutex_unlock(&mutex);
		win_retard(retard);
	}while(!fiPala);

	return((void *) 1);
	//return (result);//passar result a var global i crear bucle
}