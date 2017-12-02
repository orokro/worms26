// C Source File
// Created 11/11/2017; 11:34:12 PM

#include "Main.h"


#include "SpriteData.c"
  
/*
	Draw
	----
	
	Here we define our global drawing routines, for drawing each kind of scene:
		- Main game
		- Pause Menu
		- Weapons Menu
*/

// these sprites will be generated at the begining of the each turn to match the current wind conditions
unsigned long windSprites[3][3];


// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Convents world coordinates to screen coordinates, mutates the passed in pointers, and returns TRUE/FLASE if on screen (roughly).
 * 
 * The game is updated in world coordinates, but when it comes time to draw,
 * everything needs to be in screen coordinates.
 *
 * This method takes in the Camera's X/Y position and adjusts the passed in X/Y pointers accordingly.
 * 
 * If the final X/Y screen position determined is out of bounds for the screen, the method will return FALSE.
 * Otherwise, TRUE.
 *
 * The current bounding for the screen is +/- 8 pixels horizzontal on each side of the screen, and
 * +/- 16 pixels vertically.
 *
 * This may need to be changed, as the final sprites are decided
 *
 * @param x a pointer reference to the x value to change to screen coordinates
 * @param y a poitner reference to the y value to change to screen coordinates
 * @return a char boolean either TRUE or FALSE if the coordinates are roughly on screen
*/
char worldToScreen(short *x, short *y)
{
	*x = *x-(camX-80);
	*y = *y-(camY-50);
	
	// if anything is out of bounds, return false:
	if(*x<-8 || *x>168 || *y<-16 || *y>116)
		return FALSE;
	else
		return TRUE;
}

/**
	Draws all the in-game, on-screen Worms.
*/
void drawWorms()
{
	short screenX, screenY;
	
	// loop over all worms and draw them if active:
	short i;
	for(i=0; i<MAX_WORMS; i++)
	{
		if(Worm_active & (unsigned short)1<<(i))
		{
			screenX=Worm_x[i];
			screenY=Worm_y[i];
			if(worldToScreen(&screenX, &screenY))
			{
				// get the postion and direction the worm is facing..
				short x = screenX-8;
				short y = screenY-8;
				char facing = (Worm_dir & (unsigned short)1<<(i))>0;

				// use the worms fill to erase the background on it's opposite color plane, acting as a mask
				ClipSprite16_AND_R(x, y, 15, (facing ? spr_WormLeft_Mask : spr_WormRight_Mask), ((i>=8) ? lightPlane : darkPlane));
				
				// draw the worms fill and outline
				if(i<8)
				{
					ClipSprite16_AND_R(x, y, 15, (facing ? spr_WormLeft_Mask : spr_WormRight_Mask), lightPlane);
					ClipSprite16_OR_R(x, y, 15, (facing ? spr_WormLeft_Outline : spr_WormRight_Outline), darkPlane);
					ClipSprite16_OR_R(x, y, 15, (facing ? spr_WormLeft_Outline : spr_WormRight_Outline), lightPlane);
				}	
				else
				{
					ClipSprite16_OR_R(x, y, 15, (facing ? spr_WormLeft_Fill : spr_WormRight_Fill), darkPlane);
					ClipSprite16_OR_R(x, y, 15, (facing ? spr_WormLeft_Outline : spr_WormRight_Outline), lightPlane);
				}
				
				// draw health above worm
				ClipSprite16_OR_R(x, y-8, 7, Worm_HealthSprite[i], lightPlane);
				ClipSprite16_OR_R(x, y-8, 7, Worm_HealthSprite[i], darkPlane);
				
				//char foo = (char)((Worm_settled & ((unsigned short)1<<(i)))>0);
				//DrawChar(x, y-15, (foo ? (char)20 : 'X'), A_NORMAL);
				
			}// end if on screen
		}// end if active
	}// next i
	
	// for debug: draw collider for current worm:
	/*
	short x=Worm_x[(short)Worm_currentWorm];
	short y=Worm_y[(short)Worm_currentWorm];
	if(worldToScreen(&x, &y)==TRUE)
	{
		DrawLine(x-2, y, x+2, y, A_NORMAL);
		DrawLine(x, y-4, x, y+6, A_NORMAL);
	}
	*/
}

