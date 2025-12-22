#ifndef MINES_H
#define MINES_H

#include "PhysCol.h"

/* ======================================================================================================================================
   MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINE
   ====================================================================================================================================== */

// mine defines
#define mineTriggerDistance 10
#define MAX_MINES 10

// mine globals
extern short Mine_x[MAX_MINES];
extern short Mine_y[MAX_MINES];
extern char Mine_xVelo[MAX_MINES];
extern char Mine_yVelo[MAX_MINES];
extern char Mine_fuse[MAX_MINES];
extern unsigned short Mine_active;
extern unsigned short Mine_triggered;
extern unsigned short Mine_settled;


// mine function prototypes

/**
 * This spawns all the Mines on the map.
 *
 * This method is always called in the Map_build method, and will check if Mines are enabled on it's own.
 * This sets up, and makes active all the Mines.
*/
extern void Mines_spawnMines();

/**
 * Updates the Mines currently active, should be called once per frame.
*/
extern void Mines_update();

/**
 * Allows a worm to trigger a mine
 *
 * @param index the mine index to trigger
*/
extern void Mines_trigger(short);

/**
 * Changes the heath of a worm. Updates worms health sprite.
 *
 * @param index the worm to change
 * @param health the amount to set it to, or add/subtract from
 * @param additive TRUE or FALSE if using additive mode
*/
extern void Worm_setHealth(short, short, char);

#endif /* MINES_H */