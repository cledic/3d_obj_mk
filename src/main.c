#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include "drv_glcd.h"
#include "3d.h"
#include "drawobj.h"
#include "touchscreen.h"

/* */
unsigned int TS_registration( void);
void *TS_Read_XY( void *ptr);
unsigned int xpos, ypos;
pthread_t thread1;
int iret1;
volatile int tsrdy=0;
/* */
unsigned int v3DVal;     /* Indica il valore del movimento da applicare all'oggetto 3D */
unsigned int v3DFn;     /* Indica la funzione da svolgere: XYZRot, Zoom, Move*/
void esegui_mv( int c,  int k);
/* */
char fb[32];
/* */
#define BILLION 1E9
struct timespec start, end;
double t_ns;
char tm_elps[32];

int main( void)
{
	/* */
	strcpy(fb, "/dev/fb1");
	
	if ( FBLCD_Init( &fb[0])) {
		printf("FBLCD Init Error!\n");
		return(0);
	} else {
		printf("FBLCD Init Success!\n");
	}
	
	FBLCD_DrawGrafBackgndColor( BLACK);
	
	/* */
	if ( TS_registration()) {
		printf("ERROR: TS Init\n");
		return(1);
	}
	/* */
	printf("TS registration Success!\n");
	
	/* Inizio la visualizzazione 3D */
	if ( Init3D()) {
		printf("ERROR: Init3D\n");
		return(1);
	}
	/* */
	printf("Init 3D Success!\n");
	
	/* */
	v3DVal = cXY_NORD;
	v3DFn  = cEXEC_XYROT;
	
	//clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start);
	/* Loop di controllo touchscreen */
	while(1) {
		/* */
		if ( tsrdy) {
			tsrdy=0;
#if 0			
			/* Verifica della posizione del tap */
			if ( TS_CheckWindows( 319-32, 239-32, 319, 239, xpos, ypos)) {
				reset();      /* Riavvio del software */
			}
#endif			
			if ( TS_CheckWindows( 0, 0, 319/2, 219/2, xpos, ypos)) {
				v3DFn  = cEXEC_XYROT;
				v3DVal = cXY_NORD;
			}
			if ( TS_CheckWindows( 319/2, 0, 319, 219/2, xpos, ypos)) {
				v3DFn  = cEXEC_XYROT;
				v3DVal = cXY_SUD;
			}
			if ( TS_CheckWindows( 0, 219/2, 319/2, 219, xpos, ypos)) {
				v3DFn  = cEXEC_XYROT;
				v3DVal = cXY_EST;
			}
			if ( TS_CheckWindows( 319/2, 219/2, 319, 219, xpos, ypos)) {
				v3DFn  = cEXEC_XYROT;
				v3DVal = cXY_OVEST;
			}
		}
#if 1
		/* L'oggetto è in movimento continuo. */
		clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &end);
		t_ns = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / BILLION; // get elapsed time in us
		/* */
		if ( t_ns >= 0.050) {
			/* */
			clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start);
			esegui_mv( v3DVal, v3DFn);
			clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &end);
			t_ns = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / BILLION; // get elapsed time in us
			/* */
			sprintf( tm_elps, "%3.2lfms", t_ns);  
			FBLCD_DrawStr( tm_elps, 319-64, 0, 0x009bdb9c);
			/* */
			clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start);
		}
#else
		esegui_mv( v3DVal, v3DFn);
		sleep(1);
#endif
	} /* while(1) */

	return(0);
}

unsigned int TS_registration( void)
{
	/* */
	if ( TS_Init( 2) ) {
		return(1);
	}
	/* */
	iret1 = pthread_create( &thread1, NULL, TS_Read_XY, (void*)&tsrdy);
	/* */
	return(0);
}

void *TS_Read_XY( void *ptr)
{
	int *flag;
	
	flag=(int*)ptr;
	
	while(1)
	{
		while( TS_TouchGet( &xpos, &ypos));
		*flag=1;
		/* printf("X: %d, Y:%d\n", xpos, ypos); */
		/* sleep(1); */
	}
}