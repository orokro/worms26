// C Source File
// Created 11/11/2017; 11:34:12 PM
#include "../Headers/System/Main.h"


static void *mapPtr;

void setMapPtr(void *ptr)
{
	mapPtr=ptr;
}

void drawMap()
{
	char *lcd = LCD_MEM;
	char *map = mapBuffer;
	short x,y;
	for(x=0; x<30; x++)
	{
		for(y=0; y<127; y++)
			lcd[y*30+x] = map[y*60+x];
	}
}


// main drawing routine for the game, e.g. map, worms, weapons, etc
void Draw_renderGame()
{
	
	
	//enum GameModes {gameMode_WormSelect, gameMode_Turn, gameMode_WeaponSelect, gameMode_Pause, gameMode_Cursor, gameMode_TurnEnd, gameMode_Death, gameMode_AfterTurn, gameMode_GameOver};
	char modes[9][16] = {"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	ClrScr();
	
	//PortSet(mapBuffer, 477, 254);
	int camStartY=0;
	int camEndY=0;
	int screenStartOffset=0;
	int screenEndOffset=0;
	
	// which rows should we copy from the map buffer?
	short mapOriginX=239;
	short mapOriginY=127;
	
	// it should start 64 pixels above the cameras offset from the maps center
	camStartY = mapOriginY+camY-64;
	
	// if it's less than 0, just start drawing the map at row 0
	if(camStartY<0)
	{
		screenStartOffset = -camStartY;
		screenEndOffset = camStartY;
		camStartY=0;
	}
		
		
	// if the camera is super below the map, we dont need to draw the map at all:
	if(camStartY>254)
		return;
		
	// we should STOP drawing 63 pixels below the cameras offset from the maps center
	camEndY = mapOriginY+camY+63;
	
	// if the end point is greater than 254, it shoudl stop drawing the map after 254 in the map buffer
	if(camEndY>254)
	{
		screenStartOffset = 0;
		camEndY = camStartY+(camEndY-254);
	}
		
		
	// if the camera is super high up, no need to draw the map
	if(camEndY<0)
		return;
	
	// where in memory should it copy the map?
	
	char *lcd = virtual;
	char *map = mapBuffer;
	short x,y;
	short lcdY=0;
	for(x=0; x<30; x++)
	{
		lcdY=0;
		for(y=camStartY; y<camEndY; y++)
		{
			lcd[lcdY*30+x+screenStartOffset] = map[y*60+x];
			lcdY++;
		}
			
	}
	
//	memcpy (virtual, mapBuffer, LCD_SIZE);
	
	DrawStr(0,0,modes[(short)Game_mode], A_XOR);
	DrawStr(0,10,modes[(short)Game_previousMode], A_XOR);
	
	char timeStr[40];
	//short time = Game_timer/100;  
	sprintf(timeStr, "time: %d, %d, %d", (short)(Game_graceTimer/TIME_MULTIPLIER), (short)(Game_timer/TIME_MULTIPLIER), (short)(Game_retreatTimer));
	DrawStr(0,30,timeStr, A_XOR);	
	
	DrawStr(0,40, (Game_currentTeam ? "Team: Black" : "Team: White") , A_XOR);	
	
	char wormStr[20];
	sprintf(wormStr, "Worm Up: %d", (short)Game_currentWormUp[(short)Game_currentTeam]);
	
	DrawStr(0,50, wormStr , A_XOR);	
	
	
	char camStr[40];
	sprintf(camStr, "Cam: %d, %d", (short)camX, (short)camY);
	DrawStr(0,60, camStr , A_XOR);
	
	char heapStr[40];
	sprintf(heapStr, "heap: %lu", (unsigned long)HeapAvail());
	DrawStr(0,70, heapStr , A_XOR);
}


// main drawing routine for the pause menu
void Draw_renderPauseMenu(char menuItem)
{
	// prevent warnings for now
	menuItem=menuItem;
	ClrScr();
	DrawStr(0,0,"pause menu", A_XOR);
	
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
