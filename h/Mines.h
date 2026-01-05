/*
	Mines.h
	-------
	
	Main header file for the Mines.c module
*/
#ifndef MINES_H
#define MINES_H


// includes
#include "PhysCol.h"


// mine defines
#define mineTriggerDistance 10
#define MAX_MINES 10


// mine globals
extern short *Mine_x;
extern short *Mine_y;
extern char *Mine_xVelo;
extern char *Mine_yVelo;
extern PhysObj *Mine_physObj;
extern char *Mine_fuse;
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
 * triggers a mine
 * 
 * @param index - the mine to trigger
 */
void Mines_trigger(short index);


/**
 * @brief Draws all the mines
 * 
 */
extern void Mines_drawAll();


/**
 * @brief Spawns a mine at a specific location
 * 
 * @param x x position
 * @param y y position
 */
extern void Mines_spawnAt(short x, short y);

#endif
