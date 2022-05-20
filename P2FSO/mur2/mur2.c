/*****************************************************************************/
/*                                                                           */
/*                           mur0.c                                          */
/*                                                                           */
/*  Programa inicial d'exemple per a les practiques 2 de FSO.                */
/*                                                                           */
/*  Compilar i executar:                                                     */
/*     El programa invoca les funcions definides a "winsuport.c", les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':                                   */
/*                                                                           */
/*       $ gcc -Wall -c winsuport.c -o winsuport.o                           */
/*       $ gcc -Wall mur0.c winsuport.o -o mur0 -lcurses                     */
/*                                                                           */
/*  Al tenir una orientació vertical cal tenir un terminal amb prou files.   */
/*  Per exemple:                                                             */
/*               xterm -geometry 80x50                                       */
/*               gnome-terminal --geometry 80x50                             */
/*                                                                           */
/*****************************************************************************/

//#include <stdint.h>		/* intptr_t for 64bits machines */

#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "winsuport.h"		/* incloure definicions de funcions propies */

/* definicio de constants */
#define MAX_THREADS	10
#define MAXBALLS	(MAX_THREADS-1)
#define MIN_FIL	10		/* dimensions del camp. Depenen del terminal ! */
#define MAX_FIL	50
#define MIN_COL	10
#define MAX_COL	80
#define BLKSIZE	3
#define BLKGAP	2
#define BLKCHAR 'B'
#define WLLCHAR '#'
#define FRNTCHAR 'A'
#define LONGMISS	65
			/* variables globals */
char *descripcio[] = {
	"\n",
	"Aquest programa implementa una versio basica del joc Arkanoid o Breakout:\n",
	"generar un camp de joc rectangular amb una porteria, una paleta que s\'ha\n",
	"de moure amb el teclat per a cobrir la porteria, i una pilota que rebota\n",
	"contra les parets del camp, a la paleta i els blocs. El programa acaba si\n",
	"la pilota surt per la porteria o no queden mes blocs. Tambe es pot acabar\n",
	"amb la tecla RETURN.\n",
	"\n",
	"  Arguments del programa:\n",
	"\n",
	"       $ ./mur0 fitxer_config [retard]\n",
	"\n",
	"     El primer argument ha de ser el nom d\'un fitxer de text amb la\n",
	"     configuracio de la partida, on la primera fila inclou informacio\n",
	"     del camp de joc (valors enters), i la segona fila indica posicio\n",
	"     i velocitat de la pilota (valors reals):\n",
	"          num_files  num_columnes  mida_porteria\n",
    "          pos_fil_pal pos_col_pal mida_pal\n"
	"          pos_fila   pos_columna   vel_fila  vel_columna\n",
	"\n",
	"     on els valors minims i maxims admesos son els seguents:\n",
	"          MIN_FIL <= num_files     <= MAX_FIL\n",
	"          MIN_COL <= num_columnes  <= MAX_COL\n",
	"          0 < mida_porteria < num_files-2\n",
	"        1.0 <= pos_fila     <= num_files-3\n",
	"        1.0 <= pos_columna  <= num_columnes\n",
	"       -1.0 <= vel_fila     <= 1.0\n",
	"       -1.0 <= vel_columna  <= 1.0\n",
	"\n",
	"     Alternativament, es pot donar el valor 0 a num_files i num_columnes\n",
	"     per especificar que es vol que el tauler ocupi tota la pantalla. Si\n",
	"     tambe fixem mida_porteria a 0, el programa ajustara la mida d\'aquesta\n",
	"     a 3/4 de l\'altura del camp de joc.\n",
	"\n",
	"     A mes, es pot afegir un segon argument opcional per indicar el\n",
	"     retard de moviment del joc en mil.lisegons; el valor minim es 10,\n",
	"     el valor maxim es 1000, i el valor per defecte d'aquest parametre\n",
	"     es 100 (1 decima de segon).\n",
	"\n",
	"  Codis de retorn:\n",
	"     El programa retorna algun dels seguents codis:\n",
	"	0  ==>  funcionament normal\n",
	"	1  ==>  numero d'arguments incorrecte\n",
	"	2  ==>  no s\'ha pogut obrir el fitxer de configuracio\n",
	"	3  ==>  algun parametre del fitxer de configuracio es erroni\n",
	"	4  ==>  no s\'ha pogut crear el camp de joc (no pot iniciar CURSES)\n",
	"\n",
	"   Per a que pugui funcionar aquest programa cal tenir instal.lada la\n",
	"   llibreria de CURSES (qualsevol versio).\n",
	"\n",
	"*"
};				/* final de la descripcio */

