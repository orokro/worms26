// Header File
// Created 11/11/2017; 11:31:20 PM

/*
	Map
	---
	
	This file defines the globals for our Map system

*/

// when a spawn point is requested of the map, it will find one
// and update these global map variables:
extern short Map_lastRequestedSpawnX;
extern short Map_lastRequestedSpawnY;

// function prototypes
char Map_testPoint(short, short);
void Map_getSpawnPoint();

