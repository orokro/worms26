// Header File
// Created 11/11/2017; 11:31:20 PM

/*
	Map
	---
	
	This file defines the globals for our Map system

*/

// when a spawn point is requested of the map, it will find one
// and update these global map variables:
short Map_lastRequestedSpawnX=0;
short Map_lastRequestedSpawnY=0;

// function prototypes
char Map_testPoint(short, short);
void Map_getSpawnPoint();

