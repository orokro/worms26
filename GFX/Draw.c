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
	/*
			How this works:
			
			Our mapBuffer pointer points a buffer that's four times the normal LCD_SIZE.
			
			We only want to draw a sub-section of that buffer on the screen.
			
			For vertical:
			
			The calculators screen height is 100 pixels, so we should draw 50 pixels above and below
			the camera's "center"
			
			So: CameraTop = CameraY-50, and CameraBottom = CameraY+50
			
			The rows we draw on the screen will always be between 0 and 99 inclusive.
			
			So we need to determine the following:
				- The top most row on the screen to draw
				- The bottom most row on the screen to draw
				- The top row from the buffer we should start copying.
				
			If the camera goes out of bounds of the map buffer, we can still draw part of the map
			In this case, there are a few conditions:
				- If the Camera goes above the map (CameraTop is less than 0 in the map-buffer)
					then we can still draw the map, but ScreenTop will be lower on the screen
				- If the Camera goes below the map (CameraBottom is more than 200 in the map-buffer)
					then we can still draw the map. ScreenTop will be 0, but we'll only draw till the end of the buffer
				- If either the CameraBottom is less than 0 or CameraTop is greater than 200 then the map is compeltey
					off screen, and we don't need to draw it at all.
					
			This way, the camera can still pan outside the map, into nothingness.
			This is useful for following weapons or worms that fly far off the map, or projectiles that fly high
			into the sky.
	*/
	
	// camera top position, in world-space
	short camTop = camY-50;
	
	// camera bottom position, in wrold-space
	short camBottom = camY+50;
	
	// we shouldn't continue if either is out of bounds
	if(!(camBottom<0) && !(camTop>199))
	{
		
		// the of the screen we should start copying the buffer to
		short screenTop=0;
		
		// the bottom of the screen we should stop copying the buffer to
		short screenBottom=99;
		
		// the top row from the buffer we should start copying from
		// note: we don't need a buffer bottom, since the loop is controlled from screenTop to screenBottom
		// so we only really need bufferTop in the loop
		short bufferTop=camTop;

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

		// also check if the bottom of the camera is beyond the map buffer
		}else if(camBottom>199)
		{
			// always draw on the top of the screen:
			screenTop = 0;
			
			// always draw from the camera-top in the buffer:
			bufferTop = camTop;

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
}


// main drawing routine for the game, e.g. map, worms, weapons, etc
void Draw_renderGame()
{
	// clear screen
	ClrScr();
	
	// for some reason I can't put this in a method... the identical code copied from the method below
	drawMap();	
	 
	// for now, we will output a bunch of debug info on the screen
	
	// game modes by name	
	char modes[9][16] = {"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// draw the current and previous game mode on the scren
	DrawStr(0,0,modes[(short)Game_mode], A_XOR);
	DrawStr(0,10,modes[(short)Game_previousMode], A_XOR);
	
	// draw the current grace time, turn time, and retreat time on the screen
	// NOTE: for some reason, drawing sudden death time instead of retreat time crashes the game)
	char timeStr[40];
	sprintf(timeStr, "time: %d, %d, %d", (short)(Game_graceTimer/TIME_MULTIPLIER), (short)(Game_timer/TIME_MULTIPLIER), (short)(Game_retreatTimer));
	DrawStr(0,30,timeStr, A_XOR);	

	// draw the current team on the screen	
	DrawStr(0,40, (Game_currentTeam ? "Team: Black" : "Team: White") , A_XOR);	
	
	// drwa the current worm on the screen
	char wormStr[20];
	sprintf(wormStr, "Worm Up: %d", (short)Game_currentWormUp[(short)Game_currentTeam]);
	DrawStr(0,50, wormStr , A_XOR);	
	
	// draw the camera's position on the screen
	char camStr[40];
	sprintf(camStr, "Cam: %d, %d", (short)camX, (short)camY);
	DrawStr(0,60, camStr , A_XOR);
	
	// draw our free memory on the screen
	char heapStr[40];
	sprintf(heapStr, "heap: %lu", (unsigned long)HeapAvail());
	DrawStr(0,70, heapStr , A_XOR);
	
	DrawStr(0,80, ((Weapon_active>0)?"W on" : "W off") , A_XOR);
}


// main drawing routine for the pause menu
void Draw_renderPauseMenu(char menuItem)
{
	// clear the screen
	ClrScr();
	
	// draw the pause menu, and which menu item is currently selected
	DrawStr(0,0,"pause menu", A_XOR);
	DrawStr(0,10,(!menuItem ? "Continue" : "Quit Game"), A_NORMAL);
}


// main drawing routine for the weapons menu
void Draw_renderWeaponsMenu(char wx, char wy)
{
	// clear the screen
	ClrScr();
	
	// draw the weapons menu and the X/Y position of the selected weapon in the menu
	DrawStr(0,0,"Weapons Menu", A_NORMAL);
	char weapStr[32];  
	sprintf(weapStr, "Selected: %d, %d", (short)wx, (short)wy);
	DrawStr(0,10,weapStr, A_NORMAL);	
}