int n_fil, n_col;		/* numero de files i columnes del taulell */
int m_por;			/* mida de la porteria (en caracters) */
int f_pal, c_pal;		/* posicio del primer caracter de la paleta */
int m_pal;				/* mida de la paleta */
int f_pil[MAXBALLS], c_pil[MAXBALLS];		/* posicio de la pilota, en valor enter */
float pos_f[MAXBALLS], pos_c[MAXBALLS];		/* posicio de la pilota, en valor real */
float vel_f[MAXBALLS], vel_c[MAXBALLS];		/* velocitat de la pilota, en valor real */
int numPilotes = 0;
int novaPil[MAXBALLS];//Centinelles bloc B
int nblocs = 0;
int dirPaleta = 0;
int retard;			/* valor del retard de moviment, en mil.lisegons */
/**Var locals -> globals**/
int fiPala = 0;
int fiPilota = 0;
pthread_t tid[MAX_THREADS];/* taula d'identificadors dels threads */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char strin[LONGMISS];			/* variable per a generar missatges de text */

/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE * fit)
{
	int ret = 0;

	fscanf(fit, "%d %d %d\n", &n_fil, &n_col, &m_por);	/* camp de joc */
	fscanf(fit, "%d %d %d\n", &f_pal, &c_pal, &m_pal);	/* paleta */
	//fscanf(fit, "%f %f %f %f\n", &pos_f[0], &pos_c[0], &vel_f[0], &vel_c[0]);	/* pilota */
	while (!feof(fit)&& numPilotes<MAXBALLS) { 
		fscanf(fit,"%f %f %f %f\n",&pos_f[numPilotes], &pos_c[numPilotes], &vel_f[numPilotes], &vel_c[numPilotes]);
		numPilotes++;
  	}
	if ((n_fil != 0) || (n_col != 0)) {	/* si no dimensions maximes */
		if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) || (n_col < MIN_COL) || (n_col > MAX_COL))
			ret = 1;
		else if (m_por > n_col - 3)
			ret = 2;
        else if ((m_pal > n_col - 3) || (m_pal < 1) || (f_pal > n_fil-1) || (f_pal < 1) || (c_pal + m_pal > n_col -1 || c_pal < 1 ))
            ret = 3;
		for(int i = 0; i < numPilotes; i++){
			if ((pos_f[i] < 1) || (pos_f[i] >= n_fil - 3) || (pos_c[i] < 1)
			 || (pos_c[i] > n_col - 1))	// tres files especials: línia d'estat, porteria i paleta
			ret = 4;
		}
		/*else if ((pos_f[0] < 1) || (pos_f[0] >= n_fil - 3) || (pos_c[0] < 1)
			 || (pos_c[0] > n_col - 1))	// tres files especials: línia d'estat, porteria i paleta
			ret = 4;*/
	}
	for(int i = 0; i < numPilotes; i++){
		if ((vel_f[i] < -1.0) || (vel_f[i] > 1.0) || (vel_c[i] < -1.0) || (vel_c[i] > 1.0))
		ret = 5;
	}
	/*if ((vel_f[0] < -1.0) || (vel_f[0] > 1.0) || (vel_c[0] < -1.0) || (vel_c[0] > 1.0))
		ret = 5;*/

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
            fprintf(stderr,"\tmida de la paleta incorrecta:\n");
            fprintf(stderr, "\tf_pal= %d \tc_pal= %d \t m_pal=%d\n", f_pal,c_pal,m_pal);
            break;
		case 4:
			fprintf(stderr, "\tposicio de la pilota incorrecta:\n");
			fprintf(stderr, "\tpos_f= %.2f \tpos_c= %.2f\n", pos_f[0],
				pos_c[0]);
			break;
		case 5:
			fprintf(stderr,
				"\tvelocitat de la pilota incorrecta:\n");
			fprintf(stderr, "\tvel_f= %.2f \tvel_c= %.2f\n", vel_f[0],
				vel_c[0]);
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
	int c, nb, offset;

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

	for (i = 0; i < m_pal; i++)	/* dibuixar paleta inicial */
		win_escricar(f_pal, c_pal + i, '0', INVERS);

	/* generar la pilota */
	if (pos_f[0] > n_fil - 1)
		pos_f[0] = n_fil - 1;	/* limita posicio inicial de la pilota */
	if (pos_c[0] > n_col - 1)
		pos_c[0] = n_col - 1;
	f_pil[0] = pos_f[0];
	c_pil[0] = pos_c[0];		/* dibuixar la pilota inicialment */
	win_escricar(f_pil[0], c_pil[0], '1', INVERS);

	// generar els blocs 
	nb = 0;
	nblocs = n_col / (BLKSIZE + BLKGAP) - 1;
	offset = (n_col - nblocs * (BLKSIZE + BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nblocs; i++) {
		for (c = 0; c < BLKSIZE; c++) {
			win_escricar(3, offset + c, FRNTCHAR, INVERS);
			nb++;
			win_escricar(4, offset + c, BLKCHAR, NO_INV);
			nb++;
			win_escricar(5, offset + c, FRNTCHAR, INVERS);
			nb++;
		}
		offset += BLKSIZE + BLKGAP;
	}
	nblocs = nb / BLKSIZE;
	// generar les defenses
	nb = n_col / (BLKSIZE + 2 * BLKGAP) - 2;
	offset = (n_col - nb * (BLKSIZE + 2 * BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nb; i++) {
		for (c = 0; c < BLKSIZE + BLKGAP; c++) {
			win_escricar(6, offset + c, WLLCHAR, NO_INV);
		}
		offset += BLKSIZE + 2 * BLKGAP;
	}

	sprintf(strin,
		"Tecles: \'%c\'-> Esquerra, \'%c\'-> Dreta, RETURN-> sortir\n",
		TEC_ESQUER, TEC_DRETA);
	win_escristr(strin);
	return (0);
}

/* funcio que escriu un missatge a la línia d'estat i tanca les curses */
void mostra_final(char *miss)
{	int lmarge;
	char marge[LONGMISS];

	/* centrar el misssatge */
	lmarge=(n_col+strlen(miss))/2;
	sprintf(marge,"%%%ds",lmarge);

	sprintf(strin, marge,miss);
	win_escristr(strin);

	/* espera tecla per a que es pugui veure el missatge */
	getchar();
}

/* Si hi ha una col.lisió pilota-bloci esborra el bloc */
void comprovar_bloc(int f, int c, int index)
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
        if (quin == BLKCHAR) novaPil[index]=1;

		nblocs--;
	}
}

