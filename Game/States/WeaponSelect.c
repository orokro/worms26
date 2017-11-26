/* ----------------------------------------------------------------------------------------
	 WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT ++
   ---------------------------------------------------------------------------------------- */

// the x/y position of the weapon-select cursor:
char weaponSelectX=0;
char weaponSelectY=0;
char weaponFastMove=0;

/**
	Called on the first-frame when the Games state machine is set to WeaponSelect mode.
*/
static void WeaponSelect_enter()
{
	// make sure our fast-move timer starts at 0
	weaponFastMove = 0;
}

/**
	Called every frame that the Games state machine is in WeaponSelect mode.
*/
static void WeaponSelect_update()
{
	// for each cursor key, move the weapon selection position, when its pressed
	if(Keys_keyDown(keyLeft))
		weaponSelectX--;
	else if(Keys_keyDown(keyRight))
		weaponSelectX++;
		
	if(Keys_keyDown(keyUp))
		weaponSelectY--;
	else if(Keys_keyDown(keyDown))
		weaponSelectY++;
		
	// as long as ANY of the arrow keys are down, increment our fast-move timer:
	if(Keys_keyState(keyCursors))
		weaponFastMove++;
	else
		weaponFastMove = 0;
		
	// if our weapons-fast move timer is over 15 frames, we will auto-move the cursor every 3 frames
	if(weaponFastMove>=15 && weaponFastMove%3==0)
	{
		if(Keys_keyState(keyLeft))
			weaponSelectX--;
		else if(Keys_keyState(keyRight))
			weaponSelectX++;
			
		if(Keys_keyState(keyUp))
			weaponSelectY--;
		else if(Keys_keyState(keyDown))
			weaponSelectY++;
	}
	
	// finally we bound-check our weapon positions:
	if(weaponSelectX<0)
		weaponSelectX=13;
	else if(weaponSelectX>13)
		weaponSelectX=0;
		
	if(weaponSelectY<0)
		weaponSelectY=4;
	else if(weaponSelectY>4)
		weaponSelectY=0;
	
	// if the user pressed ESCAPE we should just exit the weapons menu
	if(Keys_keyDown(keyEscape))
	{
		Game_changeMode(Game_previousMode);
		return;
	}
	
	// if the user pressed the action key, make sure they have stock of that weapon,
	// and if so, select it and exit:
	if(Keys_keyDown(keyAction))
	{
		// TO-DO: implement
	}
	
	// All regular game-updates during this mode
	gameUpdates();
	
	// draw the weapons menu!
	Draw_renderWeaponsMenu(weaponSelectX, weaponSelectY);	
}

/**
	Called on the first-frame when the Games state machine leaves WeaponSelect mode.
*/
static void WeaponSelect_exit()
{
	
}
