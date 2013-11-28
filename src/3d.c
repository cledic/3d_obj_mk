/*
  Modulo tratto dal Moudla 2 FTL per la costruzione di un oggetto
  in 3 dimensioni che ruota

  19 Feb 91 Modificato per interpretare tutti e tre i punti messi a
			disposizione da Stereo CAD
  28 Feb 91 Inserita gestione lista linkata semplice dei triangoli.
  15 Mar 91 Modificata la lista in linkata doppia. Inserito il sort.
  17 Mar 91 Ok e' funzionante.
  19 Mar 91 Implementata la visualizzazione prospettica.
  04 Dic 91 Adattato al Borland C++. Versione di test per lo sviluppo.
  23 Feb 92 Ho implementato l'uso delle matrici per la rotazione sui
			tre assi.
  29 Feb 92 Il programma gira bene meno il sort delle superfici. Comunque
			faccio troppo uso delle variabili globali.
  29 Mar 92 Ho inserito anche la matrice di traslazione e quella di
			trasformazione in scala.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* */
#include "3d.h"
#include "drv_glcd.h"

extern COORD *pos[];
extern TRIANGLE *MARKER;
extern TRIANGLE *sequence();
extern int sort();
extern  int *px2d[], *py2d[];
extern  int shape;
extern  int vertices;
extern int sort(void);
extern TRIANGLE *sequence( TRIANGLE *punt);

typedef float Transform3D[4][4];
Transform3D r1, r2, xform;
float  distance=20.0;
float  zc=50.0, xc=50.0, yc=50.0;
int premuto, xo, yo;

/*
  Dichiarazione dei prototipi.
*/
void XRotation(int dir);
void YRotation( int dir);
void ZRotation( int dir);
void XMove( int dir);
void YMove( int dir);
void ZMove( int dir);
void DrawShapeSolid( int m);
void DrawShapeWireFrame( int m);
void ParallelTransform( void);
int PerspectiveTransform( void);
void start_3d( void);
void draw_3d( int);
void draw_vertices( void);
void XYrot( int);
void Zoom( int);
void Zoom( int);
void Mv( int);
void show_pos( int a, int b, int c, int d);
void show_info( void);
void esegui_mv( int a, int b);
void esegui_tr( int a);
/* */
void XYZTrasl( int a);
void ConcatMat( void);
void PartialNonHomTransform( void);
void FormatRotateMat(char axis, int dir);
void FormatTranslMat( char axis, float val);
void IdentMat( void);
void IdentMat2( void);
void DoMove( void);
void SwapMat( void);

/*void FormatRotateMat(char, float, Transform3D);*/

/* */
void XRotation( int dir)
{
 /* Formatta una matrice per una rotazione X */
 FormatRotateMat('x', dir);
}

void YRotation( int dir)
{
 FormatRotateMat('y', dir);
}

void ZRotation( int dir)
{
 FormatRotateMat('z', dir);
}
/*
  Procedura per il movimento sull'asse X UP
*/
void XMove( int dir)
{
 FormatTranslMat('x', (float)2.0f*dir);
}
/*
  Procedura per il movimento sull'asse Y LEFT
*/
void YMove( int dir)
{
 FormatTranslMat('y', (float)2.0f*dir);
}
/*
  Procedura per il movimento sull'asse Z UP
*/
void ZMove( int dir)
{
 FormatTranslMat('z', (float)2.0f*dir);
}

/*
  Procedura per la visualizzazione solida dell'oggetto 3D.
*/
void DrawShapeSolid( int m)
{
TRIANGLE *value;
int polyLine[8];         /* Array per contenere le coordinate dei punti */

  sort();          /* Esegue il sort delle facce prima di visualizzarle */
  value = MARKER;

  while((value = sequence(value)) != ( TRIANGLE *)NULL)
	{
	polyLine[0] = polyLine[6] = *px2d[value->first];
	polyLine[1] = polyLine[7] = *py2d[value->first];
	polyLine[2] = *px2d[value->second];
	polyLine[3] = *py2d[value->second];
	polyLine[4] = *px2d[value->third];
	polyLine[5] = *py2d[value->third];
    FBLCD_SetPolyLine( 4, &polyLine[0], m);
	}
}
/*
  Procedura per la visualizzazione wire frame dell'oggetto 3D
*/
void DrawShapeWireFrame( int m)
{
	TRIANGLE *value;
	value = MARKER;
	value = sequence(value);

	while(value != ( TRIANGLE *)NULL)
	{
		/* printf("vf:%d, vs:%d, vt:%d\n", value->first, value->first, value->first); */
		FBLCD_SetLine( *px2d[value->first], *py2d[value->first], *px2d[value->second], *py2d[value->second], m);
		FBLCD_SetLine( *px2d[value->second], *py2d[value->second], *px2d[value->third], *py2d[value->third], m);
		FBLCD_SetLine( *px2d[value->third], *py2d[value->third], *px2d[value->first], *py2d[value->first], m);
		/* */
		value = sequence(value);
	}
}

