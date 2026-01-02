/*
	MatchSettings.c
	---------------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the MatchSettings state machine specific code.
*/


/**
 * main drawing routine for the MatchSettings menu
 * 
 * @param menuItem - which menu item to be selected
 */
void Draw_renderMatchSettingsMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	FontSetSys(F_8x10);
	GrayDrawStr2B(11, 3, "Match Settings", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(10, 2, "Match Settings", A_XOR, lightPlane, darkPlane);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// we're done drawing
	screenIsStale--;

}


/**
	Called on the first-frame when the Games state machine is set to MatchSettings mode.
*/
static void MatchSettings_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// start with first menu item selected
	menuItem = 0;
}


/**
	Called every frame that the Games state machine is in MatchSettings mode.
*/
static void MatchSettings_update()
{
	Draw_renderMatchSettingsMenu();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyF5))
		Game_changeMode(gameMode_MatchMenu);
}


/**
	Called on the first-frame when the Games state machine leaves MatchSettings mode.
*/
static void MatchSettings_exit()
{

}
