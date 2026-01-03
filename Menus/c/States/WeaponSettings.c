/*
	WeaponSettings.c
	----------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the WeaponSettings state machine specific code.
*/


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
	FontSetSys(F_8x10);
	GrayDrawStr2B(1, 3, "Weapons Settings", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(0, 2, "Weapons Settings", A_XOR, lightPlane, darkPlane);

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

	// start with first menu item selected
	menuItem = 0;
}


/**
	Called every frame that the Games state machine is in WeaponSettings mode.
*/
static void WeaponSettings_update()
{
	Draw_renderWeaponSettingsMenu();
}


/**
	Called on the first-frame when the Games state machine leaves WeaponSettings mode.
*/
static void WeaponSettings_exit()
{

}
