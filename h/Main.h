
#include <tigcclib.h>

// include all our headers in dependency order
#include "Lib\extgraph.h"
#include "MainGeneral.h"


/* ======================================================================================================================================
   MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++
   ====================================================================================================================================== */
  
// main defines
  
// how many frames consist of one second?
#define TIME_MULTIPLIER 15
	       
// main globals
extern void *GblDBuffer;
extern char GameRunning;
extern void *mapBuffer;
extern void *lightPlane;
extern void *darkPlane;
	
// main prototypes

/**
 * Calculates the distance between two 2D points.
 	 * 
 	 * @param x1 the x value of the first coordinate
 	 * @param y1 the y value of the first coordinate
 	 * @param x2 the x value of the second coordinate
 	 * @param y2 the y value of the second coordinate
 	 * @return the distance between 2 points, via Pythags thereom
*/
short dist(short, short, short, short);
