/*
	MainMenu.c
	----------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MainMenu state machine specific code.
*/


const char *mainMenuText[4] = {
	"Start Match",
	"Team Settings",
	"Game Settings",
	"Credits"
};

char row = 0;
char col = 0;

/**
 * main drawing routine for the MainMenu menu
 */
void Draw_renderMainMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw help text based on menu item
	Draw_helpText(mainMenuText[(short)menuItem]);

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Worms 68k Party");

	// draw the x close & check accept buttoons
	Draw_XandCheck(BTN_CLOSE);

	// draw the big menu buttons
	Draw_bigMenuButton(40, 25, 0, menuItem, spr_MenuGame);
	Draw_bigMenuButton(88, 25, 1, menuItem, spr_MenuTeams);
	Draw_bigMenuButton(40, 55, 2, menuItem, spr_MenuSettingsAndHelp);
	Draw_bigMenuButton(88, 55, 3, menuItem, spr_MenuCredits);
		
	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to MainMenu mode.
*/
static void MainMenu_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// start with first menu item selected
	menuItem = 0;
}


/**
	Called every frame that the Games state machine is in MainMenu mode.
*/
static void MainMenu_update()
{
	Draw_renderMainMenu();

	// use arrow keys to move selection
	if(Keys_keyDown(keyLeft | keyRight))
	{
		col = (col==0) ? 1 : 0;
		menuItem = row * 2 + col;
		screenIsStale = STALE;
	}
	else if(Keys_keyDown(keyUp | keyDown))
	{
		row = (row==0) ? 1 : 0;
		menuItem = row * 2 + col;
		screenIsStale = STALE;
	}

	// F1 always exits game on this screen
	if(Keys_keyUp(keyF1|keyEscape))
		GameRunning=FALSE;

	// action will change game mode based on menu item
	if(Keys_keyUp(keyAction))
	{
		if(menuItem==0)
			State_changeMode(menuMode_MatchMenu);
		else if(menuItem==1)
			State_changeMode(menuMode_TeamSettings);
		else if(menuItem==2)
			State_changeMode(menuMode_HelpAndSettings);
		else
			State_changeMode(menuMode_Credits);
	}
}


/**
	Called on the first-frame when the Games state machine leaves MainMenu mode.
*/
static void MainMenu_exit()
{

}
