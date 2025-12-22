/*
	Game.h
	------
	
	Main header file for the game.c module
*/
#ifndef GAME_H
#define GAME_H


// game enums
enum GameModes {
	gameMode_WormSelect,
	gameMode_Turn,
	gameMode_WeaponSelect,
	gameMode_Pause,
	gameMode_Cursor,
	gameMode_TurnEnd,
	gameMode_Death,
	gameMode_AfterTurn,
	gameMode_GameOver
};

// flags for current weapon state
#define targetPicked 	0b00000001
#define strikeLeft  	0b00000010
#define firstShotTaken  0b00000100

// game globals
extern char Game_mode;
extern char Game_previousMode;
extern short Game_timer;
extern short Game_retreatTimer;
extern short Game_graceTimer;
extern long Game_suddenDeathTimer;
extern char Game_wind;
extern char Game_waterLevel;
extern char Game_currentTeam;
extern char Game_currentWormUp[2];
extern short Game_cursorX;
extern short Game_cursorY;
extern char Game_cursorDir;
extern short Game_xMarkSpotX;
extern short Game_xMarkSpotY;
extern char Game_xMarkPlaced;
extern char Game_xMarkAllowedOverLand;
extern char Game_cursorEndTurn;
extern char Game_turn;
extern short Game_weapInventory[5][13];
extern char Game_currentWeaponSelected;
extern unsigned long Game_currentWeaponProperties;
extern unsigned short Game_currentWeaponState;
extern unsigned short Game_currentWeaponCharge;
extern char Game_aimAngle;


// game function prototypes

/**
 * Changes the Games primary state machine to a new mode.
 *
 * @param newMode a char representing a game mode as defined in the GameModes enum.
*/
extern void Game_changeMode(char);


/**
 * The main update method for the entire game, should be called once per frame.
 *
 * The Game_update() method handles general every-frame logic,
 * as well as calling the current game states update method as well.
 * This is called every frame in the main-loop.
*/
extern void Game_update();


/**
	* Returns TRUE or FALSE if all he game items in the game are settled and not doing anything.
	* 
	* Crates, Mines, Drums, and Worms must be settled on the ground.
	* No active mine timers allowed.
	* No active explosions.
	* No active weapons
	*
	* @return TRUE or FALSE if everything is settled or not
*/
extern char Game_allSettled();

#endif