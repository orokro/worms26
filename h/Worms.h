#ifndef WORMS_H
#define WORMS_H

/* ======================================================================================================================================
   WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORM
   ====================================================================================================================================== */

// Animation States
#define ANIM_NONE     0
#define ANIM_JUMP     1
#define ANIM_BACKFLIP 2

// worm defines
#define MAX_WORMS 16

// Global tracking
extern char Game_wormAnimState;
extern int  Game_wormAnimTimer;
extern char Game_wormFlipStartDir; // To know if we started right or left

// worm enums
enum WormModes {wormMode_idle, wormMode_walking, wormMode_jumping, wormMode_backFliping, wormMode_falling, wormMode_knockBack, wormMode_parachute, wormMode_rope, wormMode_bungie};

// worm globals
extern short Worm_x[MAX_WORMS];
extern short Worm_y[MAX_WORMS];
extern char Worm_xVelo[MAX_WORMS];
extern char Worm_yVelo[MAX_WORMS];
extern PhysObj Worm_physObj[MAX_WORMS];
extern unsigned short Worm_dir;
extern short Worm_health[MAX_WORMS];
extern unsigned short Worm_isDead;
extern unsigned short Worm_active;
extern char Worm_mode[MAX_WORMS];
extern char Worm_currentWorm;
extern unsigned short Worm_settled;
extern unsigned short Worm_onGround;

// worm function prototypes

/**
 * At the beginning of the game, this places all the worms on the map and sets them active.
 *
 * This also gives them a random initial direction to face: left or right.
*/
extern void Worm_spawnWorms();

/**
 * This handles updates, (i.e. phsysics, gravity, and map collisions) for worms, as well as damage checking from explosions.
*/
extern void Worm_update();

#endif