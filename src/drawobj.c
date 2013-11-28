/*
  Modulo per lo sviluppo delle figure.
*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "3d.h"
#include "drawobj.h"
/* */
#include "drv_glcd.h"
#include "touchscreen.h"

int read_points(void);

/* */
extern COORD *pos[];
extern  int *px2d[], *py2d[];
extern TRIANGLE *MARKER;
extern TRIANGLE *sequence();
extern int vertices;
extern int snap;
extern int mark;                /* Tipo mark (quadrato circolare) */
extern int mode_grid;           /* Grandezza griglia. */
extern int spinline;
extern int num_rot;				/* Numero di rotazioni per lo SpinLine */

/* Variabili globali. */
int dummy;
int step;
int left, top, right, bottom;	// Coordinate campo.
float angolo;
unsigned int ts_x, ts_y;

/*
  Questo array e' usato solo nel dominio del 2D. Penso di adoperarlo
  per il re-draw della griglia e per inizializzare i punti a 3D.
*/
int mark_pos[50][2];			// Array con le coordinate dei punti ins.
int pos_index=0;				// Indice all'array di coordinate.
/* ********************************************************************* */

/*
  Procedura per la creazione dell'obj.
  (Extrude)(SplinLine)
*/
int read_points(void)
{
  mark_pos[pos_index][0] = mark_pos[pos_index][1] = -1;

  FBLCD_DrawStr( "Draw Obj.", 319-32-72, 0, 0x009bdb9c);
   
  show_field();          /* Disegna il campo su cui posare i mark. */
  read_pos();            /* Legge la pos. del mouse ogni qual volta viene premuto il tasto */
                         /* Per uscire dal loop di acquisizione, fare tap sull'angolo in basso a DX!! */

  if (spinline)
	return(make_spinline());
  else
	return(make_extrude());

}
/*
  Crea la griglia nel rettangolo di lavoro. Il parametro passatogli
  determina il tipo di griglia.
*/
void do_grid(int i)
{
 unsigned int x, y;
 int px, py;

 switch(i)
  {
   case SMALL:
	   step =  5; break;
   case NORMAL:
	   step = 10; break;
   case BIG :
	   step = 15; break;
  }

 px = (HALF_WIDTH * 2) / step;
 py = (HALF_HIGT * 2) / step;

 x=left+px;

 for(; x<(HALF_WIDTH*2)+left; x+=px)
   for(y=top+py; y<(HALF_HIGT*2)+top; y+=py)
	FBLCD_SetPixel( x, y, PLOT);

}
/*
  Legge la pos. del mouse ogni qual volta viene premuto il tasto di
  sinistra. Ritorna al chiamante se si preme il tasto del mouse destro.
*/
void read_pos(void)
{
	//GLCD_LoadPic( 319-32, 239-32, &CheckMark32Pic, 0);
	/* Loop per leggere i tap sullo schermo */
	while( 1) {
		/* */
		while( TS_TouchGet( &ts_x, &ts_y));
		if ( TS_CheckWindows( 319-32, 239-32, 319, 239, ts_x, ts_y)) {
			break;
		}
		show_mark( ts_x, ts_y);
	}
}
/*
  Decide di visualizzare il mark seguendo la griglia o dove e'
  avvenuto il click. Controlla la var. globale snap.
*/
void show_mark(int x, int y)
{
 /* Verifica se le coordinate del tap sono comprese nell'aree ammesse */
 if (!chk_xy(x, y))
   return;
 /* Verifica se si èn modalitànap */
 if (!snap)
   plot_mark(x, y);
 else
   do_snap(x, y);
}
/*
  Date le coordinate in cui e' avvenuto il click pone il mark in
  un punto della griglia.
*/
void do_snap(int x, int y)
{
 int nx, ny, px, py, kx, ky;
 
 /* Normalizzo le coordinate. */
 nx = x-left;
 ny = y-top;

 px = (HALF_WIDTH * 2) / step;
 py = (HALF_HIGT * 2) / step;

 kx = nx / px;
 ky = ny / py;

 plot_mark((px*kx)+left, (py*ky)+top);

}
/*
  Visualizza un mark alle coordinate passategli.
  A seconda della var. mark visualizza un cerchio o un quadrato.
*/
void plot_mark(l, t)
int l, t;
{
 int tmp_x;

 store_coord(l, t);
 
 switch(mark)
  {
   case SQR:
	 FBLCD_SetRect(l-MARK_W, t-MARK_W, l+MARK_W, t+MARK_W, 0, PLOT);
	 if (spinline)
	   {
		tmp_x=l-(left+HALF_WIDTH); /* Distanza dalla riga centrale */
		tmp_x=(left+HALF_WIDTH)-tmp_x;
		FBLCD_SetRect(tmp_x-MARK_W, t-MARK_W, tmp_x+MARK_W, t+MARK_W, 0, PLOT);
	   }
	 break;
   case CIR:
	 FBLCD_SetCircle(l, t, MARK_W, PLOT);
	 if (spinline)
	   {
		tmp_x=l-(left+HALF_WIDTH); /* Distanza dalla riga centrale */
		tmp_x=(left+HALF_WIDTH)-tmp_x;
		FBLCD_SetCircle(tmp_x, t, MARK_W, PLOT);
	   }
	 break;
  }
}
/*
  Per adesso questa proc. non fa visualizzare il mark se negli estremi
  della griglia. Potrei pero' fare in modo che modifichi le variabili
  x e y in modo da farle rientrare nella griglia.????

  Ritorna 0 quando non accetta le coordinate, 1 se sono OK.
*/
int chk_xy(int x, int y)
{
 int px, py, ix, iy;

 ix = x-left;
 iy = y-top;
 px = ( HALF_WIDTH * 2) / step;
 py = ( HALF_HIGT * 2) / step;

 if ( ix <= px || iy <= py)
   return( 0);

 if ( ix >= (( HALF_WIDTH * 2) - px) || iy >= (( HALF_HIGT * 2) - py))
   return( 0);

 /* Verifico se in modalitàpinline ètato fatto tap sulla parte SX */
 if ( spinline) {
   if ( ix <= HALF_WIDTH )
     return ( 0);           /* Ritorno senza accettare il tap */
 }
 
 return(1);
}
/*
  Controllo della tastiera.
  Ritorna 0 per uscire dal programma.
  Altrimenti sempre 1.
*/
int chk_key(char i)
{
 if (i=='Q' || i==27)		// Se ESC o Q esce.
   return(0);
 else
   return(1);
}
/*
  Memorizza nell'array globale la posizione di ogni mark inserito in video.
*/
void store_coord(int x, int y)
{
 mark_pos[pos_index][0] = x;
 mark_pos[pos_index][1] = y;
 pos_index++;
 mark_pos[pos_index][0] = mark_pos[pos_index][1] = -1;
}
/*
  Disegna il campo su cui posare i mark.
*/
void show_field(void)
{
 left = getmaxx() / 2 - HALF_WIDTH;  /* Pone le dimensioni della griglia */
 top = getmaxy() / 2 - HALF_HIGT;
 right = getmaxx() / 2 + HALF_WIDTH;
 bottom = getmaxy() / 2 + HALF_HIGT;

 /* draw a rectangle */
 FBLCD_SetRect(left,top,right,bottom, 0, PLOT);
 if (spinline)
   FBLCD_SetLine(left+HALF_WIDTH, top, left+HALF_WIDTH, top+(HALF_HIGT*2), PLOT);

 do_grid(mode_grid);
}
/* **********************************************
  Questa procedura inserisce i dati in una struttura simile a quella
  che veniva letta da un file .BIP .
************************************************* */
int make_extrude(void)
{
 TRIANGLE *prt, *prtold;
 int c=0;
 int dummy;

 /* Inserisco la prima faccia */
 dummy=pos_index;
 while(dummy > 0) {
	  if ( (pos[c] = (COORD *)malloc(sizeof( COORD)))
		 != (COORD *)NULL ) {
		 pos[c]->x=(float) (mark_pos[c][0]-(getmaxx()/2))/100;
		 pos[c]->y=(float) (mark_pos[c][1]-(getmaxy()/2))/100;
		 pos[c]->z=(float) 0.0f;
	  }
	  else {
		 printf("Errore nell'allocare memoria\n");
		 return(-1);
	  }
	  c++;
	  dummy--;
 }
 dummy=0;
/* Ripeto il ciclo per generare la faccia inferiore */
 while(dummy < pos_index) {
	  if ( (pos[c] = (COORD *)malloc(sizeof( COORD)))
		 != (COORD *)NULL ) {
		 pos[c]->x=(float) (mark_pos[dummy][0]-(getmaxx()/2))/100;
		 pos[c]->y=(float) (mark_pos[dummy][1]-(getmaxy()/2))/100;
		 pos[c]->z=(float) -2.0f;
	  }
	  else {
		 printf("Errore nell'allocare memoria\n");
		 return(-1);
	  }
	  c++;
	  dummy++;
 }
 /* Inserisco la sequenza di vertici che crea le facce */
 vertices=c; /* Vertici che compongono la figura */
 c=0;
 prtold = MARKER;
 while(c < pos_index-1) {
	  if( (prt = ( TRIANGLE *)malloc (sizeof (TRIANGLE)))
		 != /*(struct coord*)*/NULL ) {
		prt->first=c;
		prt->second=c+1;
		prt->third=c+pos_index+1;
		if (insert(prtold, MARKER, prt)==-1) {
		   printf("Errore nella struttura Dati\n");
		   return(-1);
		}
		prtold = prt;
	  }
	  else {
		printf("Errore nell'allocare memoria\n");
		return(-1);
	  }

	  if( (prt = ( TRIANGLE *)malloc (sizeof (TRIANGLE)))
		 != /*(struct coord*)*/NULL ) {
		prt->first=c;
		prt->second=c+pos_index+1;
		prt->third=c+pos_index;
		if (insert(prtold, MARKER, prt)==-1) {
		   printf("Errore nella struttura Dati\n");
		   return(-1);
		}
		prtold = prt;
	  }
	  else {
		printf("Errore nell'allocare memoria\n");
		return(-1);
	  }
	  c++;
 } /* while(c < pos_index-1) */
/* ******************************************************************** */
   c = pos_index-1;
   if( (prt = ( TRIANGLE *)malloc (sizeof ( TRIANGLE)))
	 != /*(struct coord*)*/NULL ) {
	 prt->first=c;
	 prt->second=0;
	 prt->third=pos_index;
	 if (insert(prtold, MARKER, prt)==-1) {
		printf("Errore nella struttura Dati\n");
		return(-1);
	 }
	 prtold = prt;
   }
   else {
	printf("Errore nell'allocare memoria\n");
	return(-1);
   }
/* ******************************************************************* */
   if( (prt = ( TRIANGLE *)malloc (sizeof ( TRIANGLE)))
	 != /*(struct coord*)*/NULL ) {
	 prt->first=c;
	 prt->second=pos_index;
	 prt->third=c+pos_index;
	 if (insert(prtold, MARKER, prt)==-1) {
		printf("Errore nella struttura Dati\n");
		return(-1);
	 }
	 prtold = prt;
   }
   else {
	printf("Errore nell'allocare memoria\n");
	return(-1);
   }
 
 dummy=0;
 while(dummy < vertices) {
	  px2d[dummy] = (int *)malloc(sizeof(int));
	  py2d[dummy] = (int *)malloc(sizeof(int));
	  dummy++;
 }
 return(1);
}

