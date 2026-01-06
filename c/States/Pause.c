/*
	Pause.c
	-------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the Pause state machine specific code.
*/


// there will be TWO menu items for the pause menu: continue (0) and exit (1)
char pauseMenuItem=0;


/**
 * @brief Draws a stack of worms for the pause menu worm counter
 * 
 * @param yPos - y position to draw at
 * @param teamIndex - which team to draw worms for (0 or 1)
 */
void Draw_pauseMenuWormCounter(short yPos, short teamIndex)
{
	short i, x;
	
	// draw the team name on the left
	x = 80 - 6*strlen(Match_teamNames[teamIndex]);
	GrayDrawStr2B(x, yPos, Match_teamNames[teamIndex], A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(x, yPos, Match_teamNames[teamIndex], A_XOR, lightPlane, darkPlane);

	// build a mask for this team's worms
	const unsigned short teamMask = (teamIndex==0) ? 0b0000000011111111 : 0b1111111100000000;

	// get how many worms are alive for this team
	unsigned short liveWorms = ~Worm_isDead & teamMask & Worm_active;

	// count how many bits are set in liveWorms
	short liveCount = 0;
	for(i=0; i<MAX_WORMS; i++)
		if((liveWorms & (1<<i))!=0)
			liveCount++;
	
	// loop too draw worm sprites for each teams remaining worms from right to left
	x = 78 + liveCount * 9;
	yPos -= 3;
	for(i=0; i<liveCount; i++)
	{
		x-=9;
		ClipSprite16_AND_R(x, yPos, 13, wormsSprites[WORM_IDLE_MASK], darkPlane);
		ClipSprite16_AND_R(x, yPos, 13, wormsSprites[WORM_IDLE_MASK], lightPlane);
		ClipSprite16_OR_R(x, yPos, 13, wormsSprites[WORM_IDLE_LIGHT], darkPlane);
		ClipSprite16_OR_R(x, yPos, 13, wormsSprites[WORM_IDLE_LIGHT], lightPlane);
	}// next i
}


/**
 * main drawing routine for the pause menu
 * @param menuItem - which menu item to be selected
 */
void Draw_renderPauseMenu(char menuItem)
{
	// draw the game as normal..
	Draw_renderGame();
	
	// make it darker
	FastFillRect(darkPlane, 0, 0, 159, 99, A_NORMAL);
	
	// draw hud normally
	Draw_HUD();

	// menu item text
	char continueStr[] = " Continue ";
	char exitStr[] = " Exit ";
	
	// if selected, add markers
	if(menuItem==0)
	{
		continueStr[0] = 26;
		continueStr[9] = 25;
	}else
	{
		exitStr[0] = 26;
		exitStr[5] = 25;
	}
	
	// draw menu options
	GrayDrawStr2B(50, 20, continueStr, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(50, 20, continueStr, A_XOR, lightPlane, darkPlane);
	GrayDrawStr2B(62, 30, exitStr, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(62, 30, exitStr, A_XOR, lightPlane, darkPlane);
	
	// draw title
	FontSetSys(F_8x10);

	// dark plane is solid, so we can XOR the shadow area to make it WHITE on the dark plane
	GrayDrawStr2B(57, 2, "Paused", A_XOR, darkPlane, darkPlane);
	
	// draw shadow JUST on light plane
	GrayDrawStr2B(57, 2, "Paused", A_NORMAL, lightPlane, lightPlane);
	
	// draw it to both planes in black, then XOR it to make it WHITE
	GrayDrawStr2B(56, 1, "Paused", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(56, 1, "Paused", A_XOR, lightPlane, darkPlane);
	
	/*
		NOTE:
		
		I was gonna draw control info below the options, but it runs too slow.
		
		Perhaps I'll use sprites one day if there's space.
	*/
	FontSetSys(F_6x8);
	Draw_pauseMenuWormCounter(50, 0);
	Draw_pauseMenuWormCounter(70, 1);

}


/**
	Called on the first-frame when the Games state machine is set to Pause mode.
*/
static void Pause_enter()
{
	// always make sure its on continue when we enter
	pauseMenuItem=0;
	Game_waterLevel+=10;
}


/**
	Called every frame that the Games state machine is in Pause mode.
*/
static void Pause_update()
{
	// if escape is pressed again, just exit the Pause Menu
	if(Keys_keyDown(keyEscape))
	{
		Game_changeMode(Game_previousMode);
		return;
	}
		
	// if shift is pressed, camera is being moved, ignore input
	if(!Keys_keyState(keyCameraControl))
	{		
		/*
		  if either up or down are pressed, we should toggle the pause menu item
		  since there are only two menu items, there is no "direction", if the top item is selected
		  pressing up would loop it to the bottom item, and pressing down would move down to the bottom item
		  thus, we can test for either key at once by masking them together: keyUp | keyDown and then just
		  toggle the state of the variable
		*/
		if(Keys_keyDown(keyUp | keyDown))
			pauseMenuItem = ((pauseMenuItem==0) ? 1 : 0);
			
		// if the user presses the action key, we should either quit the game, or exit the pause menu
		if(Keys_keyDown(keyAction))
		{
			if(pauseMenuItem==0)
			{
				Game_changeMode(Game_previousMode);
				return;
			}else
			{
				// setting this will exit the next tick of our main loop
				GameRunning=FALSE;
				return;
			}
		}	
	}// end if moving camera
	
	// note: there are no physics, worms, time updates, etc during pause!
	
	// draw the pause menu!
	Draw_renderPauseMenu(pauseMenuItem);

}


/**
	Called on the first-frame when the Games state machine leaves Pause mode.
*/
static void Pause_exit()
{

}
