/*
	OilDrums.h
	----------
	
	Main header file for the OilDrums.c module
*/

#ifndef OILDRUMS_H
#define OILDRUMS_H


// includes
#include "PhysCol.h"


// OilDrum defines
#define MAX_OILDRUMS 6
#define OILDRUM_HEALTH 10

// OilDrum globals
extern short *OilDrum_x;
extern short *OilDrum_y;
extern char *OilDrum_health;
extern unsigned short OilDrum_active;
extern unsigned short OilDrum_settled;
extern char *OilDrum_xVelo;
extern char *OilDrum_yVelo;
extern PhysObj *OilDrum_physObj;


// OilDrum function prototypes

/**
 * This spawns all the Oil Drums on the map.
 *
 * This method is always called in the Map_build method, and will check if OilDrums are enabled on it's own.
 * This sets up, and makes active all the OilDrums.
*/
extern void OilDrums_spawnDrums();


/**
 * Updates the Oil Drums currently active, should be called once per frame.
*/
extern void OilDrums_update();


/**
	Draws all the in-game, on-screen Oil Drums.
*/
extern void OilDrums_drawAll();

#endif