/**
	Draws all the in-game, on-screen Mines.
*/
void drawMines()
{
	short screenX, screenY;
	
	char fuseStr[8];
	
	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<MAX_MINES; i++)
	{
		if(Mine_active & (unsigned short)1<<(i))
		{
			screenX=Mine_x[i];
			screenY=Mine_y[i];
			if(worldToScreen(&screenX, &screenY))
			{				
				// draw the mines fill and outline
				ClipSprite8_OR_R(screenX-3, screenY-1, 4, spr_Mine_Dark, darkPlane);
				ClipSprite8_OR_R(screenX-3, screenY-1, 4, spr_Mine_Light, lightPlane);
				
				// if the mine has an active fuse, draw that too
				if(Mine_fuse[i]>0)
				{
					sprintf(fuseStr, "%d", (Mine_fuse[i]/TIME_MULTIPLIER));
					DrawStr(screenX-4, screenY-16, fuseStr, A_NORMAL);
				}// end if fuse
				
				// if the oil drum is "settled" draw an arrow above it, for debug
				//char foo = (Mine_settled & (unsigned short)1<<(i));
				//DrawChar(screenX, screenY-8, (foo ? (char)20 : 'X'), A_NORMAL);
				
				/*
				char txt[4];
				sprintf(txt, "%d", (short)Mine_yVelo[i]);
				DrawStr(screenX-3, screenY-8, txt, A_NORMAL);
				*/
				
			}// end if on screen
		}// end if active
	}// next i
}

/**
	Draws all the in-game, on-screen Oil Drums.
*/
void drawOilDrums()
{
	short screenX, screenY;

	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<MAX_OILDRUMS; i++)
	{
		if(OilDrum_active & (unsigned short)1<<(i))
		{
			screenX=OilDrum_x[i];
			screenY=OilDrum_y[i];
			if(worldToScreen(&screenX, &screenY))
			{
				// draw the oil drums fill and outline
				ClipSprite16_OR_R(screenX-4, screenY-5, 11, spr_Oil_Dark, darkPlane);
				ClipSprite16_OR_R(screenX-4, screenY-5, 11, spr_Oil_Light, lightPlane);
				
				
				// if the oil drum is "settled" draw an arrow above it, for debug
				//if(OilDrum_settled & (unsigned short)1<<(i))
				//	DrawChar(screenX, screenY-10, (char)20, A_NORMAL);
				
				/*
				char txt[4];
				sprintf(txt, "%d", (short)OilDrum_yVelo[i]);
				DrawStr(screenX-4, screenY-20, txt, A_NORMAL);
				*/
				
			}// end if on screen
		}// end if active
	}// next i
	
}


/**
	Draws all the in-game, on-screen Crates.
*/
void drawCrates()
{
	short screenX, screenY;
	
	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<MAX_CRATES; i++)
	{
		if(Crate_active & (unsigned short)1<<(i))
		{
			screenX=Crate_x[i];
			screenY=Crate_y[i];
			if(worldToScreen(&screenX, &screenY))
			{
				DrawClipChar(screenX-4, screenY-8, 'C', (&(SCR_RECT){{0, 0, 159, 99}}), A_XOR);
				if(Crate_type[i]==crateHealth)
					DrawClipChar(screenX-4, screenY-16, 'H', (&(SCR_RECT){{0, 0, 159, 99}}), A_XOR);
				else if(Crate_type[i]==crateWeapon)
					DrawClipChar(screenX-4, screenY-16, 'W', (&(SCR_RECT){{0, 0, 159, 99}}), A_XOR);
				else
					DrawClipChar(screenX-4, screenY-16, 'T', (&(SCR_RECT){{0, 0, 159, 99}}), A_XOR);
				
			}// end if on screen
		}// end if active
	}// next i
}


