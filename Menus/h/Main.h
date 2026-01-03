/*
	Main.h
	------
	
	Main header file for the game / main.c
*/


// includes
#include <tigcclib.h>
#include "Lib\extgraph.h"
#include "MainGeneral.h"


// main defines
  
// how many frames consist of one second?
#define TIME_MULTIPLIER 15

// worm defines
#define MAX_WORMS 16
       
	       
// main globals
extern void *GblDBuffer;
extern char GameRunning;
extern void *mapBuffer;
extern void *lightPlane;
extern void *darkPlane;