/*
  Procedura per la conversione delle coordinate da 3 a 2 dimensioni
  utilizzando la proiezione parallela.
*/
void ParallelTransform( void)
{
 int i=0;

 int addingX, addingY;
 float f;

 addingX=getmaxx()/2;
 addingY=getmaxy()/2;

  while(i<vertices)
	{
	f = (float)(1000.0f / (distance - pos[i]->z));
	*px2d[i] = (int) (pos[i]->x*f) + addingX;
	*py2d[i] = (int) (pos[i]->y*f) + addingY;
	/* printf("x:%d, y:%d, idx:%d\n", *px2d[i], *py2d[i], i); */
	i++;
	}
}
/*
*/
int PerspectiveTransform( void)
{
 int i=0;
 int addingX, addingY;
 float t;

 addingX=getmaxx()/2;
 addingY=getmaxy()/2;

 while(i<vertices)
   {
   t = (float) (zc - pos[i]->z);
   *px2d[i] = (int) (( (pos[i]->x*zc)-(xc*pos[i]->z) )/t) + addingX;
   *py2d[i] = (int) (( (pos[i]->y*zc)-(yc*pos[i]->z) )/t) + addingY;
   i++;
   }
 return 0;
}
/*
*/
void start_3d( void)
{
#ifdef DEBUG
  while (kbhit() == 0)
   {
	draw_3d(0);
	XYrot(6);
	XYZTrasl(1);
	XYZTrasl(2);
	XYZTrasl(3);
	draw_3d(1);
   }
#else
  XYrot(3);
#endif
}
/*
 Con l'ausilio dei tasti Ctrl e Alt esegue i movimenti sull'oggetto
 prendendo come riferimento il movimento del mouse, tradotto in un 
 unico valore indice 'c'
*/
void esegui_mv( int c,  int k)
{
 /* Per cancellare l'oggetto dalla vecchia posizione pulisco un rettangolo di schermo. */
 /* FBLCD_SetRect( 319/5, 239/5, 319-(319/5), 239-(239/5), 1, UNPLOT); */
 draw_3d( UNPLOT);
 if (k==4)    /* */
	 XYrot(c);
 else if (k==8) /* */
		  Zoom(c);
	  else if (k==12)
			  draw_vertices();
		   else
			  Mv(c);
 draw_3d( PLOT);
}
/*
   Disegnare l'oggetto in 3D.
*/
void draw_3d( int w)
{
 show_info();
 ParallelTransform();
/* PerspectiveTransform(); */
 DrawShapeWireFrame(w);
/* DrawShapeSolid(w);*/
}
/*
  Procedura per la visualizzazione dei punti che compongono la figura
  attraverso il loro numero.
*/
void draw_vertices( void)
{
 register int i=0;
 char num[4];

 FBLCD_SetFont( cFONT_SMALL, 0xFFFFFF, 0x009bdb9c);
 
 for (i=0;i<vertices;i++)
	{
	 sprintf( num, "%d", i);
	 FBLCD_DrawStr( num, *px2d[i], *py2d[i], 0x009bdb9c);
	 //printf( "%d",i);
	}
}
/*
*/
void XYrot( int i)
{
 switch(i) {
   case  1: XRotation(1); IdentMat2(); break;                 /* Est */
   case  2: YRotation(1); IdentMat2(); break;                 /* Sud */
   case  3: XRotation(1); SwapMat(); YRotation(1); break;     /* SE */
   case  4: XRotation(-1); IdentMat2(); break;                /* Ovest */
   case  6: XRotation(-1); SwapMat(); YRotation(1); break;    /* SO */
   case  7: YRotation(-1); IdentMat2(); break;                /* Nord */
   case  8: XRotation(1); SwapMat(); YRotation(-1); break;    /* NE */
   case 11: XRotation(-1); SwapMat(); YRotation(-1); break;   /* NO */
 }
 ConcatMat();
 PartialNonHomTransform();
}
/*
*/
void Zoom( int i)
{
 switch(i) {
  case 2:
  case 3:
  case 6: distance += 10.0;break;
  case 7:
  case 8:
  case 11: distance -= 10.0;break;
 }
}
/*
  Questa procedura viene usata per la visualizzazione prospettica.
*/
/*
void Zoom(i)
int i;
{
 switch(i) {
  case 2:
  case 3:
  case 6: ZMove(1);break;
  case 7:
  case 8:
  case 11: ZMove(-1);break;
 }
}
*/
/*
  Procedura per il movimentu sugli assi.
*/
void Mv( int i)
{
 switch(i) {
   case  1: XMove(1); IdentMat2(); break;             /* Est */
   case  2: YMove(-1); IdentMat2(); break;            /* Nord */
   case  3: XMove(1); SwapMat(); YMove(-1); break;    /* NE */
   case  4: XMove(-1); IdentMat2(); break;            /* Ovest */
   case  6: XMove(-1); SwapMat(); YMove(-1); break;   /* NO */
   case  7: YMove(1); IdentMat2(); break;             /* Sud */
   case  8: XMove(1); SwapMat(); YMove(1); break;     /* SE */
   case 11: XMove(-1); SwapMat(); YMove(1); break;    /* SO */
 }
 ConcatMat();
 DoMove();
}
/*
void show_pos(xs, ys, cs, ks)
int xs, ys, cs, ks;
{
 int i;
 char mess[160];

 settextjustify(RIGHT_TEXT, TOP_TEXT);
 setcolor(EGA_BLACK);
 outtextxy(580, 1, "                                         ");

 setcolor(EGA_WHITE);
 sprintf(mess, "X:%3d  Y:%3d  Dir:%2d, Key:%2d",xs, ys, cs, ks);
 outtextxy(580, 1, mess);
}
*/
/*
*/
void show_info( void)
{
 char mess[31];
 
  sprintf( mess, "Vertici #%4d Triangoli #%4d", vertices, shape);
  FBLCD_DrawStr( mess, 0,0, 0x009bdb9c);
  //printf("Vertici #%4d Triangoli #%4d", vertices, shape);
}