/**
 * draw falling leave wind speed indicators
*/
void drawLeaves()
{
	// no need for external data, only four leaves at a time so we can keep it static
	static short Leaf_x[] = {0,		40,		80,		120};
	static short Leaf_y[] = {0, 	25,		50,		75};
	
	// leaves always move down at a constant rate, and sideways by the wind rate
	short i=0;
	for(i=0; i<1; i++)
	{
		Leaf_y[i]++;
		Leaf_x[i]+=Game_wind/4;
		
		short xPos = Leaf_x[i];
		short yPos = Leaf_y[i];
		
		if(worldToScreen(&xPos, &yPos))
			ClipSprite16_OR_R(xPos, yPos, 8, spr_Leaf, lightPlane);
		else if(yPos>100)
		{
			if(abs(Game_wind)<5)
			{
				Leaf_x[i] = camX+random(160)-80;
				Leaf_y[i] = camY-58;
			}else
			{
				Leaf_x[i] = camX + ((Game_wind<0) ? 90 : -90);
				Leaf_y[i] = camY-61 + random(70);
			}
			
		}
	}	
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
	
	// camera left position, in world-space
	short camLeft = camX-80;
	
	// camera right position, in world-space
	short camRight = camX+80;
	
	// we shouldn't continue if either is out of bounds
	if(!(camBottom<0) && !(camTop>199) && !(camLeft>319) && !(camRight<0))
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
		
		// the left side of the screen, we should start drawing to
		short screenLeft = 0;
		
		// the right side of the screen, we should stop drawing at
		short screenRight = 160;
		
		// the position in the buffer we should start copying left-to-right
		short bufferLeft = camLeft;
		
		// if the screen's left is out of bounds of our map buffer
		// then we should start drawing in the middle of the screen...
		if(camLeft<0)
		{
			// the screen left should be the same as how far OB the camera is.
			// if the camera is 10 pixels to the left of the map, the map
			// should draw at 10px right on the screen...
			screenLeft = (camLeft*-1);
			
			// since we're starting at the left of the map, we will draw
			// all the way to the right edge of the screen:
			screenRight = 160;
			
			// since we're starting at the left of the screen, we will
			// start at the left of the buffer:
			bufferLeft=0;
			
		// if the right of the camera is beyond the right edge of the buffer...
		}else if(camRight>319)
		{
			// we will definately start drawing on the left side of the screen
			screenLeft = 0;
			
			// we will start the buffer at the left-camera edge:
			bufferLeft = camLeft;
			
			// we only need to draw until the buffer runs out..
			screenRight = 160; //320-bufferLeft;
		}
		
		// because we are going to be copying bits in blocks of 32 at a time below
		// we need to convert our screenLeft, screenRight and bufferLeft into columns
		short colLeft = (screenLeft-(screenLeft%32))/32;
		short colRight = (screenRight-(screenRight%32))/32+1;
		short colBuff = (bufferLeft-(bufferLeft%32))/32;

		// loop to manually copy memory from a sub-section of our map, via ClipSprite32
		unsigned long *light = (unsigned long*)mapLight;
		unsigned long *dark = (unsigned long*)mapDark;
		short x;
		short bufferCol = 0;
		
		// loop over the visible columns of the map on screen, or till our buffer runs out
		for(x=colLeft; (x<colRight && (colBuff+bufferCol)<10); x++)
		{
			short offset = screenLeft==0 ? camLeft%32 : 0;

			// take advantage of extgrah's sprite method to handle bit shiting and mem copying in one swoop!
			ClipSprite32_OR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &light[((colBuff+bufferCol)*200)+bufferTop], lightPlane);
			ClipSprite32_XOR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &dark[((colBuff+bufferCol)*200)+bufferTop], lightPlane);
			ClipSprite32_OR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &dark[((colBuff+bufferCol)*200)+bufferTop], darkPlane);

			// on the next iteration we will be on the next buffer 32 bit colum
			bufferCol++;
		
		}// next x
		
	}// end if draw map
}


/**
 * Draws either the selection arrow, or current worm arrow in Worm Select mode.
*/
void drawSelectArrow()
{
	static char frame=0;
	frame++;
	if(frame>7)
		frame=0;
	
	short x=Worm_x[(short)Worm_currentWorm]-9;
	short y=Worm_y[(short)Worm_currentWorm]-32;
	
	if(worldToScreen(&x, &y))
	{
		// take advantage of extgrah's sprite method to handle bit shiting and mem copying in one swoop!
		ClipSprite16_XOR_R(x, y, 16, ((frame<4) ? spr_SelectionArrowFrame1 : spr_SelectionArrowFrame2), darkPlane);
		ClipSprite16_XOR_R(x, y, 16, ((frame<4) ? spr_SelectionArrowFrame1 : spr_SelectionArrowFrame2), lightPlane);	
	}
}


