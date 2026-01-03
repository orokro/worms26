/*
	MatchMenu.c
	-----------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MatchMenu state machine specific code.
*/

// help strings
const char *matchMenuText[5] = {
	"+/- To Adjust Map Type",
	"+/- To Change Team 1 Size",
	"+/- To Change Team 2 Size",
	"Adjust Weapons Stock",
	"Adjust Match Settings",	
};

// pointers to the various grave sprites we have
const unsigned long* groundTextures[] = {
	tex_Ground1,
	tex_Ground2,
	tex_Ground3
};

// local var now that we have a new app
char match_menuItem = 0;

// define menu item ids
#define MENU_ITEM_MAP_TYPE     0
#define MENU_ITEM_TEAM_1       1
#define MENU_ITEM_TEAM_2       2
#define MENU_ITEM_WEAPONS      3
#define MENU_ITEM_MATCH        4

// define positions for layout
#define MAP_LEFT 20
#define MAP_TOP 15
#define BIG_BUTTONS_TOP 60
#define WEAPONS_BUTTON_LEFT 80-32-5
#define MATCH_BUTTON_LEFT 85
#define TEAMS_LEFT 60
#define TEAM_1_TOP 15
#define TEAM_2_TOP 37


/**
 * @brief Draws a team name header label & a row of worms to imply team members count
 * 
 * @param x - x
 * @param y - y
 * @param teamIdx - team id
 * @param isSelected - true if selected
 */
void drawTeamRow(char x, char y, short teamIdx, char isSelected)
{
	// draw the team name
	char buffer[32];
	sprintf(buffer, "Team %s:", Match_teamNames[teamIdx]);
	GrayDrawStr2B((short)x, (short)y, buffer, A_NORMAL, lightPlane, darkPlane);
	
	// draw a rectangle to hold the worms & highlight if selected
	Draw_RectOutlineColor((short)x, (short)(y+10), 79, 5, isSelected ? 3 : 1);

	// loop to draw masked sprite for the number of worms on this team
	short i;
	for(i=0; i<Match_wormCount[teamIdx]; i++)
	{
		short wormX = x + 2 + (i * 8);
		GrayClipSprite16_MASK_R(wormX, (short)(y+6), 13, worm_IDLE_Outline, worm_IDLE_Outline, worm_IDLE_Mask, worm_IDLE_Mask, lightPlane, darkPlane);
	}
}


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

	// draw help text based on menu item
	Draw_helpText(86, matchMenuText[(short)match_menuItem]);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_CLOSE | BTN_ACCEPT);

	// -----------------

	// draw the map box label & current map texture
	GrayDrawStr2B(MAP_LEFT,  MAP_TOP, "Map Type:", A_NORMAL, lightPlane, darkPlane);
	Draw_RectOutlineColor(MAP_LEFT, MAP_TOP+6, 34, 34, match_menuItem==MENU_ITEM_MAP_TYPE ? 3 : 1);
	ClipSprite32_OR_R(MAP_LEFT+1, MAP_TOP+7, 32, groundTextures[(short)Match_mapType], lightPlane);
	FastFilledRect_Invert_R(lightPlane, MAP_LEFT+1, MAP_TOP+7, MAP_LEFT+1+31, MAP_TOP+7+31);	
	ClipSprite32_OR_R(MAP_LEFT+1, MAP_TOP+7, 32, groundTextures[(short)Match_mapType], darkPlane);

	// -----------------

	// Draw the team names & rows of worms
	drawTeamRow(TEAMS_LEFT, TEAM_1_TOP, 0, (match_menuItem==MENU_ITEM_TEAM_1));
	drawTeamRow(TEAMS_LEFT, TEAM_2_TOP, 1, (match_menuItem==MENU_ITEM_TEAM_2));

	// Draw the big menu buttons for sub menus
	Draw_bigMenuButton(WEAPONS_BUTTON_LEFT, BIG_BUTTONS_TOP, MENU_ITEM_WEAPONS, match_menuItem, spr_MenuWeapons);
	Draw_bigMenuButton(MATCH_BUTTON_LEFT, BIG_BUTTONS_TOP, MENU_ITEM_MATCH, match_menuItem, spr_MenuMatch);

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
}


