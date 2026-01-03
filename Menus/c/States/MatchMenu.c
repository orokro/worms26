/*
	MatchMenu.c
	-----------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MatchMenu state machine specific code.
*/


const char *matchMenuText[5] = {
	"+/- To Adjust Map Type",
	"Adjust Weapons Stock",
	"Adjust Match Settings",
	"+/- To Change Team 1 Size",
	"+/- To Change Team 2 Size"
};


/**
 * main drawing routine for the MatchMenu menu
 */
void Draw_renderMatchMenuMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Game Setup");

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_CLOSE | BTN_ACCEPT);

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to MatchMenu mode.
*/
static void MatchMenu_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// start with first menu item selected
	menuItem = 0;
}


/**
	Called every frame that the Games state machine is in MatchMenu mode.
*/
static void MatchMenu_update()
{
	Draw_renderMatchMenuMenu();

	// F1 returns to main menu
	if(Keys_keyUp(keyF1|keyEscape))
		State_changeMode(menuMode_MainMenu);
	
	// F5 starts game (WormSelect for now)
	if(Keys_keyUp(keyF5))
		State_changeMode(menuMode_SaveAndExit);
}


/**
	Called on the first-frame when the Games state machine leaves MatchMenu mode.
*/
static void MatchMenu_exit()
{

}
