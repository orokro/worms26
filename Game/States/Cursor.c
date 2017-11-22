/* ----------------------------------------------------------------------------------------
	 CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ 
   ---------------------------------------------------------------------------------------- */
/*
	For reference:
	
	Game_cursorX
	Game_cursorY
	Game_cursorDir
	Game_xMarkSpotX
	Game_xMarkSpotY
	Game_xMarkPlaced
	Game_xMarkAllowedOverLand
*/
char cursorFastMove=0;

/**
	Called on the first-frame when the Games state machine is set to Cursor mode.
*/
static void Cursor_enter()
{
	// when we enter this mode, always put the cursor over the current worm:
	Game_cursorX = Worm_x[(short)Worm_currentWorm];
	Game_cursorY = Worm_y[(short)Worm_currentWorm];
	
	// there doesn't exist an xMarkSpot yet..
	Game_xMarkPlaced = FALSE;
	
	// no time on the fast-move mode yet
	cursorFastMove=0;
	
	// we should set the camera to follow the cursor:
	Camera_focusOn(&Game_cursorX, &Game_cursorY);	
}

/**
	Called every frame that the Games state machine is in Cursor mode.
*/
static void Cursor_update()
{
	
	// All regular game-updates during this mode
	gameUpdates();
	
	// for each cursor key, move the cursor position, when its pressed
	if(Keys_keyDown(keyLeft)==TRUE)
		Game_cursorX--;
	else if(Keys_keyDown(keyRight)==TRUE)
		Game_cursorX++;
		
	if(Keys_keyDown(keyUp)==TRUE)
		Game_cursorY--;
	else if(Keys_keyDown(keyDown)==TRUE)
		Game_cursorY++;
		
	// as long as ANY of the arrow keys are down, increment our fast-move timer:
	if(Keys_keyState(keyCursors)==TRUE)
		cursorFastMove++;
	else
		cursorFastMove = 0;
		
	// if our weapons-fast move timer is over 30 frames, we will auto-move the cursor every 5 frames
	if(cursorFastMove>=30 && cursorFastMove%5==0)
	{
		if(Keys_keyState(keyLeft)==TRUE)
			Game_cursorX--;
		else if(Keys_keyState(keyRight)==TRUE)
			Game_cursorX++;
			
		if(Keys_keyState(keyUp)==TRUE)
			Game_cursorY--;
		else if(Keys_keyState(keyDown)==TRUE)
			Game_cursorY++;
	}
	
	// finally we bound-check our cursor position:
	if(Game_cursorX<0)
		Game_cursorX=0;
	else if(Game_cursorX>319)
		Game_cursorX=319;
		
	if(Game_cursorY<0)
		Game_cursorY=0;
	else if(Game_cursorY>190)
		Game_cursorY=190;
	
	// if the user pressed ESCAPE we should just exit cursor mode
	if(Keys_keyDown(keyEscape)==TRUE)
	{
		Game_changeMode(Game_previousMode);
		return;
	}
	
	// if the user pressed the action key, we should test if it's a valid point,
	// place the xMark spot and exit...
	if(Keys_keyDown(keyAction)==TRUE)
	{
		if(Game_xMarkAllowedOverLand==TRUE || Map_testPoint(Game_cursorX, Game_cursorY)==TRUE )
		{
			Game_xMarkSpotX = Game_cursorX;
			Game_xMarkSpotY = Game_cursorY;
			Game_xMarkPlaced = TRUE;
			
			// at this point we should do whatever weapon spawning junk we need to do..
			// TO-DO: implement
			Weapons_setTarget(Game_xMarkSpotX, Game_xMarkSpotY);
			
			// depending on the type of weaponing being used
			// this should either return to TURN mode, or goto TurnEND
			Game_changeMode((Game_cursorEndTurn==TRUE) ? gameMode_TurnEnd : gameMode_Turn );
			return;
		}
	}
	
	// the game
	Draw_renderGame();
}

/**
	Called on the first-frame when the Games state machine leaves Cursor mode.
*/
static void Cursor_exit()
{
	// clear the camera from focusing on our cursor
	Camera_clearFocus();
}