/*
  Riceve x y e bottone del mouse, lo stato dei Cntrl, Alt e il tasto premuto.
*/
int analizza( int x,  int y,  int b,  int k,  int t,  int e)
{
 int dx, dy, xdir=0, ydir=0, code=0;


 if (b == 0){
	 premuto = 0;                /* Mouse button */
	 if (e!=0)
	   {
		esegui_tr(e);
		return(1);
	   }
	 return(0);
 }

 if (premuto == 0) {
	xo = x;
	yo = y;
	premuto = 1;
 }

 if (xo == x && yo == y) /* Old pos == new pos */
	 return(0);

 dx = x - xo;              /* Delta di spostamento */
 dy = yo - y;

 if (dx != 0)
	xdir = (dx>0)?1:4;
 if (dy != 0)
	ydir = (dy>0)?2:7;

 code = xdir + ydir;
 esegui_mv(code, k);
/* show_pos(x, y, code, k);*/
 xo = x;                /* Salva la nuova posizione */
 yo = y;
 return(1);
}

/******************************************************************************
 * Description
 *	Concatenate two 4-by-4 transformation matrices.
 *
 * Input
 *	l		multiplicand (left operand)
 *	r		multiplier (right operand)
 *
 * Output
 *	*m		Result matrix
 *****************************************************************************/

void ConcatMat( void)
	{
	int i;
	int j;

	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j)
			xform[i][j] = r1[i][0] * r2[0][j]
				+ r1[i][1] * r2[1][j]
				+ r1[i][2] * r2[2][j]
				+ r1[i][3] * r2[3][j];
	}



/******************************************************************************
 * Description
 *	Format a matrix that will perform a rotation transformation
 *	about the specified axis.  The rotation angle is measured
 *	counterclockwise about the specified axis when looking
 *	at the origin from the positive axis.
 *
 * Input
 *	axis		Axis ('x', 'y', 'z') about which to perform rotation
 *	r1		    Is a pointer to rotation matrix
 *
 * Output
 *****************************************************************************/

void FormatRotateMat(char axis, int dir)
	{
	float s, c;

	IdentMat();

	s = sinphi;
	c = cosphi;

	switch(axis)
		{
		case 'x':
			r1[1][1] = r1[2][2] = c;
			r1[1][2] = (float)(s*dir);
			r1[2][1] = (float)(-s*dir);
			break;
		case 'y':
			r1[0][0] = r1[2][2] = c;
			r1[2][0] = (float)(s*dir);
			r1[0][2] = (float)(-s*dir);
			break;
		case 'z':
			r1[0][0] = r1[1][1] = c;
			r1[0][1] = (float)(s*dir);
			r1[1][0] = (float)(-s*dir);
			break;
		}
	}