/**
 * Draws a grayscale parallax mountain range in the background.
 */
void drawMountains()
{
	short i;
	short offsetX = (camX-160)/5;
	short offsetY = camY/5;
	for(i=0; i<5; i++)
		ClipSprite32_OR_R(i*32-offsetX, 60-offsetY, 38, spr_Mountain[i], lightPlane);

}



/**
 * Draws the games HUD (i.e. timer, wind, etc)
*/
void drawHUD()
{		

	// copy the wind bar to both buffers:
	ClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, lightPlane);
	ClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, darkPlane);
	ClipSprite32_OR_R(126, 93, 5, spr_WindMeter, lightPlane);
	ClipSprite32_OR_R(126, 93, 5, spr_WindMeter, darkPlane);
	
	// current frame of annimation:
	static char frame=0;
	if(++frame>33)
		frame=0;
	
	// draw current frame of animated bar
	ClipSprite32_OR_R(127, 94, 3, windSprites[(short)((frame/3)%3)], darkPlane);
	
	// only draw the timer if the game is in select or turn mode!
	if(Game_timer>0 && (Game_mode==gameMode_WormSelect || Game_mode==gameMode_Turn || Game_mode==gameMode_Cursor))
	{
		// exrase space for timer
		ClipSprite16_AND_R(2, 87, 11, spr_timerMask, lightPlane);
		ClipSprite16_AND_R(2, 87, 11, spr_timerMask, darkPlane);
		
		// if we have trace time, blink the border:
		if(Game_graceTimer>0 && (Game_graceTimer/3)%6<3)
		{
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, lightPlane);
		}else
		{
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, lightPlane);
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, darkPlane);
		}
		
		// draw the time in the turn:
		short time = (Game_graceTimer>0) ? (short)(Game_graceTimer/TIME_MULTIPLIER) : (short)(Game_timer/TIME_MULTIPLIER);
		char txt[3];
		sprintf(txt, "%d", time);
		
		GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
		DrawStr(3 + ((time<10) ? 3 : 0), 89, txt, A_NORMAL);
		GrayDBufSetHiddenAMSPlane(DARK_PLANE);
		DrawStr(3 + ((time<10) ? 3 : 0), 89, txt, A_NORMAL);
		
		// if the game timer is less than 5, the inside, not the border
		if((short)(Game_timer/TIME_MULTIPLIER)<=5 && (Game_timer/3)%6<3)
			ClipSprite16_XOR_R(2, 87, 11, spr_timerBlink, darkPlane);
		
	}// end if draw timer	
	
}



/**
 * draws the water infront of the map / objects
*/
void drawWater()
{
	// frame and frame timer
	static char frame=0;
	static char frameTimer=0;
	if(++frameTimer>=2)
	{
		frameTimer=0;
		if(++frame>7)
			frame=0;
	}		
	
	static char frames[] = {0, 1, 2, 3, 4, 3, 2, 1};
	short f = frames[(short)frame];
	
	short waterY = 241-camY;
	short i;
	for(i=0; i<7; i++)
	{
		short waterX = i*32 - (camX+328)%32;
	
		// draw just the and dark light planes:
		ClipSprite32_MASK_R(waterX, waterY, 9, &spr_Water_Dark[0+(f*9)], &spr_Water_Mask[0+(f*9)], darkPlane);
		ClipSprite32_AND_R(waterX, waterY, 9, &spr_Water_Mask[0+(f*9)], lightPlane);
		ClipSprite32_OR_R(waterX, waterY, 23, &spr_Water_Light[0+(f*23)], lightPlane);
	}
	
}

// --------------------------------------------------------------------------------------------------------------------------------------