/* funcio per a calcular rudimentariament els efectes amb la pala */
/* no te en compta si el moviment de la paleta no és recent */
/* cal tenir en compta que després es calcula el rebot */
void control_impacte(int ind) {
	if (dirPaleta == TEC_DRETA) {
		if (vel_c[ind] <= 0.0)	/* pilota cap a l'esquerra */
			vel_c[ind] = -vel_c[ind] - 0.2;	/* xoc: canvi de sentit i reduir velocitat */
		else {	/* a favor: incrementar velocitat */
			if (vel_c[ind] <= 0.8)
				vel_c[ind] += 0.2;
		}
	} else {
		if (dirPaleta == TEC_ESQUER) {
			if (vel_c[ind] >= 0.0)	/* pilota cap a la dreta */
				vel_c[ind] = -vel_c[ind] + 0.2;	/* xoc: canvi de sentit i reduir la velocitat */
			else {	/* a favor: incrementar velocitat */
				if (vel_c[ind] >= -0.8)
					vel_c[ind] -= 0.2;
			}
		} else {	/* XXX trucs no documentats */
			if (dirPaleta == TEC_AMUNT)
				vel_c[ind] = 0.0;	/* vertical */
			else {
				if (dirPaleta == TEC_AVALL)
					if (vel_f[ind] <= 1.0)
						vel_f[ind] -= 0.2;	/* frenar */
			}
		}
	}
	dirPaleta=0;	/* reset perque ja hem aplicat l'efecte */
}

