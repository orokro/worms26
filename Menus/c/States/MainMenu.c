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
	GrayClipSprite32_OR_R(40, 30, 21, spr_MenuGame, spr_MenuGame, lightPlane, darkPlane);
	if(menuItem==0)
	{
		ClipSprite32_OR_R(40, 31, 21, spr_MenuGame, lightPlane);
		ClipSprite32_OR_R(41, 31, 21, spr_MenuGame, lightPlane);
	}

	GrayClipSprite32_OR_R(88, 30, 21, spr_MenuTeams, spr_MenuTeams, lightPlane, darkPlane);
	if(menuItem==1)
	{
		ClipSprite32_OR_R(88, 31, 21, spr_MenuTeams, lightPlane);
		ClipSprite32_OR_R(89, 31, 21, spr_MenuTeams, lightPlane);
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

	// either key will toggle menu item
	if(Keys_keyDown(keyLeft | keyRight))
	{
		menuItem = (menuItem==0) ? 1 : 0;
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
		else
			State_changeMode(menuMode_TeamSettings);
	}
}


/**
	Called on the first-frame when the Games state machine leaves MainMenu mode.
*/
static void MainMenu_exit()
{

}
