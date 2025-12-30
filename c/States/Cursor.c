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

// 

/**
 * @brief Get the Girder Coords object
 * 
 * @param x - center x to rotate around
 * @param y - center y to rotate around
 * @param outX1 - output x1
 * @param outY1 - output y1
 * @param outX2 - output x2
 * @param outY2 - output y2
 */
void Cursor_getGirderCoords(short x, short y, short* outX1, short* outY1, short* outX2, short* outY2)
{
	const short xPos[] = {-5, -3, 0, -3, -8, -6, 0, -6};
	const short yPos[] = {0, -3, -5,  3, 0, -6, -8,  6};

	*outX1 = x + xPos[(short)Game_jetPackFuel];
	*outY1 = y + yPos[(short)Game_jetPackFuel];
	*outX2 = x - xPos[(short)Game_jetPackFuel];
	*outY2 = y - yPos[(short)Game_jetPackFuel];
}

/**
 * @brief Checks if the girder at the given position and rotation is within map bounds
 * 
 * @param cx Cursor X
 * @param cy Cursor Y
 * @param rotation Rotation index (0-7)
 * @return 1 if valid, 0 if OOB
 */
short checkGirderBounds(short cx, short cy, unsigned char rotation) {
	short gx1, gy1, gx2, gy2;
	
    // Temporary save/restore of rotation (Game_jetPackFuel) since getGirderCoords uses it globaly
    unsigned char oldRot = Game_jetPackFuel;
    Game_jetPackFuel = rotation;
    Cursor_getGirderCoords(cx, cy, &gx1, &gy1, &gx2, &gy2);
    Game_jetPackFuel = oldRot;

    // Check bounds (0-319 for X, 0-199 for Y)
    if (gx1 < 0 || gx1 >= 320 || gy1 < 0 || gy1 >= 200) return 0;
    if (gx2 < 0 || gx2 >= 320 || gy2 < 0 || gy2 >= 200) return 0;

    return 1;
}

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
	
	short moveX = 0;
	short moveY = 0;

	// for each cursor key, move the cursor position, when its pressed
	if(Keys_keyDown(keyLeft))
	{
		moveX -= 3;
		Game_currentWeaponState |= strikeLeft;
	}
	else if(Keys_keyDown(keyRight))
	{
		moveX += 3;
		Game_currentWeaponState &= ~strikeLeft;
	}	
	
	if(Keys_keyDown(keyUp))
		moveY -= 3;
	else if(Keys_keyDown(keyDown))
		moveY += 3;
		
	// as long as ANY of the arrow keys are down, increment our fast-move timer:
	if(Keys_keyState(keyCursors))
		cursorFastMove++;
	else
		cursorFastMove = 0;
		
	// if our weapons-fast move timer is over 30 frames, we will auto-move the cursor every 5 frames
	if(cursorFastMove>=5 && cursorFastMove%2==0)
	{
		if(Keys_keyState(keyLeft))
			moveX -= 3;
		else if(Keys_keyState(keyRight))
			moveX += 3;
			
		if(Keys_keyState(keyUp))
			moveY -= 3;
		else if(Keys_keyState(keyDown))
			moveY += 3;
	}
	
	// Apply movement if it's safe
	short nextX = Game_cursorX + moveX;
	short nextY = Game_cursorY + moveY;

    // Clamp basic cursor bounds
    if(nextX < 0) nextX = 0;
    else if(nextX > 319) nextX = 319;
    if(nextY < -50) nextY = -50;
    else if(nextY > 190) nextY = 190;

	// Special check for Girder Placement
    if(Game_stateFlags & gs_girderPlace) {
        if(checkGirderBounds(nextX, nextY, Game_jetPackFuel)) {
            Game_cursorX = nextX;
            Game_cursorY = nextY;
        } else {
			// If moving failed, try to clamp it to valid bounds? 
            // For now, just don't move if it puts us OOB.
			// Or we could try to just move on one axis.
			if(checkGirderBounds(Game_cursorX + moveX, Game_cursorY, Game_jetPackFuel))
				Game_cursorX += moveX;
			else if(checkGirderBounds(Game_cursorX, Game_cursorY + moveY, Game_jetPackFuel))
				Game_cursorY += moveY;
		}
    } else {
        Game_cursorX = nextX;
        Game_cursorY = nextY;
    }
	
	// for girder - inert if we are not in girder mode
	// if we pressed plus or minus, rotate the girder
    if(Game_stateFlags & gs_girderPlace) 
    {
        unsigned char nextRot = Game_jetPackFuel;
        char rotChanged = 0;

        if(Keys_keyDown(keyPlus)) {		
            nextRot = (Game_jetPackFuel+1)%8;
            rotChanged = 1;
        }
        else if(Keys_keyDown(keyMinus)) {
            nextRot = (Game_jetPackFuel+7)%8;
            rotChanged = 1;
        }

        if(rotChanged) {
            // Check if new rotation fits at current position
            if(checkGirderBounds(Game_cursorX, Game_cursorY, nextRot)) {
                Game_jetPackFuel = nextRot;
            } else {
                // If it doesn't fit, try to push the cursor back in bounds
				// Spiral search or simple axis checks? Simple for now.
				short pushX[] = {0, 0, -5, 5, -10, 10};
				short pushY[] = {0, -5, 5, -10, 10, 0};
				int i;
				for(i=0; i<6; i++) {
					if(checkGirderBounds(Game_cursorX + pushX[i], Game_cursorY + pushY[i], nextRot)) {
						Game_cursorX += pushX[i];
						Game_cursorY += pushY[i];
						Game_jetPackFuel = nextRot;
						break;
					}
				}
            }
        }
    }

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
            // Final safety check for girder
            if((Game_stateFlags & gs_girderPlace) && !checkGirderBounds(Game_cursorX, Game_cursorY, Game_jetPackFuel)) {
                // Play error sound? Or just refuse.
                return;
            }

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
