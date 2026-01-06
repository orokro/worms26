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

// land styles
#define LAND_LAYERS 0
#define LAND_ISLANDS 1
#define LAND_TOWERS 2

// game drawing flags
#define DRAW_CLOUDS 0b00000001
#define DRAW_LEAVES 0b00000010
#define DRAW_MOUNTAINS 0b00000100

// match globals
extern char Match_wormCount[2];
extern char Match_gravestones[2];
extern unsigned char Match_wormStartHealth;
extern char Match_allowWormSelection;
extern char Match_artilleryMode;
extern char Match_turnTime;
extern char Match_strategicPlacement;
extern char Match_minesEnabled;
extern char Match_mineFuseLength;
extern char Match_dudMines;
extern char Match_oilDrumsEnabled;
extern char Match_toolCratesEnabled;
extern char Match_healthCratesEnabled;
extern char Match_weaponCratesEnabled;
extern char Match_suddenDeathEnabled;
extern char Match_mapType;
extern char Match_mapLandType;
extern char Match_drawingFlags;
extern char Match_defaultWeapons[65];
extern char Match_teamNames[2][10];
extern char Match_wormNames[16][12]; 

#endif
