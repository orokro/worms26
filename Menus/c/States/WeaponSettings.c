/*
	WeaponSettings.c
	----------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the WeaponSettings state machine specific code.
*/


// local var now that we have a new app
char weaponSettings_menuItem = 0;

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
}


/**
	Called on the first-frame when the Games state machine leaves WeaponSettings mode.
*/
static void WeaponSettings_exit()
{

}
