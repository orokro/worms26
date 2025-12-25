/*
	Cursor.c
	--------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the Cursor state machine specific code.

	For reference:
	
	Game_cursorX
	Game_cursorY
	Game_cursorDir
	Game_xMarkSpotX
	Game_xMarkSpotY
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
	if(Keys_keyDown(keyLeft))
	{
		Game_cursorX -= 3;
		Game_currentWeaponState |= strikeLeft;
	}
	else if(Keys_keyDown(keyRight))
	{
		Game_cursorX += 3;
		Game_currentWeaponState &= ~strikeLeft;
	}	
	
	if(Keys_keyDown(keyUp))
		Game_cursorY -= 3;
	else if(Keys_keyDown(keyDown))
		Game_cursorY += 3;
		
	// as long as ANY of the arrow keys are down, increment our fast-move timer:
	if(Keys_keyState(keyCursors))
		cursorFastMove++;
	else
		cursorFastMove = 0;
		
	// if our weapons-fast move timer is over 30 frames, we will auto-move the cursor every 5 frames
	if(cursorFastMove>=5 && cursorFastMove%2==0)
	{
		if(Keys_keyState(keyLeft))
			Game_cursorX -= 3;
		else if(Keys_keyState(keyRight))
			Game_cursorX += 3;
			
		if(Keys_keyState(keyUp))
			Game_cursorY -= 3;
		else if(Keys_keyState(keyDown))
			Game_cursorY += 3;
	}
	
	// finally we bound-check our cursor position:
	if(Game_cursorX<0)
		Game_cursorX=0;
	else if(Game_cursorX>319)
		Game_cursorX=319;
		
	if(Game_cursorY<-50)
		Game_cursorY = -50;
	else if(Game_cursorY>190)
		Game_cursorY = 190;
	
	// if the user pressed ESCAPE we should just exit cursor mode
	if(Keys_keyUp(keyEscape))
	{
		Game_changeMode(Game_previousMode);
		return;
	}
	
	// if the user pressed the action key, we should test if it's a valid point,
	// place the xMark spot and exit...
	if(Keys_keyUp(keyAction))
	{
		if(Game_xMarkAllowedOverLand || (!Game_xMarkAllowedOverLand && !Map_testPoint(Game_cursorX, Game_cursorY)) )
		{
			Game_xMarkSpotX = Game_cursorX;
			Game_xMarkSpotY = Game_cursorY;
			Game_currentWeaponState |= targetPicked;
			
			// at this point we should do whatever weapon spawning junk we need to do..
			// TO-DO: implement
			Weapons_setTarget(Game_xMarkSpotX, Game_xMarkSpotY);
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