/* **********************************************
  Questa procedura inserisce i dati in una struttura simile a quella
  che veniva letta da un file .BIP .
************************************************* */
int make_spinline(void)
{
 TRIANGLE *prt, *prtold;
 int c=0, t=0, n=0;
 int dummy;
 float x_rot, z_rot, y_rot;

 angolo = (float)0.0174f*((float)(360.0f/num_rot));
 /* Inserisco le coordinate dall'array mark_pos */
 dummy=pos_index;
 while(dummy > 0) {
	  if ( (pos[c] = ( COORD *)malloc(sizeof( COORD)))
		 != ( COORD *)NULL ) {
		 x_rot=(float) 0.0f;
		 y_rot=(float) (mark_pos[c][0]-(getmaxx()/2))/50;
		 z_rot=(float) (mark_pos[c][1]-(getmaxy()/2))/50;

		 pos[c]->x=x_rot*cos(angolo) - y_rot*sin(angolo);
		 pos[c]->z=z_rot;
		 pos[c]->y=y_rot*cos(angolo) + x_rot*sin(angolo);
	  }
	  else {
		 printf("Errore nell'allocare memoria\n");
		 return(-1);
	  }
	  c++;
	  dummy--;
 }



 while (t<num_rot-1)  /* Meno una perche' gia' fatta */
  {
   dummy=pos_index;
   while(dummy > 0)
	{
	  if ( (pos[c] = ( COORD *)malloc(sizeof( COORD)))
		 != ( COORD*)NULL ) {
		 x_rot=pos[c-pos_index]->x;
		 y_rot=pos[c-pos_index]->y;
		 z_rot=pos[c-pos_index]->z;
		 pos[c]->x=x_rot*cos(angolo) - y_rot*sin(angolo);
		 pos[c]->z=z_rot;
		 pos[c]->y=y_rot*cos(angolo) + x_rot*sin(angolo);

	  }
	  else {
		 printf("Errore nell'allocare memoria\n");
		 return(-1);
	  }
	  c++;
	  dummy--;
	}
	t++;
  }



/* Ripeto il ciclo per creare pos_index vertici con coordinate pari ai
   primi pos_index ma numero vertice maggiore */

   dummy=0/*pos_index*/;
   while(dummy < pos_index)
	{
	  if ( (pos[c] = ( COORD *)malloc(sizeof( COORD)))
		 != ( COORD*)NULL ) {
		 pos[c]->x=pos[dummy]->x;
		 pos[c]->z=pos[dummy]->z;
		 pos[c]->y=pos[dummy]->y;

	  }
	  else {
		 printf("Errore nell'allocare memoria\n");
		 return(-1);
	  }
	  c++;
	  dummy++;
	}
/* ******************************************************* */

 /* Inserisco la sequenza di vertici che crea le facce */
  vertices=c; /* Vertici che compongono la figura */
 c=0; t=0;
 prtold = MARKER;
 while(c < vertices-pos_index)
  {
	n=0;
	while(n<pos_index-1)
	 {
	  if( (prt = ( TRIANGLE *)malloc (sizeof ( TRIANGLE)))
		 != /*(struct coord*)*/NULL ) {
		prt->first=c;
		prt->second=c+1;
		prt->third=c+pos_index+1;
		if (insert(prtold, MARKER, prt)==-1) {
		   printf("Errore nella struttura Dati\n");
		   return(-1);
		}
		prtold = prt;
	  }
	  else {
		printf("Errore nell'allocare memoria\n");
		return(-1);
	  }
/* ******************************************************************* */
	  if( (prt = ( TRIANGLE *)malloc (sizeof ( TRIANGLE)))
		 != /*(struct coord*)*/NULL ) {
		prt->first=c;
		prt->second=c+pos_index+1;
		prt->third=c+pos_index;
		if (insert(prtold, MARKER, prt)==-1) {
		   printf("Errore nella struttura Dati\n");
		   return(-1);
		}
		prtold = prt;
	  }
	  else {
		printf("Errore nell'allocare memoria\n");
		return(-1);
	  }
	  n++;
	  c++;
	 } /* Fine ciclo interno */
	c++;
  } /* Fine while(c<vertices-pos_index) */

 dummy=0;
 while(dummy < vertices) {
	  px2d[dummy] = (int *)malloc(sizeof(int));
	  py2d[dummy] = (int *)malloc(sizeof(int));
	  dummy++;
 }
 return(1);
}

