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
	FontSetSys(F_8x10);
	GrayDrawStr2B(18, 3, "Worms 68k Party", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(17, 2, "Worms 68k Party", A_XOR, lightPlane, darkPlane);

	// draw the x close & check accept buttoons
	Draw_XandCheck(BTN_CLOSE);

	// draw menu item sprites, with a shadow for the selected one
	GrayClipSprite32_OR_R(40, 25, 21, spr_MenuGame, spr_MenuGame, lightPlane, darkPlane);
	if(menuItem==0)
	{
		ClipSprite32_OR_R(40, 26, 21, spr_MenuGame, lightPlane);
		ClipSprite32_OR_R(41, 26, 21, spr_MenuGame, lightPlane);
	}

	GrayClipSprite32_OR_R(88, 25, 21, spr_MenuTeams, spr_MenuTeams, lightPlane, darkPlane);
	if(menuItem==1)
	{
		ClipSprite32_OR_R(88, 26, 21, spr_MenuTeams, lightPlane);
		ClipSprite32_OR_R(89, 26, 21, spr_MenuTeams, lightPlane);
	}
		
	GrayClipSprite32_OR_R(40, 55, 21, spr_MenuSettingsAndHelp, spr_MenuSettingsAndHelp, lightPlane, darkPlane);
	if(menuItem==2)
	{
		ClipSprite32_OR_R(40, 56, 21, spr_MenuSettingsAndHelp, lightPlane);
		ClipSprite32_OR_R(41, 56, 21, spr_MenuSettingsAndHelp, lightPlane);
	}

	GrayClipSprite32_OR_R(88, 55, 21, spr_MenuCredits, spr_MenuCredits, lightPlane, darkPlane);
	if(menuItem==3)
	{
		ClipSprite32_OR_R(88, 56, 21, spr_MenuCredits, lightPlane);
		ClipSprite32_OR_R(89, 56, 21, spr_MenuCredits, lightPlane);
	}
		
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
