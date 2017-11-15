// Header File
// Created 11/11/2017; 11:31:16 PM

/*
	Worms
	-----
	
	In the previous version of Worms, we had two separate sets of arrays:
	one for the White team and one for the Black team.
	
	In this new implementation, we will have one set of arrays for ALL worms.
	
	This way, we can loop over all of them at once, and update all of them at once.
*/

/*
	Worm Arrays...
	
	Since C doesn't have Ojects (and even if it did they'd be too expensive on TI-89...)
	
	We will store all the worms properties in a set of arrays.
	
	The indicies will be synced, so worm_x[index] and worm_y[index] will refer to the x/y
	position of a the same worm, etc.
	
	The TOTAL number of worms in a Game at any time is 16.
	
	We will use indicies 0-7 for the White team, and 8-15 for the Black team
*/

// the X position of all the worms
short *Worm_x;

// the Y position of all the worms
short *Worm_y;

// the current X/Y velocity of the worm, if it was knocked back by an explosion, etc
char *Worm_xVelo;
char *Worm_yVelo;

// the direction the worm is currently FACING.. 0 = LEFT 1 = RIGHT
char *Worm_dir;

// the current HEALTH of the worm
char *Worm_health;

// bit mask if the worm is dead. bit 1 = dead, bit = 0 alive
long Worm_isDead;

// bit mask if the worm is ACTIVE... different that dead.
long Worm_active;

// worms can be in various states, depending on the game mode or their physical situation
// this ENUM defines the possible states of a Worm
enum WormModes {wormMode_idle, wormMode_walking, wormMode_jumping, wormMode_backFliping, wormMode_falling, wormMode_knockBack, wormMode_parachute, wormMode_rope, wormMode_bungie};

// store the current mode for every worm:
char *Worm_mode;

// every time the team / turn switches a new worm is the "active" worm.
// if worm-select is enabled, during this period the play can change the active worm.
// the game-logic will update this variable, such that worms that are alive, and on the current team, will cycle
char Worm_currentWorm;

