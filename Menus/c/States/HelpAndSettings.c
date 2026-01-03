/*
	HelpAndSettings.c
	-----------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the HelpAndSettings state machine specific code.
*/


/**
 * main drawing routine for the HelpAndSettings menu
 */
void Draw_renderHelpAndSettings()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Help & Settings");

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to HelpAndSettings mode.
*/
static void HelpAndSettings_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// start with first menu item selected
	menuItem = 0;
}


/**
	Called every frame that the Games state machine is in HelpAndSettings mode.
*/
static void HelpAndSettings_update()
{
	Draw_renderHelpAndSettings();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyAction|keyF1|keyF5|keyEscape))
		State_changeMode(menuMode_MainMenu);
}


/**
	Called on the first-frame when the Games state machine leaves HelpAndSettings mode.
*/
static void HelpAndSettings_exit()
{

}
