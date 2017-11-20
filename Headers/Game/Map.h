// Header File
// Created 11/11/2017; 11:31:20 PM

/*
	Map
	---
	
	This file defines the globals for our Map system.
*/

// when a spawn point is requested of the map, it will find one
// and update these global map variables:
extern short Map_lastRequestedSpawnX;
extern short Map_lastRequestedSpawnY;

// make a buffer for the map
extern void *mapBuffer;

// function prototypes
extern char Map_testPoint(short, short);
extern void Map_getSpawnPoint();
extern void Map_makeMap(void*);