float control_impacte2(int c_pil, float velc0) {
	int distApal;
	float vel_c;

	distApal = c_pil - c_pal;
	if (distApal >= 2*m_pal/3)	/* costat dreta */
		vel_c = 0.5;
	else if (distApal <= m_pal/3)	/* costat esquerra */
		vel_c = -0.5;
	else if (distApal == m_pal/2)	/* al centre */
		vel_c = 0.0;
	else /*: rebot normal */
		vel_c = velc0;
	return vel_c;
}

/* funcio per moure la pilota: retorna un 1 si la pilota surt per la porteria,*/
/* altrament retorna un 0 */
void * mou_pilota(void *index)
{
	int ind = (intptr_t) index;
	int f_h, c_h;
	char rh, rv, rd;
	int fora=0;
	do{
		f_h = pos_f[ind] + vel_f[ind];	/* posicio hipotetica de la pilota (entera) */
		c_h = pos_c[ind] + vel_c[ind];
		rh = rv = rd = ' ';
		pthread_mutex_lock(&mutex);
		if ((f_h != f_pil[ind]) || (c_h != c_pil[ind])) {
		/* si posicio hipotetica no coincideix amb la posicio actual */
			if (f_h != f_pil[ind]) {	/* provar rebot vertical */
				rv = win_quincar(f_h, c_pil[ind]);	/* veure si hi ha algun obstacle */
				if (rv != ' ') {	/* si hi ha alguna cosa */
					comprovar_bloc(f_h, c_pil[ind], ind);
					if (rv == '0')	/* col.lisió amb la paleta? */
						//control_impacte();
						vel_c[ind] = control_impacte2(c_pil[ind], vel_c[ind]);
					vel_f[ind] = -vel_f[ind];	/* canvia sentit velocitat vertical */
					f_h = pos_f[ind] + vel_f[ind];	/* actualitza posicio hipotetica */
				}
			}
			if (c_h != c_pil[ind]) {	/* provar rebot horitzontal */
				rh = win_quincar(f_pil[ind], c_h);	/* veure si hi ha algun obstacle */
				if (rh != ' ') {	/* si hi ha algun obstacle */
					comprovar_bloc(f_pil[ind], c_h, ind);
					/* TODO?: tractar la col.lisio lateral amb la paleta */
					vel_c[ind] = -vel_c[ind];	/* canvia sentit vel. horitzontal */
					c_h = pos_c[ind] + vel_c[ind];	/* actualitza posicio hipotetica */
				}
			}
			if ((f_h != f_pil[ind]) && (c_h != c_pil[ind])) {	/* provar rebot diagonal */
				rd = win_quincar(f_h, c_h);
				if (rd != ' ') {	/* si hi ha obstacle */
					comprovar_bloc(f_h, c_h, ind);
					vel_f[ind] = -vel_f[ind];
					vel_c[ind] = -vel_c[ind];	/* canvia sentit velocitats */
					f_h = pos_f[ind] + vel_f[ind];
					c_h = pos_c[ind] + vel_c[ind];	/* actualitza posicio entera */
				}
			}
			/* mostrar la pilota a la nova posició */
			if (win_quincar(f_h, c_h) == ' ') {	/* verificar posicio definitiva *//* si no hi ha obstacle */
				win_escricar(f_pil[ind], c_pil[ind], ' ', NO_INV);	/* esborra pilota */
				pos_f[ind] += vel_f[ind];
				pos_c[ind] += vel_c[ind];
				f_pil[ind] = f_h;
				c_pil[ind] = c_h;	/* actualitza posicio actual */
				if (f_pil[ind] != n_fil - 1)	/* si no surt del taulell, */
					win_escricar(f_pil[ind], c_pil[ind], '1', INVERS);	/* imprimeix pilota */
				else
					fora = 1;
			}
		} else {	/* posicio hipotetica = a la real: moure */
			pos_f[ind] += vel_f[ind];
			pos_c[ind] += vel_c[ind];
		}
		fiPilota = (nblocs==0 || fora);
		pthread_mutex_unlock(&mutex);
		win_retard(retard);
	}while(!fiPilota);

	return((void *) 1);
	//return (nblocs==0 || fora);
}

