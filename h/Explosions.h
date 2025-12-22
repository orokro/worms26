#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H
   
/* ======================================================================================================================================
   EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++
   ====================================================================================================================================== */

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
 * @param size the max radious the explosion should achieve
 * @param power the max health damagine, and velocity causing power of the explosion
 * @param hasFire a char boolean if the explosion should generate fire particles or not
*/
extern void Explosion_spawn(short, short, char, char, char);

/**
 * Updates all the explisions currently active, should be called once per frame.
*/
extern void Explosion_update();

/**
 * Draws all explosions 
 */
extern void Explosion_drawAll();

#endif