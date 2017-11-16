// C Source File
// Created 11/11/2017; 11:34:12 PM
#include "../Headers/System/Main.h"

// main drawing routine for the game, e.g. map, worms, weapons, etc
void Draw_renderGame()
{
	//enum GameModes {gameMode_WormSelect, gameMode_Turn, gameMode_WeaponSelect, gameMode_Pause, gameMode_Cursor, gameMode_TurnEnd, gameMode_Death, gameMode_AfterTurn, gameMode_GameOver};
	char modes[9][16] = {"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	ClrScr();
	DrawStr(0,0,modes[(short)Game_mode], A_NORMAL);
	DrawStr(0,10,modes[(short)Game_previousMode], A_NORMAL);
	
	char timeStr[40];
	//short time = Game_timer/100;  
	sprintf(timeStr, "time: %d, %d, %d", (short)(Game_graceTimer/TIME_MULTIPLIER), (short)(Game_timer/TIME_MULTIPLIER), (short)(Game_retreatTimer));
	DrawStr(0,30,timeStr, A_NORMAL);	
	
	DrawStr(0,40, (Game_currentTeam ? "Team: Black" : "Team: White") , A_NORMAL);	
	
	char wormStr[20];
	sprintf(wormStr, "Worm Up: %d", (short)Game_currentWormUp[(short)Game_currentTeam]);
	
	DrawStr(0,50, wormStr , A_NORMAL);	
	
	
	char camStr[40];
	sprintf(camStr, "Cam: %d, %d", (short)camX, (short)camY);
	DrawStr(0,60, camStr , A_NORMAL);
}


// main drawing routine for the pause menu
void Draw_renderPauseMenu(char menuItem)
{
	// prevent warnings for now
	menuItem=menuItem;
	ClrScr();
	DrawStr(0,0,"pause menu", A_NORMAL);
	
	DrawStr(0,10,(!menuItem ? "Continue" : "Quit Game"), A_NORMAL);
}


// main drawing routine for the weapons menu
void Draw_renderWeaponsMenu(char wx, char wy)
{
	// prevent warnings for now
	wx=wx;
	wy=wy;
	ClrScr();
	DrawStr(0,0,"Weapons Menu", A_NORMAL);
	
	
	char weapStr[32];  
	sprintf(weapStr, "Selected: %d, %d", (short)wx, (short)wy);
	DrawStr(0,10,weapStr, A_NORMAL);
	
	
}
