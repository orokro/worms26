/*
	Match.h
	-------
	
	Main header file for the Match.c module
*/

#ifndef MATCH_H
#define MATCH_H


// match globals
extern char Match_wormCount[2];
extern char Match_gravestones[2];
extern char Match_wormStartHealth;
extern char Match_allowWormSelection;
extern char Match_artilleryMode;
extern char Match_turnTime;
extern char Match_minesEnabled;
extern char Match_mineFuseLength;
extern char Match_dudMines;
extern char Match_oilDrumsEnabled;
extern char Match_toolCratesEnabled;
extern char Match_healthCratesEnabled;
extern char Match_weaponCratesEnabled;
extern short Match_teamWeapons[2][65];
extern const char *Match_teamNames[2];
extern const char *Match_wormNames[16];

#endif