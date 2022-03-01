/*****************************************************************************/
/*                                                                           */
/*                           camp.c                                          */
/*                                                                           */
/*  Programa inicial d'exemple per a les practiques 2 i 3 d'ISO.             */
/*                                                                           */
/*  Compilar i executar:                                                     */
/*     El programa invoca les funcions definides a "winsuport.c", les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':                                   */
/*                                                                           */
/*       $ gcc -c winsuport.c -o winsuport.o                                 */
/*       $ gcc camp.c winsuport.o -o camp -lcurses                           */
/*                                                                           */
/*  Al tenir una orientació vertical cal tenir un terminal amb prou files.   */
/*  Per exemple:                                                             */
/*               xterm -geometry 80x50                                       */
/*               gnome-terminal --geometry 80x50                             */
/*                                                                           */
/*****************************************************************************/


#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport.h"		/* incloure definicions de funcions propies */

/* definicio de constants */
#define MIN_FIL	10		/* dimensions del camp. Depenen del terminal ! */
#define MAX_FIL	50
#define MIN_COL	10
#define MAX_COL	80
			/* variables globals */

int n_fil, n_col;		/* numero de files i columnes del taulell */
int m_por;			/* mida de la porteria (en caracters) */
int f_pal, c_pal, m_pal;		/* posicio del primer caracter de la paleta */
int f_pil, c_pil;		/* posicio de la pilota, en valor enter */
float pos_f, pos_c;		/* posicio de la pilota, en valor real */
float vel_f, vel_c;		/* velocitat de la pilota, en valor real */

/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE * fit)
{
	int ret = 0;

	fscanf(fit, "%d %d %d\n", &n_fil, &n_col, &m_por);	/* camp de joc */
	fscanf(fit, "%d %d %d\n", &f_pal, &c_pal, &m_pal);	/* plaleta */
	fscanf(fit, "%f %f %f %f\n", &pos_f, &pos_c, &vel_f, &vel_c);	/* pilota */
	if ((n_fil != 0) || (n_col != 0)) {	/* si no dimensions maximes */
		if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) || (n_col < MIN_COL) || (n_col > MAX_COL))
			ret = 1;
		else if (m_por > n_col - 3)
			ret = 2;
		else if ((pos_f < 1) || (pos_f >= n_fil - 3) || (pos_c < 1)
			 || (pos_c > n_col - 1))	/* tres files especials: línia d'estat, porteria i paleta */
			ret = 3;
	}
	if ((vel_f < -1.0) || (vel_f > 1.0) || (vel_c < -1.0) || (vel_c > 1.0))
		ret = 4;

	if (ret != 0) {		/* si ha detectat algun error */
		fprintf(stderr, "Error en fitxer de configuracio:\n");
		switch (ret) {
		case 1:
			fprintf(stderr,
				"\tdimensions del camp de joc incorrectes:\n");
			fprintf(stderr, "\tn_fil= %d \tn_col= %d\n", n_fil,
				n_col);
			break;
		case 2:
			fprintf(stderr, "\tmida de la porteria incorrecta:\n");
			fprintf(stderr, "\tm_por= %d\n", m_por);
			break;
		case 3:
			fprintf(stderr, "\tposicio de la pilota incorrecta:\n");
			fprintf(stderr, "\tpos_f= %.2f \tpos_c= %.2f\n", pos_f,
				pos_c);
			break;
		case 4:
			fprintf(stderr,
				"\tvelocitat de la pilota incorrecta:\n");
			fprintf(stderr, "\tvel_f= %.2f \tvel_c= %.2f\n", vel_f,
				vel_c);
			break;
		}
	}
	fclose(fit);
	return (ret);
}

/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
/* retorna diferent de zero si hi ha algun problema */
int inicialitza_joc(void)
{
	int i, retwin;
	int i_port, f_port;	/* inici i final de porteria */

	retwin = win_ini(&n_fil, &n_col, '+', INVERS);	/* intenta crear taulell */

	if (retwin < 0) {	/* si no pot crear l'entorn de joc amb les curses */
		fprintf(stderr, "Error en la creacio del taulell de joc:\t");
		switch (retwin) {
		case -1:
			fprintf(stderr, "camp de joc ja creat!\n");
			break;
		case -2:
			fprintf(stderr,
				"no s'ha pogut inicialitzar l'entorn de curses!\n");
			break;
		case -3:
			fprintf(stderr,
				"les mides del camp demanades son massa grans!\n");
			break;
		case -4:
			fprintf(stderr, "no s'ha pogut crear la finestra!\n");
			break;
		}
		return (retwin);
	}

	if (m_por > n_col - 2)
		m_por = n_col - 2;	/* limita valor de la porteria */
	if (m_por == 0)
		m_por = 3 * (n_col - 2) / 4;	/* valor porteria per defecte */

	i_port = n_col / 2 - m_por / 2 - 1;	/* crea el forat de la porteria */
	f_port = i_port + m_por - 1;
	for (i = i_port; i <= f_port; i++)
		win_escricar(n_fil - 2, i, ' ', NO_INV);

	n_fil = n_fil - 1;	/* descompta la fila de missatges */

	f_pal = n_fil - 2;	/* posicio inicial de la paleta per defecte */
	c_pal = (n_col-m_pal) / 2;	/* a baix i centrada */
	for (i = 0; i < m_pal; i++)	/* dibuixar paleta inicial */
		win_escricar(f_pal, c_pal + i, '0', INVERS);

	/* generar la pilota */
	if (pos_f > n_fil - 1)
		pos_f = n_fil - 1;	/* limita posicio inicial de la pilota */
	if (pos_c > n_col - 1)
		pos_c = n_col - 1;
	f_pil = pos_f;
	c_pil = pos_c;		/* dibuixar la pilota inicialment */
	win_escricar(f_pil, c_pil, '*', INVERS);

	return (0);
}

/* programa principal */
int main(int n_args, char *ll_args[])
{
	FILE *fit_conf;

	fit_conf = fopen(ll_args[1], "rt");	/* intenta obrir el fitxer */
	if (!fit_conf) {
		fprintf(stderr, "Error: no s'ha pogut obrir el fitxer \'%s\'\n",
			ll_args[1]);
		exit(2);
	}

	if (carrega_configuracio(fit_conf) != 0)	/* llegir dades del fitxer  */
		exit(3);	/* aborta si hi ha algun problema en el fitxer */

	if (inicialitza_joc() != 0)	/* intenta crear el taulell de joc */
		exit(4);	/* aborta si hi ha algun problema amb taulell */

	getchar();
	win_fi();		/* tanca les curses */

	return (0);		/* retorna sense errors d'execucio */
}
