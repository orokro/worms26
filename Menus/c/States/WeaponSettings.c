/*
	WeaponSettings.c
	----------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the WeaponSettings state machine specific code.
*/


// local var now that we have a new app
char weaponSettings_menuItem = 0;

// the x/y position of the weapon-select cursor:
char weaponSelectX=0;
char weaponSelectY=0;

// all weapon names
static const char weaponNames[65][16] = {
	// row 1
	"Jetpack", 				// 0
	"Bazooka",				// 1
	"Grenade",				// 2
	"Shotgun",				// 3
	"Fire Punch",			// 4
	"Dynamite",				// 5
	"Air Strike",			// 6
	"Blow Torch",			// 7
	"Ninja Rope",			// 8
	"Sup. Banana Bomb",		// 9
	"Petrol Bomb",			// 10
	"Mad Cows",				// 11
	"Skip Go",				// 12
	
	// row 2
	"Low Gravity",			// 13
	"Homing Missile",		// 14
	"Cluster Bomb",			// 15
	"Handgun",				// 16
	"Dragon Ball",			// 17
	"Mine",					// 18
	"Napalm Strike",		// 19
	"Pneumatic Drill",		// 20
	"Bungee",				// 21
	"Holy Hand G'nade",		// 22
	"Skunk",				// 23
	"Old Woman",			// 24
	"Surrender",			// 25
	
	// row 3
	"Fast Walk",			// 26
	"Mortar",				// 27
	"Banana Bomb",			// 28
	"Uzi",					// 29
	"Kamikaze",				// 30
	"Sheep",				// 31
	"Mail Strike",			// 32
	"Girder",				// 33
	"Parachute",			// 34
	"Flame Thrower",		// 35
	"Ming Vase",			// 36
	"Concrete Donkey",		// 37
	"Select Worm",			// 38
	
	// row 4
	"Laser Sight",			// 39
	"Homing Pigeon",		// 40
	"Battle Axe",			// 41
	"Minigun",				// 42
	"Suicide Bomber",		// 43
	"Super Sheep",			// 44
	"Mine Strike",			// 45
	"Baseball Bat",			// 46
	"Teleport",				// 47
	"Salvation Army",		// 48
	"Sheep Strike",			// 49
	"Nuclear Test",			// 50
	"Freeze",				// 51
	
	// row 5
	"Invisibility",			// 52
	"Sheep Launcher",		// 53
	"Earthquake",			// 54
	"Longbow",				// 55
	"Prod",					// 56
	"Mole Bomb",			// 57
	"Mole Squadron",		// 58
	"Girder Pack",			// 59
	"ScalesOfJustice",		// 60
	"MB Bomb",				// 61
	"Carpet Bomb",			// 62
	"Armageddon",			// 63
	"Magic Bullet"			// 54
};


/**
 * main drawing routine for the WeaponSettings menu
 */
void Draw_renderWeaponSettingsMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Weapon Stock");

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// currently selected weapon
	short weapID = (weaponSelectX + (weaponSelectY * 13));

	// -----------------

	// loop to draw the grid of weapons
	// loop over our available weapons and draw their sprites, only on dark plane
	short x, y;
	for(x=0; x<13; x++)
	{
		for(y=0; y<5; y++)
		{
			// weapon index in this visual slot
			short weapID = (x + (y * 13));
			
			// if its a valid weapon, draw it's sprite
			if(Match_defaultWeapons[weapID]==0)
			{
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], lightPlane);
			}
			else 
			{
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], lightPlane);
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], darkPlane);
			}
			
		}// next y

	}// next x

	// draw the current cursor location
	ClipSprite16_XOR_R(2+(weaponSelectX*12), 14+(weaponSelectY*12), 11, spr_weaponSelect, lightPlane);
	ClipSprite16_XOR_R(2+(weaponSelectX*12), 14+(weaponSelectY*12), 11, spr_weaponSelect, darkPlane);

	// -----------------

	// prepare buffer with the name of the currently selected weapon
	char nameBuffer[50] = "";
	memcpy(nameBuffer, weaponNames[weapID], 15);

	// the amount of the weapon
	if(Match_defaultWeapons[weapID]<0)
		strcat(nameBuffer, " (     )");
	else
	{
		char countStr[8];
		sprintf(countStr, " (x%d)", Match_defaultWeapons[weapID]);
		strcat(nameBuffer, countStr);
	}
	
	// draw the name of the currently selected weapon and its stock count
	const short left = (160 - (strlen(nameBuffer) * 4)) / 2;
	GrayDrawStr2B(left, 78, nameBuffer, A_NORMAL, lightPlane, darkPlane);

	// if infinity, draw sprite
	if(Match_defaultWeapons[weapID]<0)
	{
		const char width = DrawStrWidth(nameBuffer, F_4x6)-11;
		ClipSprite8_OR_R(left + width, 78, 5, spr_Infinite, lightPlane);
		ClipSprite8_OR_R(left + width, 78, 5, spr_Infinite, darkPlane);
	}

	// draw instructions text
	GrayDrawStr2B(42, 89, "+/- to change stock", A_NORMAL, lightPlane, darkPlane);

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to WeaponSettings mode.
*/
static void WeaponSettings_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;
}


/**
	Called every frame that the Games state machine is in WeaponSettings mode.
*/
static void WeaponSettings_update()
{
	Draw_renderWeaponSettingsMenu();

	// this only has check, so F5 returns to MatchMenu
	if(Keys_keyUp(keyF5|keyEscape))
		State_changeMode(menuMode_MatchMenu);

	// use arrow keys to change the weapon select cursor
	if(Keys_keyDown(keyLeft))
	{
		if(weaponSelectX>0)
			weaponSelectX--;
	}
	else if(Keys_keyDown(keyRight))
	{
		if(weaponSelectX<12)
			weaponSelectX++;
	}
	else if(Keys_keyDown(keyUp))
	{
		if(weaponSelectY>0)
			weaponSelectY--;
	}
	else if(Keys_keyDown(keyDown))
	{
		if(weaponSelectY<4)
			weaponSelectY++;
	}

	// use plus and minus to adjust the stock of the selected weapon
	short weapID = (weaponSelectX + (weaponSelectY * 13));
	if(Keys_keyUp(keyPlus))
	{
		// increase stock, max 99 (infinity is -1)
		if(Match_defaultWeapons[weapID]<99)
		{
			if(Match_defaultWeapons[weapID]>=0)
				Match_defaultWeapons[weapID]++;
			else
				Match_defaultWeapons[weapID] = 1;
		}
	}
	else if(Keys_keyUp(keyMinus))
	{
		// decrease stock, min 0, then infinity
		if(Match_defaultWeapons[weapID]>0)
			Match_defaultWeapons[weapID]--;
		else if(Match_defaultWeapons[weapID]==0)
			Match_defaultWeapons[weapID] = -1;
	}

	// redraw for any keypress
	if(Keys_keyUp(keyAny))
		screenIsStale = STALE;
}


/**
	Called on the first-frame when the Games state machine leaves WeaponSettings mode.
*/
static void WeaponSettings_exit()
{

}
