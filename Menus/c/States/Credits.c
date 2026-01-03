/*
	Credits.c
	---------
	
	This file is a snippet that is included raw in State.c
	
	This handles the Credits state machine specific code.
*/


/**
 * main drawing routine for the Credits menu
 */
void Draw_renderCreditsRoll()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	FontSetSys(F_8x10);
	GrayDrawStr2B(53, 3, "Credits", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(52, 2, "Credits", A_XOR, lightPlane, darkPlane);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to Credits mode.
*/
static void Credits_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;
}


/**
	Called every frame that the Games state machine is in Credits mode.
*/
static void Credits_update()
{
	Draw_renderCreditsRoll();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyAction|keyF1|keyF5|keyEscape))
		State_changeMode(menuMode_MainMenu);
}


/**
	Called on the first-frame when the Games state machine leaves Credits mode.
*/
static void Credits_exit()
{

}
