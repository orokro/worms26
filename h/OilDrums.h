#ifndef OILDRUMS_H
#define OILDRUMS_H

#include "PhysCol.h"

/* ======================================================================================================================================
   OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUM
   ====================================================================================================================================== */

// OilDrum defines
#define MAX_OILDRUMS 6


// OilDrum globals
extern short OilDrum_x[MAX_OILDRUMS];
extern short OilDrum_y[MAX_OILDRUMS];
extern char OilDrum_health[MAX_OILDRUMS];
extern unsigned short OilDrum_active;
extern unsigned short OilDrum_settled;
extern char OilDrum_xVelo[MAX_OILDRUMS];
extern char OilDrum_yVelo[MAX_OILDRUMS];


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
