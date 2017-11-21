// C Source File
// Created 11/11/2017; 11:34:05 PM

#include "../Headers/System/Main.h"

/*
	Worms
	-----
	
	This file defines the worms. Note that, a separate file will handle controlling
	the currently selected worm during a turn.

	In the previous version of Worms, we had two separate sets of arrays:
	one for the White team and one for the Black team.
	
	In this new implementation, we will have one set of arrays for ALL worms.
	
	This way, we can loop over all of them at once, and update all of them at once.
*/

// the X/Y position of all the worms
short Worm_x[16] = {15, 81, 120, 65, 90, 35, 150, 40, 175, 95, 250, 210, 25, 140, 150, 10};
short Worm_y[16] = {100, 195, 55, 40, 15, 140, 20, 135, 30, 20, 150, 45, 170, 15, 75, 5};

// the current X/Y velocity of the worm, if it was knocked back by an explosion, etc
char Worm_xVelo[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Worm_yVelo[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// the direction the worm is currently FACING.. 0 = LEFT 1 = RIGHT
unsigned long Worm_dir = 0;

// the current HEALTH of the worm
char Worm_health[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

// bit mask if the worm is dead. bit 1 = dead, bit = 0 alive
long Worm_isDead = 0;

// bit mask if the worm is ACTIVE... different that dead.
long Worm_active = 0;

// store the current mode for every worm:
char Worm_mode[16] = {wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };

// every time the team / turn switches a new worm is the "active" worm.
// if worm-select is enabled, during this period the play can change the active worm.
// the game-logic will update this variable, such that worms that are alive, and on the current team, will cycle
char Worm_currentWorm = 0;

// local function prototypes

/**
 * Spawns a Worm of given index on the Map.
 * 
 * @param index the worm to spawn.
*/
void spawnWorm(short);



// --------------------------------------------------------------------------------------------------------------------------------------



// spawns worms on the map
void Worm_spawnWorms()
{
	short i=0;
	for(i=0;i<Match_wormCount[0]; i++)
		spawnWorm(i);
	for(i=0;i<Match_wormCount[1]; i++)
		spawnWorm(8+i);
}

// spawns a single worm on the map
void spawnWorm(short index)
{
	// find a free place for it on the map
	Map_getSpawnPoint();
	Worm_x[index] = Map_lastRequestedSpawnX;
	Worm_y[index] = Map_lastRequestedSpawnY;
	
	// set active
	Worm_active |= (long)1<<(index);
	
	// random direction
	if(random(2)==0)
		Worm_dir |= (unsigned long)1<<(index);
}