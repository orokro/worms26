/* ----------------------------------------------------------------------------------------
	 WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT ++
   ---------------------------------------------------------------------------------------- */

// the x/y position of the weapon-select cursor:
char weaponSelectX=0;
char weaponSelectY=0;
char weaponFastMove=0;

/**
 * When a team brings up the weapons menu, we only want to show the items they have in stock.
 *
 * This method updates the Game_weapInventory[5][13] array such that it's vertical columns
 * reflect the weapons the user has available in that colum.
*/
void calcWeapInventory()
{
	// loop over the current teams inventory and fill out our Game_weapInventory array
	short row, col;
	for(col=0; col<13; col++)
	{
		// as we loop over the row, we will only update the slot to fill, when
		// a team has that weapon in stock
		short stockRow=0;
		
		// loop over every row in this column
		for(row=0; row<5; row++)
		{
			// is there stock?
			if(Match_teamWeapons[(short)Game_currentTeam][(row*13)+col]>0)
			{
				// store the ID of this weapon in our current inventory row/col
				// note: we can also use flat ID's!
				Game_weapInventory[stockRow][col] = ((row*13)+col);
				
				// next stock row
				stockRow++;
			}
		}// next row	
		
		// if stockRow is 5 that means the user had every weapon in this row
		// but if its less than 5, we need to fill the rest of the slots with -1 (code for no weapon ID)
		for(row=stockRow; row<5; row++)
			Game_weapInventory[row][col] = -1;
			
	}// next col	
	
}



/**
 * Called on the first-frame when the Games state machine is set to WeaponSelect mode.
*/
static void WeaponSelect_enter()
{
	// make sure our fast-move timer starts at 0
	weaponFastMove = 0;
	
	// we need to build a matrix of the AVAILABLE weapons this user has access to...
	calcWeapInventory();
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
	if(weaponFastMove>=10 && weaponFastMove%2==0)
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
		weaponSelectX=12;
	else if(weaponSelectX>12)
		weaponSelectX=0;
		
	if(weaponSelectY<0)
		weaponSelectY=4;
	else if(weaponSelectY>4)
		weaponSelectY=0;
	
	// if the user pressed ESCAPE or the weapons menu key again we should just exit the weapons menu
	if(Keys_keyDown(keyEscape) || Keys_keyDown(keyWeaponsSelect))
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