/**
	Called every frame that the Games state machine is in MatchMenu mode.
*/
static void MatchMenu_update()
{
	Draw_renderMatchMenuMenu();

	// F1 returns to main menu
	if(Keys_keyUp(keyF1|keyEscape))
	{
		State_transitionButton = BTN_CLOSE;
		State_changeMode(menuMode_MainMenu, 3);
	}
	
	// F5 starts game (WormSelect for now)
	if(Keys_keyUp(keyF5))
	{
		State_transitionButton = BTN_ACCEPT;
		State_changeMode(menuMode_SaveAndExit, 3);
	}
		
	// left and right navigation
	if(Keys_keyDown(keyLeft))
	{
		if(match_menuItem==MENU_ITEM_TEAM_1 || match_menuItem==MENU_ITEM_TEAM_2)
			match_menuItem=MENU_ITEM_MAP_TYPE;
		else if(match_menuItem==MENU_ITEM_MATCH)
			match_menuItem=MENU_ITEM_WEAPONS;
	}
	else if(Keys_keyDown(keyRight))
	{
		if(match_menuItem==MENU_ITEM_MAP_TYPE)
			match_menuItem=MENU_ITEM_TEAM_1;
		else if(match_menuItem==MENU_ITEM_WEAPONS)
			match_menuItem=MENU_ITEM_MATCH;
	}

	// up and down navigation
	if(Keys_keyDown(keyUp))
	{	
		if(match_menuItem==MENU_ITEM_WEAPONS)
			match_menuItem=MENU_ITEM_MAP_TYPE;
		else if(match_menuItem==MENU_ITEM_TEAM_2)
			match_menuItem=MENU_ITEM_TEAM_1;
		else if(match_menuItem==MENU_ITEM_MATCH)
			match_menuItem=MENU_ITEM_TEAM_2;
	}
	else if(Keys_keyDown(keyDown))
	{
		if(match_menuItem==MENU_ITEM_MAP_TYPE)
			match_menuItem=MENU_ITEM_WEAPONS;
		else if(match_menuItem==MENU_ITEM_TEAM_1)
			match_menuItem=MENU_ITEM_TEAM_2;
		else if(match_menuItem==MENU_ITEM_TEAM_2)
			match_menuItem=MENU_ITEM_MATCH;
	}

	// if we're on map type, +/- changes it
	if(match_menuItem==MENU_ITEM_MAP_TYPE)
	{
		if(Keys_keyUp(keyMinus) && Match_mapType>0)
			Match_mapType--;
		
		else if(Keys_keyUp(keyPlus) && Match_mapType<2)
			Match_mapType++;
	}

	// if we're over team 1 or team 2, then +/- changes worm count
	if(match_menuItem==MENU_ITEM_TEAM_1)
	{
		if(Keys_keyUp(keyMinus) && Match_wormCount[0]>1)
			Match_wormCount[0]--;
		
		else if(Keys_keyUp(keyPlus) && Match_wormCount[0]<8)
			Match_wormCount[0]++;
	}
	else if(match_menuItem==MENU_ITEM_TEAM_2)
	{
		if(Keys_keyUp(keyMinus) && Match_wormCount[1]>1)
			Match_wormCount[1]--;
		
		else if(Keys_keyUp(keyPlus) && Match_wormCount[1]<8)
			Match_wormCount[1]++;
	}

	// if were over either big buttons, action enters that menu
	if(Keys_keyUp(keyAction))
	{
		if(match_menuItem==MENU_ITEM_WEAPONS)
			State_changeMode(menuMode_WeaponsSettings, 0);
		else if(match_menuItem==MENU_ITEM_MATCH)
			State_changeMode(menuMode_MatchSettings, 0);
	}

	// redraw for any keypress
	if(Keys_keyUp(keyAny))
		screenIsStale = STALE;
}


/**
	Called on the first-frame when the Games state machine leaves MatchMenu mode.
*/
static void MatchMenu_exit()
{

}
