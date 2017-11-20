// C Source File
// Created 11/11/2017; 11:34:05 PM

#include "../Headers/System/Main.h"

// worm variable defaults
short Worm_x[16] = {15, 81, 120, 65, 90, 35, 150, 40, 175, 95, 250, 210, 25, 140, 150, 10};
short Worm_y[16] = {100, 195, 55, 40, 15, 140, 20, 135, 30, 20, 150, 45, 170, 15, 75, 5};
char Worm_xVelo[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Worm_yVelo[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long Worm_dir = 0;
char Worm_health[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
long Worm_isDead = 0;
long Worm_active = 0;
char Worm_mode[16] = {wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };
char Worm_currentWorm = 0;

void Worm_spawnWorms();
void spawnWorm(short);

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
void spawnWorm(short i)
{
	// find a free place for it on the map
	Map_getSpawnPoint();
	Worm_x[i] = Map_lastRequestedSpawnX;
	Worm_y[i] = Map_lastRequestedSpawnY;
	
	// set active
	Worm_active |= (long)1<<(i);
	
	// random direction
	if(random(2)==0)
		Worm_dir |= (unsigned long)1<<(i);
}