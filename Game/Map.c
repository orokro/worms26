// C Source File
// Created 11/11/2017; 11:34:08 PM

#include "../Headers/System/Main.h"

// declare our external/global variables
short Map_lastRequestedSpawnX=0;
short Map_lastRequestedSpawnY=0;

// tests a point on the map
char Map_testPoint(short x, short y)
{
	// prevent warnings for now
	x=x;
	y=y;
	
	// TO-DO: implement
	return FALSE;
}

// find a free point to spawn something, that doesn't overlap with something else existing
void Map_getSpawnPoint()
{
	// TO-DO: implement
	Map_lastRequestedSpawnX=0;
	Map_lastRequestedSpawnY=0;
	
}