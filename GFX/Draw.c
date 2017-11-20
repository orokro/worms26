// C Source File
// Created 11/11/2017; 11:34:12 PM
#include "../Headers/System/Main.h"

char worldToScreen(short*, short*);
void drawWorms();
void drawMines();
void drawOilDrums();
void drawCrates();

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
			// all the way to the right edge o the screen:
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
		
		// because we are going to be copying bits in blocks of 16 at a time below
		// we need to convert our screenLeft, screenRight and bufferLeft into columns
		short colLeft = (screenLeft-(screenLeft%16))/16;
		short colRight = (screenRight-(screenRight%16))/16;
		short colBuff = (bufferLeft-(bufferLeft%16))/16;

		// loop to manually copy memory from a sub-section of our map
		unsigned short *lcd = virtual;
		unsigned short *map = mapBuffer;
		short x,y, bufferCol;
		
		// loop through the visible rows on the screen
		for(y=0; y<=(screenBottom-screenTop); y++)
		{
			bufferCol=0;
			
			// loop over the visible columns of the map on screen, or till our buffer runs out
			for(x=colLeft; (x<colRight && (colBuff+bufferCol)<20); x++)
			{
			
				// we want to bit-shift the map if the camera's position isn't on an even division of 8
				unsigned short screenData = map[(bufferTop+y)*30+colBuff+bufferCol];
				short offset = camLeft%16;
				if(offset!=0)
				{
				
					// if the camera is negative we need slightly different logic
					if(offset<0)
					{
						// make our off set positive, since it was the result of % on a negative camLeft
						offset *= -1;
						
						// shift right by the scroll offset:
						screenData = (screenData >> offset);
						
						// we don't need to copy from the previous tile, if we're in the first row:
						if((colBuff+bufferCol)>0)
						{
								
							// we also want to copy the lower bits from the previous tile over:
							unsigned short previousTile = map[(bufferTop+y)*30+colBuff+bufferCol-1];
							
							// move these bits left, so the lower bits become the missing upper bits on our data
							// if our camera is offset by 6, we will have shift everything 16-6, and have 6 open spots on the right
							previousTile = previousTile << (16-offset);
							
							// now if we OR our two data sets together, we should have the correctly scrolled data
							screenData |= previousTile;
							
							// also note, that, since we're negative, our screenX should increase by 1
							screenLeft++;
							
						}// end if left edge of buffer
					
					// otherwise, we're offset, but in a positive region
					}else
					{
						// shift left by the scroll offset:
						screenData = (screenData << offset);
						
						// we don't need to copy from the next tile, if we're on the last tile of the map:
						if((colBuff+bufferCol+1) < 20)
						{
								
							// we also want to copy the upper bits from the next tile over:
							unsigned short nextTile = map[(bufferTop+y)*30+colBuff+bufferCol+1];
							
							// move these bits right, so the upper bits become the missing lower bits on our data
							// if our camera is offset by 6, we will have shift everything 6, and have 6 open spots on the right
							// therefore, this needs to be shift 16-6 to the right:
							nextTile = nextTile >> (16-offset);
							
							// now if we OR our two data sets together, we should have the correctly scrolled data
							screenData |= nextTile;
							
						}// end if right edge of buffer
					}// end if negative scroll
				}// end if is offset
				
				// copy the scroll-offset memory to our screen location
				lcd[(screenTop+y)*15+x] = screenData;
				
				// on the next iteration we will be on the next buffer colum
				bufferCol++;
			
			}// next x
			
		}// next y		
		
		//	memcpy (virtual, mapBuffer, LCD_SIZE);
		
	}// end if draw map
}

// convert world coordinates to screen coordinates
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

// draw all the worms in the game
void drawWorms()
{
	short screenX, screenY;
	
	// loop over all worms and draw them if active:
	short i;
	for(i=0; i<16; i++)
	{
		if(Worm_active & (long)1<<(i))
		{
			screenX=Worm_x[i];
			screenY=Worm_y[i];
			if(worldToScreen(&screenX, &screenY)==TRUE)
				DrawClipChar(screenX-4, screenY-8, ((i<8) ? 'W' : 'B'), (&(SCR_RECT){{0, 0, 159, 99}}), A_XOR); 
		}
	}
}

// draw all the mines active in the game
void drawMines()
{
	short screenX, screenY;
	
	char fuseStr[8];
	
	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<10; i++)
	{
		if(Mine_active & (int)1<<(i))
		{
			screenX=Mine_x[i];
			screenY=Mine_y[i];
			if(worldToScreen(&screenX, &screenY)==TRUE)
			{
				DrawClipChar(screenX-4, screenY-6, '*', (&(SCR_RECT){{0, 0, 159, 99}}), A_XOR);
				
				// if the mine has an active fuse, draw that too
				if(Mine_fuse[i]>0)
				{
					sprintf(fuseStr, "%d", Mine_fuse[i]);
					DrawStr(screenX-4, screenY-16, fuseStr, A_NORMAL);
				}// end if fuse
			}// end if on screen
		}// end if active
	}// next i
}

// draw all the oil drums active in the game
void drawOilDrums()
{
	short screenX, screenY;

	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<8; i++)
	{
		if(OilDrum_active & (int)1<<(i))
		{
			screenX=OilDrum_x[i];
			screenY=OilDrum_y[i];
			if(worldToScreen(&screenX, &screenY)==TRUE)
			{
				DrawStr(screenX-4, screenY-8, "[]", A_XOR);
			}// end if on screen
		}// end if active
	}// next i
	
}

// draw all the crates active in the game
void drawCrates()
{
	short screenX, screenY;
	
	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<8; i++)
	{
		if(Crate_active & (int)1<<(i))
		{
			screenX=Crate_x[i];
			screenY=Crate_y[i];
			if(worldToScreen(&screenX, &screenY)==TRUE)
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


// main drawing routine for the game, e.g. map, worms, weapons, etc
void Draw_renderGame()
{
	// clear screen
	ClrScr();
	
	// for some reason I can't put this in a method... the identical code copied from the method below
	drawMap();	
	
	// draw oil drums first, as everything else should overlap them
	drawOilDrums();
	
	// draw crates ontop of oil drums...
	drawCrates();
	
	// draw our wormy bros
	drawWorms();
	
	// mines are important, so draw them on top of everything else
	drawMines();
	
	// for now, we will output a bunch of debug info on the screen
	
	// game modes by name	
	char modes[9][16] = {"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// draw the current and previous game mode on the scren
	DrawStr(0,0,modes[(short)Game_mode], A_XOR);

	// draw the current grace time, turn time, and retreat time on the screen
	// NOTE: for some reason, drawing sudden death time instead of retreat time crashes the game)
	char timeStr[40];
	sprintf(timeStr, "time: %d, %d, %d", (short)(Game_graceTimer/TIME_MULTIPLIER), (short)(Game_timer/TIME_MULTIPLIER), (short)(Game_retreatTimer));
	DrawStr(0,10,timeStr, A_XOR);	

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
	
	// draw our free memory on the screen
	char heapStr[40];
	sprintf(heapStr, "heap: %lu", (unsigned long)HeapAvail());
	DrawStr(0,20, heapStr , A_XOR);
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
