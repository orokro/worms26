/*
	TitleScreen.c
	-------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the TitleScreen state machine specific code.
*/


/**
 * main drawing routine for the TitleScreen menu
 */
void Draw_renderTitleScreen()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	FontSetSys(F_8x10);
	GrayDrawStr2B(18, 3, "Worms 68K Party", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(17, 2, "Worms 68K Party", A_XOR, lightPlane, darkPlane);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to TitleScreen mode.
*/
static void TitleScreen_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;
}


/**
	Called every frame that the Games state machine is in TitleScreen mode.
*/
static void TitleScreen_update()
{
	Draw_renderTitleScreen();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyAction|keyF1|keyF5|keyEscape))
		State_changeMode(menuMode_MainMenu);
}


/**
	Called on the first-frame when the Games state machine leaves TitleScreen mode.
*/
static void TitleScreen_exit()
{

}
