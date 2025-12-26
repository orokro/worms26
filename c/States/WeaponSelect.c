/*
	WeaponSelect.c
	--------------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the WeaponSelect state machine specific code.
*/


// the x/y position of the weapon-select cursor:
char weaponSelectX=0;
char weaponSelectY=0;
char weaponFastMove=0;


/**
 * main drawing routine for the weapons menu
 * @param wx - selected weapon x grid pos
 * @param wy - selected weapon y grid pos
 */
void Draw_renderWeaponsMenu(char wx, char wy)
{
	short x, y;
	
	// clear both our buffers
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	// draw title with shadow
	FontSetSys(F_8x10);
	GrayDrawStr2B(54, 2, "Weapons", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(53, 1, "Weapons", A_NORMAL, lightPlane, darkPlane);
	FontSetSys(F_6x8);
	
	// draw light and dark grid lines
	// for(x=1; x<14; x++)
	//		GrayFastDrawLine2B(0+(x*12), 13, 0+(x*12), 73, 1, lightPlane, darkPlane);
	//for(y=1; y<6; y++)
	//		GrayFastDrawHLine2B(1, 156, 12+(y*12), 1, lightPlane, darkPlane);
	
	// draw black grid lies
	// for(x=0; x<14; x++)
	// 	GrayFastDrawLine2B(1+(x*12), 13, 1+(x*12), 73, 3, lightPlane, darkPlane);
	// for(y=0; y<6; y++)
	// 	GrayFastDrawHLine2B(1, 156, 13+(y*12), 3, lightPlane, darkPlane);
	
	// draw drop shadow for weapons box (also to make it "centered" perfectly
	// GrayFastDrawHLine2B(2, 157, 74, 1, lightPlane, darkPlane);
	// GrayFastDrawLine2B(158, 14, 158, 74, 1, lightPlane, darkPlane);
	
	// get the weapon ID at this POS
	short weapID = Game_weapInventory[(short)wy][(short)wx];
	
	// if we have the weapon in our inventory...
	if(weapID!=-1)
	{
		// copy name to buffer with extra bit for null termination
		char nameBuffer[32] = "";
		memcpy(nameBuffer, weaponNames[weapID], 15);
		nameBuffer[16] = 0;
		
		// the amount of the weapon
		char countStr[8];
		sprintf(countStr, " (x%d)", Match_teamWeapons[(short)Game_currentTeam][weapID]);
		strcat(nameBuffer, countStr);
		
		// draw the name of the currently selected weapon and its stock count
		GrayDrawStr2B(1, 78, nameBuffer, A_NORMAL, lightPlane, darkPlane);
	}
	
	// loop over our available weapons and draw their sprites, only on dark plane
	for(x=0; x<13; x++)
	{
		for(y=0; y<5; y++)
		{
			// weapon index in this visual slot
			short weapID = Game_weapInventory[y][x];
			
			// if its a valid weapon, draw it's sprite
			if(weapID!=-1)
			{
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], lightPlane);
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], darkPlane);
			}
			else
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weaponSelect, lightPlane);

		}// next y
	}// next x
			
	// draw the current cursor location
	ClipSprite16_XOR_R(2+(wx*12), 14+(wy*12), 11, spr_weaponSelect, lightPlane);
	ClipSprite16_XOR_R(2+(wx*12), 14+(wy*12), 11, spr_weaponSelect, darkPlane);
	
	// draw instructions
	GrayDrawStr2B(19, 89, "[2nd] Select [ESC] Exit", A_NORMAL, lightPlane, lightPlane);
	
	// draw the current timer so the user can see time while picking a weapon
	Draw_timer();
}


/**
 * When a team brings up the weapons menu, we only want to show the items they have in stock.
 *
 * This method updates the Game_weapInventory[5][13] array such that it's vertical columns
 * reflect the weapons the user has available in that column.
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
	
	// start out in the top-left corner if our current weapon is -1
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
	
	// get the weapon ID at the current cursor position
	short weapID = Game_weapInventory[(short)weaponSelectY][(short)weaponSelectX];
		
	// if the user pressed the action key over a valid weapon, select that weapon and GTFO
	// note that we wait for key UP because we don't want the 2ND press to accidentally fire
	// the weapon when we return to the previous state...
	if(weapID!=-1 && Keys_keyUp(keyAction))
	{
		// set current weapon and properties
		Game_currentWeaponSelected = weapID;
		Game_currentWeaponProperties = Weapon_props[weapID];
		Game_currentWeaponState = 0;
		Game_changeMode(Game_previousMode);
		return;
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

