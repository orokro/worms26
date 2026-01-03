/*
	HelpAndSettings.c
	-----------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the HelpAndSettings state machine specific code.
*/


// local var now that we have a new app
char help_menuItem = 0;

// define menu item ids
#define MENU_ITEM_DRAW_CLOUDS     0
#define MENU_ITEM_DRAW_LEAVES     1
#define MENU_ITEM_DRAW_MOUNTAINS  2


/**
 * @brief Draws a row with a label, arrow if selected & toggle
 * 
 * @param y 
 * @param label 
 * @param isEnabled 
 * @param isSelected 
 */
void drawToggleSetting(short y, const char* label, char isEnabled, char isSelected)
{
	// draw the label
	GrayDrawStr2B(30, y, label, A_NORMAL, lightPlane, darkPlane);

	// if we're selected, also draw an arrow
	if(isSelected)
		GrayDrawStr2B(20, y, ">", A_NORMAL, lightPlane, darkPlane);
	
	// draw the toggle
	Draw_toggle(120, y, isEnabled);
}


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

	// draw 3 options:
	drawToggleSetting(17, "Draw Clouds",   (Match_drawingFlags & DRAW_CLOUDS) ? 1 : 0, help_menuItem==MENU_ITEM_DRAW_CLOUDS);
	drawToggleSetting(27, "Draw Leaves",   (Match_drawingFlags & DRAW_LEAVES) ? 1 : 0, help_menuItem==MENU_ITEM_DRAW_LEAVES);
	drawToggleSetting(37, "Draw Mountains",(Match_drawingFlags & DRAW_MOUNTAINS) ? 1 : 0, help_menuItem==MENU_ITEM_DRAW_MOUNTAINS);

	// draw help text
	GrayDrawStr2B(80-15, 45, "Controls:", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 52, "[U/D/L/R] - Move Worm", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 59, "[Diamond] - Jump", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 66, "[Alpha] - Backflip", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 73, "[2nd] - Fire Weapon", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 80, "[APPS] - Select Worm", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 87, "[CATALOG] - Select Weapon", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(20, 94, "[Shift] - Control Camera", A_NORMAL, lightPlane, darkPlane);

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
	if(Keys_keyUp(keyF1|keyF5|keyEscape))
	{
		State_transitionButton = BTN_ACCEPT;
		State_changeMode(menuMode_MainMenu, 3);
	}		

	// up and down to change menu item
	if(Keys_keyDown(keyUp))
	{
		if(help_menuItem>0)
			help_menuItem--;
	}
	else if(Keys_keyDown(keyDown))
	{
		if(help_menuItem<2)
			help_menuItem++;
	}

	// if right is pressed, enable the setting
	if(Keys_keyUp(keyRight))
	{
		if(help_menuItem==MENU_ITEM_DRAW_CLOUDS)
			Match_drawingFlags |= DRAW_CLOUDS;
		else if(help_menuItem==MENU_ITEM_DRAW_LEAVES)
			Match_drawingFlags |= DRAW_LEAVES;
		else if(help_menuItem==MENU_ITEM_DRAW_MOUNTAINS)
			Match_drawingFlags |= DRAW_MOUNTAINS;
	}

	// if left is pressed, disable the setting
	if(Keys_keyUp(keyLeft))
	{
		if(help_menuItem==MENU_ITEM_DRAW_CLOUDS)
			Match_drawingFlags &= ~DRAW_CLOUDS;
		else if(help_menuItem==MENU_ITEM_DRAW_LEAVES)
			Match_drawingFlags &= ~DRAW_LEAVES;
		else if(help_menuItem==MENU_ITEM_DRAW_MOUNTAINS)
			Match_drawingFlags &= ~DRAW_MOUNTAINS;
	}

	// action button should just toggle the setting
	if(Keys_keyUp(keyAction))
	{
		if(help_menuItem==MENU_ITEM_DRAW_CLOUDS)
			Match_drawingFlags ^= DRAW_CLOUDS;
		else if(help_menuItem==MENU_ITEM_DRAW_LEAVES)
			Match_drawingFlags ^= DRAW_LEAVES;
		else if(help_menuItem==MENU_ITEM_DRAW_MOUNTAINS)
			Match_drawingFlags ^= DRAW_MOUNTAINS;
	}
	
	// redraw for any keypress
	if(Keys_keyState(keyAny))
		screenIsStale = STALE;
}


/**
	Called on the first-frame when the Games state machine leaves HelpAndSettings mode.
*/
static void HelpAndSettings_exit()
{

}