/******************************************************************************
 * Description
 *	Format a matrix that will perform a translation
 *	about the specified axis.
 *
 * Input
 *	axis		Axis ('x', 'y', 'z') about which to perform rotation
 *	r1		    Is a pointer to rotation matrix
 *
 * Output
 *****************************************************************************/
void FormatTranslMat( char axis, float val)
{
  IdentMat();
  switch(axis)
	{
	 case 'x':
		r1[2][0] = val;
		r1[2][1] = r1[2][2] = 0;
		break;
	 case 'y':
		r1[2][1] = val;
		r1[2][0] = r1[2][2] = 0;
		break;
	 case 'z':
		r1[2][2] = val;
		r1[2][0] = r1[2][1] = 0;
		break;
	 }
}


/******************************************************************************
 * Description
 *	Format a matrix that will perform a scale transformation
 *	about the specified axis.
 *
 * Input
 *	axis		Axis ('x', 'y', 'z') about which to perform transformation
 *	val		    percentage
 *
 * Output
 *****************************************************************************/
void FormatScaleMat(char axis, float val)
{
  IdentMat();
  switch(axis)
	{
	 case 'x':
		r1[0][0] = val;
		r1[1][1] = r1[2][2] = 1;
		break;
	 case 'y':
		r1[1][1] = val;
		r1[0][0] = r1[2][2] = 1;
		break;
	 case 'z':
		r1[2][2] = val;
		r1[0][0] = r1[1][1] = 1;
		break;
	 }
}

/******************************************************************************
 * Description
 *	Format a 4x4 identity matrix.
 *
 * Output
 *	*m		Formatted identity matrix
 *****************************************************************************/

void IdentMat( void)
{
 int i;
 int j;

 for (i = 3; i >= 0; --i)
	{
	for (j = 3; j >= 0; --j)
		r1[i][j] = 0.0;
	r1[i][i] = 1.0;
	}
}

void IdentMat2( void)
{
 int i;
 int j;

 for (i = 3; i >= 0; --i)
	{
	for (j = 3; j >= 0; --j)
		r2[i][j] = 0.0;
	r2[i][i] = 1.0;
	}
}



/******************************************************************************
 * Description
 *	Perform a partial transform on non-homogeneous points.
 *	Given an array of non-homogeneous (3-coordinate) input points,
 *	this routine multiplies them by the 3-by-3 upper left submatrix
 *	of a standard 4-by-4 transform matrix.  The resulting non-homogeneous
 *	points are returned.
 *
 * Input
 *	m		4-by-4 transform matrix
 *
 *****************************************************************************/

void PartialNonHomTransform( void)
{
 int i=0;
 float tmp_x, tmp_y, tmp_z;

 while(i<vertices)
 {
  tmp_x = pos[i]->x * xform[0][0] + pos[i]->y * xform[1][0] + pos[i]->z * xform[2][0];
  tmp_y = pos[i]->x * xform[0][1] + pos[i]->y * xform[1][1] + pos[i]->z * xform[2][1];
  tmp_z = pos[i]->x * xform[0][2] + pos[i]->y * xform[1][2] + pos[i]->z * xform[2][2];
  pos[i]->x=tmp_x;
  pos[i]->y=tmp_y;
  pos[i]->z=tmp_z;
  i++;
 }
}
/*

*/
void DoMove( void)
{
 int i=0;

 while(i<vertices)
 {
  pos[i]->x+=xform[2][0];
  pos[i]->y+=xform[2][1];
  pos[i]->z+=xform[2][2];
  i++;
 }
}

/*
  Copia il contenuto della matrice r1 nella matrice r2
*/
void SwapMat( void)
{
 int i;
 int j;

	for (i = 3; i >= 0; --i)
		{
		for (j = 3; j >= 0; --j)
			r2[i][j] = r1[i][j];
		}
}

/* Campo di prova per la trasformazione in scala */
/*
*/
void XTrasl(void )
{
 FormatScaleMat('x', (float)1.1f);
}
/*
*/
void YTrasl( void)
{
 FormatScaleMat('y', (float)1.1f);
}
/*
*/
void ZTrasl( void)
{
 FormatScaleMat('z', (float)1.1f);
}
/*
*/
void XYZTrasl( int i)
{
 switch(i) {
   case  'X': XTrasl(); IdentMat2(); break;
   case  'Y': YTrasl(); IdentMat2(); break;
   case  'Z': ZTrasl(); IdentMat2(); break;
 }
 ConcatMat();
 PartialNonHomTransform();
}
/*
*/
void esegui_tr( int c)
{
 if (c=='X' || c=='Y' || c=='Z')
  {
   draw_3d(UNPLOT);
   XYZTrasl(c);
   draw_3d(PLOT);
  }
}

