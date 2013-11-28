#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "drv_glcd.h"
#include "checkmark32.h"

char fb[32];

FILE *fimg;
unsigned char *buffer;
unsigned int w, h;

int main( int argc, char *argv[])
{
	
	strcpy(fb, "/dev/fb1");
	
	if ( FBLCD_Init( &fb[0])) {
		printf("FBLCD Init Error!\n");
		return(0);
	} else {
		printf("FBLCD Init Success!\n");
	}

	if (argc==4) {
		/* RGB image test */
		fimg = fopen(argv[1],"rb");
		/* */
		if ( fimg) {
			/* */
			w=atoi(argv[2]);
			h=atoi(argv[3]);
			printf("Show RGB (%dx%d) image: %s\n", w, h, argv[1]);
			buffer = ( unsigned char *)malloc(w*h*3);
			if ( buffer != (unsigned char*)NULL) {
				fread( buffer, w*h*3, 1, fimg);
				fclose( fimg);
				FBLCD_DrawImg( 0, 0, w, h, buffer);
				sleep(3);
				/* */
				free(buffer);
			} else
				printf("Error allocating memory");
		} else
			printf("Error opening file\n");
	}

	/* */
	FBLCD_DrawGrafBackgndColor( 0xFFFFFF);
	FBLCD_DrawIcone( 319-32, 239-32, 32, 32, CheckMark32);
	printf("Icone 32x32\n");
	sleep(3);
	
	FBLCD_DrawGrafBackgndColor( 0xFFFFFF);
	printf("Bianco\n");
	sleep( 1);
	FBLCD_DrawGrafBackgndColor( 0x0000FF);
	printf("Blue\n");
	sleep( 1);
	FBLCD_DrawGrafBackgndColor( 0x00FF00);
	printf("Green\n");
	sleep( 1);
	FBLCD_DrawGrafBackgndColor( 0xFF0000);
	printf("Red\n");
	sleep( 1);

	FBLCD_SetLine( 10,10,50,50, 0xFFFFFF);
	FBLCD_SetCircle( 100,100,50,0xFFFFFF);
	
	FBLCD_SetFont( 1, BLUE, WHITE);
	FBLCD_DrawStr( "Ciao\0", 50, 50, BLUE);
	sleep( 1);

	FBLCD_SetFont( 0, BLUE, WHITE);
	FBLCD_DrawStr( "Ciao\0", 150, 150, BLUE);
	sleep( 1);
	
	FBLCD_deInit();
	printf("Done!\n");

	return(0);
}

