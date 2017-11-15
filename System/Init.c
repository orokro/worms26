// C Source File
// Created 11/15/2017; 9:31:37 AM

#include "../Main.h"

// function prototypes
void Game_changeMode(char);
void Game_update();


extern char Game_mode;
extern char Game_previousMode;
extern short Game_timer;
extern short Game_retreatTimer;
extern short Game_graceTimer;
extern long Game_suddenDeathTimer;
extern long Game_waterLevel;
extern char Game_currentTeam;
extern char *Game_currentWormUp;
extern short Game_cursorX;
extern short Game_cursorY;
extern char Game_cursirDir;
extern short Game_xMarkSpotX;
extern short Game_xMarkSpotY;
extern char Game_xMarkPlaced;
extern char Game_xMarkAllowedOverLand;
extern char Game_cursorEndTurn;

extern short *Worm_x;
extern short *Worm_y;
extern char *Worm_xVelo;
extern char *Worm_yVelo;
extern char *Worm_dir;
extern char *Worm_health;
extern long Worm_isDead;
extern long Worm_active;
extern char *Worm_mode;
extern char Worm_currentWorm;

char foo[2] = {0,0};
char bar[16] = (char[16]){100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

void init()
{
	// Game settings
	Game_mode = gameMode_WormSelect;
	Game_previousMode = gameMode_WormSelect;
	Game_timer = 0;
	Game_retreatTimer = 5;
	Game_graceTimer = 5;
	Game_suddenDeathTimer = (long)((long)((long)60*(long)TIME_MULTIPLIER) * (long)10);
	Game_waterLevel = 0;
	Game_currentTeam = 1;
	Game_currentWormUp = &foo;// (char[2]){0, 0};
	
	// Game Cursor settings
	Game_cursorX = 0;
	Game_cursorY = 0;
	Game_cursirDir = 0;
	Game_xMarkSpotX = 0;
	Game_xMarkSpotY = 0;
	Game_xMarkPlaced = FALSE;
	Game_xMarkAllowedOverLand = TRUE;
	Game_cursorEndTurn = FALSE;

	// worm variable defaults
	Worm_x = (short[16]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Worm_y = (short[16]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Worm_xVelo = (char[16]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Worm_yVelo = (char[16]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Worm_dir = (char[16]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Worm_health = &bar;
	Worm_isDead = 0;
	Worm_active = 1;
	Worm_mode = (char[16]){wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };
	Worm_currentWorm = 0;


}