// draws cake loading screen
void Draw_cake(short amount, short total)
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	ClipSprite32_OR_R(64, 38, 23, spi_CandleCake_Dark, darkPlane);
	ClipSprite32_OR_R(64, 38, 23, spi_CandleCake_Light, lightPlane);	
	
	char progStr[40];
	sprintf(progStr, "Loading... %d%%", (short)((float)((float)amount/(float)total)*100));
	DrawStr(40,70,progStr, A_NORMAL);	

	// now flip the planes
	GrayDBufToggleSync();
	
	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
}

// main drawing routine for the game, e.g. map, worms, weapons, etc
void Draw_renderGame()
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	// draw background mountains
	drawMountains();
	
	// leaves before map...
	drawLeaves();
	
	// draw the map
	drawMap();	
	
	// draw oil drums first, as everything else should overlap them
	drawOilDrums();
	
	// draw crates ontop of oil drums...
	drawCrates();
	
	// draw our wormy bros
	drawWorms();
	
	// mines are important, so draw them on top of everything else
	drawMines();
	
	// draw water that's infront of everything but the HUD
	drawWater(FALSE);
	
	// HUD stuff is drawn last since it needs to go on top of all game elements
	drawHUD();
	
	// if the game mode is worm select, draw the selection arrow
	if(Game_mode==gameMode_WormSelect)
		drawSelectArrow();
		
	// for now, we will output a bunch of debug info on the screen
	
	// game modes by name	{"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	static const char modes[9][16] = {"", "", "", "", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// draw the current and previous game mode on the scren
	DrawStr(60,1,modes[(short)Game_mode], A_XOR);

	// draw the current grace time, turn time, and retreat time on the screen
	// NOTE: for some reason, drawing sudden death time instead of retreat time crashes the game)
	//char timeStr[40];
	//sprintf(timeStr, "time: %d, %d, %d", (short)(Game_graceTimer/TIME_MULTIPLIER), (short)(Game_timer/TIME_MULTIPLIER), (short)(Game_retreatTimer));
	//DrawStr(60,1,timeStr, A_XOR);	
	
	//unsigned short currentMask = 1;
	//currentMask = (unsigned short)((unsigned short)currentMask<<(Worm_currentWorm));
	//DrawStr(80,10, (((Worm_onGround & currentMask)>0) ? "Grounded" : "Air"), A_XOR);
	
	// draw the current team on the screen	
	//DrawStr(0,40, (Game_currentTeam ? "Team: Black" : "Team: White") , A_XOR);	
	
	// drwa the current worm on the screen
	//char wormStr[20];
	//sprintf(wormStr, "Worm Up: %d", (short)Game_currentWormUp[(short)Game_currentTeam]);
	//DrawStr(0,50, wormStr , A_XOR);	
	
	// draw the camera's position on the screen
	//char camStr[40];
	//sprintf(camStr, "Cam: %d, %d", (short)camX, (short)camY);
	//DrawStr(0,60, camStr , A_XOR);
	
	// draw our free memory on the screen, only when shift is held for debbuging
	if(Keys_keyState(keyCameraControl))
	{
		char heapStr[40];
		sprintf(heapStr, "%lu", (unsigned long)HeapAvail());
		DrawStr(0,1, heapStr , A_XOR);
	}
}


// main drawing routine for the pause menu
void Draw_renderPauseMenu(char menuItem)
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	short z=0;
	for(z=0; z<2; z++)
	{
		GrayDBufSetHiddenAMSPlane((z%2==0) ? DARK_PLANE : LIGHT_PLANE);
		
		// draw the pause menu, and which menu item is currently selected
		DrawStr(0,0,"pause menu", A_XOR);
		DrawStr(0,10,(!menuItem ? "Continue" : "Quit Game"), A_NORMAL);
	}
}


// main drawing routine for the weapons menu
void Draw_renderWeaponsMenu(char wx, char wy)
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	short z=0;
	for(z=0; z<2; z++)
	{
		GrayDBufSetHiddenAMSPlane((z%2==0) ? DARK_PLANE : LIGHT_PLANE);

		// draw the weapons menu and the X/Y position of the selected weapon in the menu
		DrawStr(0,0,"Weapons Menu", A_NORMAL);
		char weapStr[32];  
		sprintf(weapStr, "Selected: %d, %d", (short)wx, (short)wy);
		DrawStr(0,10,weapStr, A_NORMAL);	
	}
}



