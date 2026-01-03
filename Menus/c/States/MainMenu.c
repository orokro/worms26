/*
	MainMenu.c
	----------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MainMenu state machine specific code.
*/

// help strings
const char *mainMenuText[4] = {
	"Start Match",
	"Team Settings",
	"Game Settings",
	"Credits"
};

// for selecting buttons
char row = 0;
char col = 0;

// local var now that we have a new app
char main_menuItem = 0;

// define menu item ids
#define MENU_ITEM_PLAY_GAME        0
#define MENU_ITEM_TEAM_SETTINGS    1
#define MENU_ITEM_GAME_SETTINGS    2
#define MENU_ITEM_CREDITS          3

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

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Worms 68k Party");

	// draw help text based on menu item
	Draw_helpText(86, mainMenuText[(short)main_menuItem]);

	// draw the x close & check accept buttoons
	Draw_XandCheck(BTN_CLOSE);

	// draw the big menu buttons
	Draw_bigMenuButton(40, 25, MENU_ITEM_PLAY_GAME, main_menuItem, spr_MenuGame);
	Draw_bigMenuButton(88, 25, MENU_ITEM_TEAM_SETTINGS, main_menuItem, spr_MenuTeams);
	Draw_bigMenuButton(40, 55, MENU_ITEM_GAME_SETTINGS, main_menuItem, spr_MenuSettingsAndHelp);
	Draw_bigMenuButton(88, 55, MENU_ITEM_CREDITS, main_menuItem, spr_MenuCredits);
		
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
		main_menuItem = row * 2 + col;
		screenIsStale = STALE;
	}
	else if(Keys_keyDown(keyUp | keyDown))
	{
		row = (row==0) ? 1 : 0;
		main_menuItem = row * 2 + col;
		screenIsStale = STALE;
	}

	// F1 always exits game on this screen
	if(Keys_keyUp(keyF1|keyEscape))
		GameRunning=FALSE;

	// action will change game mode based on menu item
	if(Keys_keyUp(keyAction))
	{
		if(main_menuItem==MENU_ITEM_PLAY_GAME)
			State_changeMode(menuMode_MatchMenu);
		else if(main_menuItem==MENU_ITEM_TEAM_SETTINGS)
			State_changeMode(menuMode_TeamSettings);
		else if(main_menuItem==MENU_ITEM_GAME_SETTINGS)
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
