#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "tslib.h"

static struct tsdev *ts;
static char *tsdevice=NULL;
static struct ts_sample samp;
static int ts_orientation=0;

/** This function open the ts device. Set the orintation.
 *
 * @param	orientation, value from 0 to 3, see the TS_TouchGet
 * @retval  1 for error, 0 OK
 */
unsigned int TS_Init( int orientation)
{
	if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL ) {
		ts = ts_open(tsdevice,0);
	} else {
		if (!(ts = ts_open("/dev/input/event0", 0)))
			ts = ts_open("/dev/touchscreen/ucb1x00", 0);
	}

	if (!ts) {
		perror("ts_open");
		return(1);
	}

	if (ts_config(ts)) {
		perror("ts_config");
		return(1);
	}

	ts_orientation=orientation;
	
	return(0);
}

/** This function read the TS device. Wait forever for a user Tap
 *
 * @param pointer to x and y value. Will be populated with the Tap coord.
 * @retval 1 for Error, 0 OK
 */
unsigned int TS_TouchGet( unsigned int *x, unsigned int *y)
{
	int ret;
	
	while ( 1) {
		/* ts_read return 1 while reading and 0 at the end. */
		ret = ts_read(ts, &samp, 1);
		
		if (ret < 0) {
			perror("ts_read");
			return(1);
		}
		/* */
		switch( ts_orientation) {
			case 0:
				*x=samp.x;
				*y=samp.y;
				break;
			case 1:
				*x=samp.x;
				*y=238-samp.y;
				break;
			case 2:
				*x=319-samp.x;
				*y=239-samp.y;
				break;
			case 3:
				*x=319-samp.x;
				*y=samp.y;
				break;			
		}
		/* */
		if (ret == 0) {
			break;
		}
		/* printf("%ld.%06ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure); */
	}
	/* */
	return(0);
}

/** Check if the tap is inside the window specified.
 *
 * @param 		x and y left corner, xsize and ysize, the x and y user Tap
 * @retval 		1 the Tap is inside the windows, 0 NO.
 */
unsigned int TS_CheckWindows (unsigned int X_up, unsigned int Y_up,
                        unsigned int X_size, unsigned int Y_size,
                        unsigned int X_point, unsigned int Y_point )
{
  if((X_up <= X_point) && (X_point <= X_up+X_size) &&
     (Y_up <= Y_point) && (Y_point <= Y_up+Y_size))
  {
    return ( 1);
  }
  return ( 0);
}
