/*
	Explosions.h
	------------
	
	Main header file for the Explosions.c module
*/

#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H
   
   
// explosion defined
#define MAX_EXPLOSIONS 8


// explosion globals
extern short Explosion_x[MAX_EXPLOSIONS];
extern short Explosion_y[MAX_EXPLOSIONS];
extern char Explosion_time[MAX_EXPLOSIONS];
extern char Explosion_size[MAX_EXPLOSIONS];
extern char Explosion_power[MAX_EXPLOSIONS];
extern unsigned short Explosion_active;
extern unsigned short Explosion_firstFrame;


// explosion function prototypes

/**
 * Spawns an explosion at the area, with the max radius size, power and if it spawns fire particles or not.
 *
 * @param x the x position in world space to spawn the explosion
 * @param y the y position in world space to spawn the explosion
 * @param size the max radius the explosion should achieve
 * @param power the max health damage, and velocity causing power of the explosion
 * @param hasFire a char boolean if the explosion should generate fire particles or not
*/
extern short Explosion_spawn(short, short, char, char, char);


/**
 * Updates all the explosions currently active, should be called once per frame.
*/
extern void Explosion_update();


/**
 * Draws all explosions 
 */
extern void Explosion_drawAll();


/**
 * updates active explosions
 */
extern void Explosion_update();


/**
 * Draws active explosions, if there are any
*/
extern void Explosion_drawAll();


/**
 * returns the index of the first active explosion, or -1 if none are active
 * @return char index of first active explosion
 */
extern char Explosion_getFirstActive();


/**
 * Digs a hole in the map
 * 
 * @param x x position
 * @param y y position
 * @param s radius
 */
extern void Explosion_dig(short x, short y, short s);

#endif