/* funcio per moure la paleta segons la tecla premuda */
/* retorna un boolea indicant si l'usuari vol acabar */
void * mou_paleta(void * nul)
{
	int tecla;
	do{
		tecla = win_gettec();
		pthread_mutex_lock(&mutex);
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
		pthread_mutex_unlock(&mutex);
		win_retard(retard);
	}while(!fiPala);

	return((void *) 1);
	//return (result);//passar result a var global i crear bucle
}

/* programa principal */
int main(int n_args, char *ll_args[])
{
	int i;
	FILE *fit_conf;
	int sec = 0, min = 0;
	char str_cont_temps[15];

	if ((n_args != 2) && (n_args != 3)) {	/* si numero d'arguments incorrecte */
		i = 0;
		do
			fprintf(stderr, "%s", descripcio[i++]);	/* imprimeix descripcio */
		while (descripcio[i][0] != '*');	/* mentre no arribi al final */
		exit(1);
	}

	fit_conf = fopen(ll_args[1], "rt");	/* intenta obrir el fitxer */
	if (!fit_conf) {
		fprintf(stderr, "Error: no s'ha pogut obrir el fitxer \'%s\'\n",
			ll_args[1]);
		exit(2);
	}

	if (carrega_configuracio(fit_conf) != 0)	/* llegir dades del fitxer  */
		exit(3);	/* aborta si hi ha algun problema en el fitxer */

	if (n_args == 3) {	/* si s'ha especificat parametre de retard */
		retard = atoi(ll_args[2]);	/* convertir-lo a enter */
		if (retard < 10)
			retard = 10;	/* verificar limits */
		if (retard > 1000)
			retard = 1000;
	} else
		retard = 100;	/* altrament, fixar retard per defecte */

	printf("Joc del Mur: prem RETURN per continuar:\n");
	getchar();

	if (inicialitza_joc() != 0)	/* intenta crear el taulell de joc */
		exit(4);	/* aborta si hi ha algun problema amb taulell */

	/*int fi1, fi2;
	do {
//bucle principal del joc
		fi1 = mou_paleta();
		fi2=0;
		fi2 = mou_pilota();
		win_retard(retard);	// retard del joc
	} while (!fi1 && !fi2);*/
	// variables globals i encastar bucle dins les funcions -> mou paleta i mou pilota passar a threads
	int n=0, t=0, t_total=0;
	//tercer arg envia el num de thread 
	for(int i=0; i<MAXBALLS; i++) novaPil[i] = 0;
	if (pthread_create(&tid[9], NULL, mou_paleta, NULL));
	if (pthread_create(&tid[0], NULL, mou_pilota, (void*)(intptr_t) 0));
	//printf("he creat %d threads, espero que acabin!\n\n",n);
	
	//Loop principal
	do{
		t = 0;
		for(int i=0; i<MAXBALLS; i++) t = t || novaPil[i];
		win_retard(1000);
		if(t){
			novaPil[n]=0;
			n++;
			if (n<numPilotes){
				/* generar la pilota */
				if (pos_f[n] > n_fil - 1)
					pos_f[n] = n_fil - 1;	/* limita posicio inicial de la pilota */
				if (pos_c[n] > n_col - 1)
					pos_c[n] = n_col - 1;
				f_pil[n] = pos_f[n];
				c_pil[n] = pos_c[n];		/* dibuixar la pilota inicialment */
				win_escricar(f_pil[n], c_pil[n], '1', INVERS);
				pthread_create(&tid[n], NULL, mou_pilota, (void*)(intptr_t) n);//***//
			}
		}

		sec++;
		if(sec == 60) min++;
		sec = sec % 60;
		sprintf(str_cont_temps, "Time: %02d:%02d", min, sec);
		win_escristr(str_cont_temps);
	}while (!fiPilota && !fiPala);

	for(int i = 0; i<numPilotes; i++){
		pthread_join(tid[i], (void **)&t);
		t_total+=t;
	}
	pthread_join(tid[9], (void **)&t);
	t_total+=t;
	printf("Entre tots els threads han escrit %d lletres.\n",t_total);

	if (nblocs == 0)
		mostra_final("YOU WIN !");
	else 
		mostra_final("GAME OVER");

	win_fi();		/* tanca les curses */

	return (0);		/* retorna sense errors d'execucio */
}
