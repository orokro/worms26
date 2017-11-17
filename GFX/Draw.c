// C Source File
// Created 11/11/2017; 11:34:12 PM
#include "../Headers/System/Main.h"


static void *mapPtr;

// crashes if I try this...
void setMapPtr(void *ptr)
{
	mapPtr=ptr;
}

// draw the map.. but I can't call this, crashes
void drawMap()
{
	// camera top position, in world-space
	short camTop = camY-50;
	
	// camera bottom position, in wrold-space
	short camBottom = camY+50;
	
	// we shouldn't continue if either is out of bounds
	if(!(camBottom<0) && !(camTop>199))
	{
		
		// the of the screen we should start copying the buffer to
		short screenTop=0;
		
		// the top row from the buffer we should start copying from
		short bufferTop=camTop;
		
		// the bottom of the screen we should stop copying the buffer to
		short screenBottom=99;
		
		// the bottom of the buffer should stop copying from
		short bufferBottom=camBottom;
		
		// if both the top and the bottom of the buffer points are inbounds
		// we can simplly draw the whole screen worth!
		// we don't need to change any variables
		
		// if, however, the top of the camera is is above the map
		// we need to change our render bounds..
		if(camTop<0)
		{
			// whatever we draw, will be from the top row of the buffer
			bufferTop = 0;
			
			// we want to draw lower on the screen... by how far the camera is beyond
			screenTop = (camTop*-1);
			
			// we want to only copy pixels for the rest of the screen
			screenBottom = 99;
			
			// we dont need to copy any more pixels from the buffer:
			bufferBottom = bufferTop+(200-screenTop);
		
		// also check if the bottom of the camera is beyond the map buffer
		}else if(camBottom>199)
		{
			// always draw on the top of the screen:
			screenTop = 0;
			
			// always draw from the camera-top in the buffer:
			bufferTop = camTop;
			
			// always draw the remaining rows in the buffer:
			bufferBottom = 200;
			
			// only draw that many rows:
			screenBottom = (200-bufferTop);
		}
		
		short *lcd = virtual;
		short *map = mapBuffer;
		short x,y;
		for(y=0; y<=(screenBottom-screenTop); y++)
			for(x=0; x<15; x++)
				lcd[(screenTop+y)*15+x] = map[(bufferTop+y)*30+x];
		
		//	memcpy (virtual, mapBuffer, LCD_SIZE);
		
	}// end if draw map
}


// main drawing routine for the game, e.g. map, worms, weapons, etc
void Draw_renderGame()
{
	// game modes by name	
	char modes[9][16] = {"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// clear screen
	ClrScr();
	
	/* ======================================================================================================================
		 DRAWING THE MAP +++ DRAWING THE MAP +++ DRAWING THE MAP +++ DRAWING THE MAP +++ DRAWING THE MAP +++ DRAWING THE MAP ++
	 	 ====================================================================================================================== */
	 	 
	// for some reason I can't put this in a method... the identical code copied from the method below
	//drawMap();	
	// camera top position, in world-space
	short camTop = camY-50;
	
	// camera bottom position, in wrold-space
	short camBottom = camY+50;
	
	// we shouldn't continue if either is out of bounds
	if(!(camBottom<0) && !(camTop>199))
	{
		
		// the of the screen we should start copying the buffer to
		short screenTop=0;
		
		// the top row from the buffer we should start copying from
		short bufferTop=camTop;
		
		// the bottom of the screen we should stop copying the buffer to
		short screenBottom=99;
		
		// the bottom of the buffer should stop copying from
		short bufferBottom=camBottom;
		
		// if both the top and the bottom of the buffer points are inbounds
		// we can simplly draw the whole screen worth!
		// we don't need to change any variables
		
		// if, however, the top of the camera is is above the map
		// we need to change our render bounds..
		if(camTop<0)
		{
			// whatever we draw, will be from the top row of the buffer
			bufferTop = 0;
			
			// we want to draw lower on the screen... by how far the camera is beyond
			screenTop = (camTop*-1);
			
			// we want to only copy pixels for the rest of the screen
			screenBottom = 99;
			
			// we dont need to copy any more pixels from the buffer:
			bufferBottom = bufferTop+(200-screenTop);
		
		// also check if the bottom of the camera is beyond the map buffer
		}else if(camBottom>199)
		{
			// always draw on the top of the screen:
			screenTop = 0;
			
			// always draw from the camera-top in the buffer:
			bufferTop = camTop;
			
			// always draw the remaining rows in the buffer:
			bufferBottom = 200;
			
			// only draw that many rows:
			screenBottom = (200-bufferTop);
		}
		
		// loop to manually copy memory from a sub-section of our map
		short *lcd = virtual;
		short *map = mapBuffer;
		short x,y;
		for(y=0; y<=(screenBottom-screenTop); y++)
			for(x=0; x<15; x++)
				lcd[(screenTop+y)*15+x] = map[(bufferTop+y)*30+x];
		
		//	memcpy (virtual, mapBuffer, LCD_SIZE);
		
	}// end if draw map
	/* ======================================================================================================================
		 END MAP +++ END MAP +++ END MAP +++ END MAP +++ END MAP +++ END MAP +++ END MAP +++ END MAP +++ END MAP +++ END MAP ++
	 	 ====================================================================================================================== */
	 	 
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
	ClrScr();
	DrawStr(0,0,"pause menu", A_XOR);
	
	DrawStr(0,10,(!menuItem ? "Continue" : "Quit Game"), A_NORMAL);
}


// main drawing routine for the weapons menu
void Draw_renderWeaponsMenu(char wx, char wy)
{
	ClrScr();
	DrawStr(0,0,"Weapons Menu", A_NORMAL);
	
	
	char weapStr[32];  
	sprintf(weapStr, "Selected: %d, %d", (short)wx, (short)wy);
	DrawStr(0,10,weapStr, A_NORMAL);
	
	
}
