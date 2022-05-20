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

#include <stdint.h>		/* intptr_t for 64bits machines */
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "memoria.h"
#include "winsuport2.h"		/* incloure definicions de funcions propies */
#include "semafor.h"
#include "missatge.h"

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
int pos_f[MAXBALLS], pos_c[MAXBALLS];		/* posicio de la pilota, en valor real */
int vel_f[MAXBALLS], vel_c[MAXBALLS];		/* velocitat de la pilota, en valor real */
int numPilotes = 0; //controla el num total de pilotes possibles, s'inicialitza al llegir el fitxer params.txt
int novaPil[MAXBALLS];//Centinelles bloc B
int nblocs = 0;
int dirPaleta = 0;
int retard;			/* valor del retard de moviment, en mil.lisegons */
/**Var locals -> globals**/
int fiPala = 0;
int fiPilota;

int id_win;
int id_sem_curses, id_sem_cont, id_sem_tauler, id_busties, *p_busties;

pid_t processosPilota[MAXBALLS];

char strin[LONGMISS];			/* variable per a generar missatges de text */

/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE * fit)
{
	int ret = 0;
	float *p_pos_c, *p_pos_f, *p_vel_f, *p_vel_c;

	fscanf(fit, "%d %d %d\n", &n_fil, &n_col, &m_por);	/* camp de joc */
	fscanf(fit, "%d %d %d\n", &f_pal, &c_pal, &m_pal);	/* paleta */
	//fscanf(fit, "%f %f %f %f\n", &pos_f[0], &pos_c[0], &vel_f[0], &vel_c[0]);	/* pilota */
	while (!feof(fit) && numPilotes<MAXBALLS) {
		//TODO mem comp
		pos_f[numPilotes] = ini_mem(sizeof(float));
		pos_c[numPilotes] = ini_mem(sizeof(float));
		vel_f[numPilotes] = ini_mem(sizeof(float));
		vel_c[numPilotes] = ini_mem(sizeof(float));
		p_pos_f = map_mem(pos_f[numPilotes]);
		p_pos_c = map_mem(pos_c[numPilotes]);
		p_vel_f = map_mem(vel_f[numPilotes]);
		p_vel_c = map_mem(vel_c[numPilotes]);
		fscanf(fit,"%f %f %f %f\n",p_pos_f, p_pos_c, p_vel_f, p_vel_c);
		//fscanf(fit,"%f %f %f %f\n",&pos_f[numPilotes], &pos_c[numPilotes], &vel_f[numPilotes], &vel_c[numPilotes]);
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
			p_pos_f = map_mem(pos_f[i]);
			p_pos_c = map_mem(pos_c[i]);
			if ((*p_pos_f < 1) || (*p_pos_f >= n_fil - 3) || (*p_pos_c < 1)
			 || (*p_pos_c > n_col - 1))	// tres files especials: línia d'estat, porteria i paleta
			ret = 4;
		}
		/*for(int i = 0; i < numPilotes; i++){
			if ((pos_f[i] < 1) || (pos_f[i] >= n_fil - 3) || (pos_c[i] < 1)
			 || (pos_c[i] > n_col - 1))	// tres files especials: línia d'estat, porteria i paleta
			ret = 4;
		}*/
		/*else if ((pos_f[0] < 1) || (pos_f[0] >= n_fil - 3) || (pos_c[0] < 1)
			 || (pos_c[0] > n_col - 1))	// tres files especials: línia d'estat, porteria i paleta
			ret = 4;*/
	}

	id_busties = ini_mem(numPilotes*sizeof(int));	// nou espai de memòria * num pilotes totals
	p_busties = map_mem(id_busties);

	for(int i = 0; i < numPilotes; i++){
		p_vel_f = map_mem(vel_f[i]);
		p_vel_c = map_mem(vel_c[i]);
		p_busties[i] = ini_mis();
		if ((*p_vel_f < -1.0) || (*p_vel_f > 1.0) || (*p_vel_c < -1.0) || (*p_vel_c > 1.0))
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
			fprintf(stderr, "\tpos_f= %.2f \tpos_c= %.2f\n", *p_pos_f,
				*p_pos_c);
			break;
		case 5:
			fprintf(stderr,
				"\tvelocitat de la pilota incorrecta:\n");
			fprintf(stderr, "\tvel_f= %.2f \tvel_c= %.2f\n", *p_vel_f,
				*p_vel_c);
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
	void* p_win;

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

	id_win = ini_mem(retwin);
	p_win = map_mem(id_win);
	win_set(p_win, n_fil, n_col);

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

	int *p_f_pil, *p_c_pil;
	float *p_pos_c, *p_pos_f;
	p_pos_f = map_mem(pos_f[0]);
	p_pos_c = map_mem(pos_c[0]);
	if (*p_pos_f > n_fil - 1)
		*p_pos_f = n_fil - 1;
	if (*p_pos_c > n_col - 1)
		*p_pos_c = n_col - 1;
	
	for (int i = 0; i<numPilotes; i++){
		f_pil[i] = ini_mem(sizeof(int));
		c_pil[i] = ini_mem(sizeof(int));
	}
	
	p_f_pil = map_mem(f_pil[0]);
	p_c_pil = map_mem(c_pil[0]);

	*p_f_pil = *p_pos_f;
	*p_c_pil = *p_pos_c;
	win_escricar(*p_f_pil, *p_c_pil, '1', INVERS);
	/* generar la pilota //TODO mem comp
	if (pos_f[0] > n_fil - 1)
		pos_f[0] = n_fil - 1;
	if (pos_c[0] > n_col - 1)
		pos_c[0] = n_col - 1;
	f_pil[0] = pos_f[0];
	c_pil[0] = pos_c[0];		
	win_escricar(f_pil[0], c_pil[0], '1', INVERS);
	*/
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
	int *p_nblocs; //cuidao
	nblocs = ini_mem(sizeof(int));
	p_nblocs = map_mem(nblocs);
	*p_nblocs = nb / BLKSIZE;
	// generar les defenses
	nb = n_col / (BLKSIZE + 2 * BLKGAP) - 2;
	offset = (n_col - nb * (BLKSIZE + 2 * BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nb; i++) {
		for (c = 0; c < BLKSIZE + BLKGAP; c++) {
			win_escricar(6, offset + c, WLLCHAR, NO_INV);
		}
		offset += BLKSIZE + 2 * BLKGAP;
	}
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

/* funcio per moure la paleta segons la tecla premuda */
/* retorna un boolea indicant si l'usuari vol acabar */
void * mou_paleta(void * nul)
{
	int tecla;
	do{
		waitS(id_sem_curses);
		tecla = win_gettec();
		signalS(id_sem_curses);
		waitS(id_sem_tauler);
		if (tecla != 0) {
			waitS(id_sem_curses);
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
			signalS(id_sem_curses);
			if (tecla == TEC_RETURN){
				waitS(id_sem_cont);
				fiPala = 1;	/* final per pulsacio RETURN */
				signalS(id_sem_cont);
			}
				
			dirPaleta = tecla;	/* per a afectar al moviment de les pilotes */
		}
		signalS(id_sem_tauler);
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
	pthread_t pala_jugador;

	// inicialitzar semafors
	id_sem_cont = ini_sem(1);
	id_sem_curses = ini_sem(1);
	id_sem_tauler = ini_sem(1);


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

	//_____________________________________________________________________
	char a1[20], a2[20], a3[20], a4[20], a5[20], a6[20], a7[20], a8[20], a9[20], a10[20], a11[20], a12[20], a13[20], a14[20], a15[20], a16[20], a17[20], a18[20], a19[20], a20[20];
	int *p_novaPil, *p_fiPilota, fiPilota_total=0;
	/*
	int f_pil[MAXBALLS], c_pil[MAXBALLS];
	float pos_f[MAXBALLS], pos_c[MAXBALLS];
	float vel_f[MAXBALLS], vel_c[MAXBALLS];
	int novaPil[MAXBALLS];//Centinelles bloc B
	*/
	//Mapeig a mem. compartida:
	sprintf(a1,"%i", f_pil[0]);
	sprintf(a2,"%i", c_pil[0]);
	sprintf(a3,"%i", pos_c[0]);
	sprintf(a4,"%i", pos_f[0]);
	sprintf(a5,"%i", vel_f[0]);
	sprintf(a6,"%i", vel_c[0]);
	sprintf(a7,"%i", c_pal);
	sprintf(a8,"%i", m_pal);
	for(int i=0; i<numPilotes;i++){
		novaPil[i] = ini_mem(sizeof(int));
		p_novaPil = map_mem(novaPil[i]);
		*p_novaPil = 0;
	}
	sprintf(a9,"%i",novaPil[0]);

	sprintf(a10,"%i",nblocs);
	sprintf(a11,"%i",n_fil);
	sprintf(a12,"%i",n_col);
	sprintf(a13,"%i",id_win);
	fiPilota = ini_mem(sizeof(int));
	p_fiPilota = map_mem(fiPilota);
	*p_fiPilota = 0;

	sprintf(a14,"%i",fiPilota);
	sprintf(a15,"%i",retard);

	// --- sem ---
	sprintf(a16,"%i",id_sem_cont);
	sprintf(a17,"%i",id_sem_curses);

	// --- busties ---
	sprintf(a18,"%i",id_busties);

	sprintf(a19,"%i",0);
	sprintf(a20,"%i",numPilotes);

	//fprintf(stderr,"inici %i %i %i %i %i %i %i %i %i %i %i %i %i////", f_pil[0], c_pil[0], pos_c[0], pos_f[0], vel_f[0], vel_c[0], c_pal, m_pal, novaPil[0], nblocs, n_fil, n_col, id_win);
	pthread_create(&pala_jugador, NULL, mou_paleta, NULL);
	processosPilota[0] = fork();
	if(processosPilota[0] == (pid_t) 0)
    {
		execlp("./pilota3", "pilota3", a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, (char*) 0);//(char*) 0 actua com a sentinella
	}
	
	int *p_f_pil, *p_c_pil;
	float *p_pos_c, *p_pos_f;
	int indNovaPil = 0, n = 1;
	//Loop principal
	do{
		waitS(id_sem_curses);	// -------------

		//for(int i=0; i<MAXBALLS; i++) t = t || novaPil[i];
		for(int i=0; i<numPilotes;i++){
			p_novaPil = map_mem(novaPil[i]);
			indNovaPil=i;
			if(*p_novaPil) break;
		}
		if(*p_novaPil){
			p_novaPil = map_mem(novaPil[indNovaPil]);
			*p_novaPil = 0;
			if (n<numPilotes){
				p_pos_f = map_mem(pos_f[n]);
				p_pos_c = map_mem(pos_c[n]);
				p_f_pil = map_mem(f_pil[n]);
				p_c_pil = map_mem(c_pil[n]);

				if (*p_pos_f > n_fil - 1)
					*p_pos_f = n_fil - 1;
				if (*p_pos_c > n_col - 1)
					*p_pos_c = n_col - 1;
				*p_f_pil = *p_pos_f;
				*p_c_pil = *p_pos_c;

				sprintf(a1,"%i", f_pil[n]);
				sprintf(a2,"%i", c_pil[n]);
				sprintf(a3,"%i", pos_c[n]);
				sprintf(a4,"%i", pos_f[n]);
				sprintf(a5,"%i", vel_f[n]);
				sprintf(a6,"%i", vel_c[n]);
				sprintf(a9,"%i",novaPil[n]);
				sprintf(a19,"%i",n);

				processosPilota[n] = fork();
				if(processosPilota[n] == (pid_t) 0)
				{
					execlp("./pilota3", "pilota3", a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, (char*) 0);//(char*) 0 actua com a sentinella
				}
				n++;
			}
		}

		win_retard(retard);
		win_update();
		signalS(id_sem_curses);
		sec++;
		if(sec == 600) {
			min++;
			sec = 0;
		}
		sprintf(str_cont_temps, "Time: %02d:%02d", min, sec/10);
		waitS(id_sem_curses);
		win_escristr(str_cont_temps);
		signalS(id_sem_curses);
		if(*p_fiPilota==n) fiPilota_total = 1;
		
	}while (!fiPilota_total && !fiPala);
	
	pthread_join(pala_jugador, NULL);
	
	int t;
	for(int i = 0; i<numPilotes; i++){
		sendM(p_busties[i], "C", 1);
		waitpid(processosPilota[i],&t, 0);
	}

	//elim_mem
	for(int i = 0; i<numPilotes; i++){
		elim_mem(f_pil[i]);
		elim_mem(c_pil[i]);
		elim_mem(pos_f[i]);
		elim_mem(pos_c[i]);
		elim_mem(vel_f[i]);
		elim_mem(vel_c[i]);
		elim_mem(novaPil[i]);
	}

	elim_mem(id_busties);

	elim_sem(id_sem_cont);
	elim_sem(id_sem_curses);
	elim_sem(id_sem_tauler);

	elim_mem(id_win);
	elim_mem(fiPilota);
	elim_mem(nblocs);
	

	if (nblocs == 0)
		mostra_final("YOU WIN !");
	else 
		mostra_final("GAME OVER");

	win_fi();		/* tanca les curses */

	return (0);		/* retorna sense errors d'execucio */
}
