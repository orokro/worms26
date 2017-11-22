/* ----------------------------------------------------------------------------------------
	 PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE ++
   ---------------------------------------------------------------------------------------- */

// there will be TWO menu items for the pause menu: continue (0) and exit (1)
char pauseMenuItem=0;

/**
	Called on the first-frame when the Games state machine is set to Pause mode.
*/
static void Pause_enter()
{
	// always make sure its on continue when we enter
	pauseMenuItem=0;
}

/**
	Called every frame that the Games state machine is in Pause mode.
*/
static void Pause_update()
{
	/*
	  if either up or down are pressed, we should toggle the pause menu item
	  since there are only two menu items, there is no "direction", if the top item is selected
	  pressing up would loop it to the bottom item, and pressing down would move down to the bottom item
	  thus, we can test for either key at once by masking them together: keyUp | keyDown and then just
	  toggle the state of the variable
	*/
	if(Keys_keyDown(keyUp | keyDown)==TRUE)
		pauseMenuItem = ((pauseMenuItem==0) ? 1 : 0);
		
	// if the user presses the action key, we should either quit the game, or exit the pause menu
	if(Keys_keyDown(keyAction)==TRUE)
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
