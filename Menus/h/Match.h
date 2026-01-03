/*
	Match.h
	-------
	
	Main header file for the Match.c module
*/

#ifndef MATCH_H
#define MATCH_H

// defines

// map styles
#define MAP_SAND 0
#define MAP_STONE 1
#define MAP_BRICKS 2

// game drawing flags
#define DRAW_CLOUDS 0b00000001
#define DRAW_LEAVES 0b00000010
#define DRAW_MOUNTAINS 0b00000100

// match globals
extern char Match_wormCount[2]; // implemented
extern char Match_gravestones[2]; // implemented
extern unsigned char Match_wormStartHealth; // implemented
extern char Match_allowWormSelection; // implemented
extern char Match_artilleryMode;
extern char Match_turnTime; // implemented
extern char Match_strategicPlacement;
extern char Match_minesEnabled; // implemented
extern char Match_mineFuseLength; // implemented
extern char Match_dudMines; // implemented
extern char Match_oilDrumsEnabled; // implemented
extern char Match_toolCratesEnabled; // implemented
extern char Match_healthCratesEnabled; // implemented
extern char Match_weaponCratesEnabled; // implemented
extern char Match_mapType; // implemented
extern char Match_drawingFlags; // implemented
extern char Match_defaultWeapons[65]; // implemented
extern char Match_teamNames[2][10]; // implemented
extern char Match_wormNames[16][12];  // implemented

#endif