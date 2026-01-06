/*
	MatchMenu.c
	-----------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MatchMenu state machine specific code.
*/

// help strings
const char *matchMenuText[5] = {
	"Adjust Weapons Stock",
	"Adjust Match Settings",
	"Adjust Map Settings",
	"+/- To Change Team 1 Size",
	"+/- To Change Team 2 Size",
};

// local var now that we have a new app
char match_menuItem = 0;

// define menu item ids
#define MENU_ITEM_WEAPONS      	0
#define MENU_ITEM_MATCH        	1
#define MENU_ITEM_MAP     		2
#define MENU_ITEM_TEAM_1       	3
#define MENU_ITEM_TEAM_2       	4

// define positions for layout
#define BIG_BUTTONS_TOP 16
#define WEAPONS_BUTTON_LEFT 22
#define MATCH_BUTTON_LEFT 22+42
#define MAP_BUTTON_LEFT 22+42+42
#define TEAMS_LEFT 40
#define TEAM_1_TOP 41
#define TEAM_2_TOP 62


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

	// Draw the big menu buttons for sub menus
	Draw_bigMenuButton(WEAPONS_BUTTON_LEFT, BIG_BUTTONS_TOP, MENU_ITEM_WEAPONS, match_menuItem, spr_MenuWeapons);
	Draw_bigMenuButton(MATCH_BUTTON_LEFT, BIG_BUTTONS_TOP, MENU_ITEM_MATCH, match_menuItem, spr_MenuMatch);
	Draw_bigMenuButton(MAP_BUTTON_LEFT, BIG_BUTTONS_TOP, MENU_ITEM_MAP, match_menuItem, spr_MenuMap);

	// Draw the team names & rows of worms
	drawTeamRow(TEAMS_LEFT, TEAM_1_TOP, 0, (match_menuItem==MENU_ITEM_TEAM_1));
	drawTeamRow(TEAMS_LEFT, TEAM_2_TOP, 1, (match_menuItem==MENU_ITEM_TEAM_2));

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
		App_exitRequested = FALSE;
		State_changeMode(menuMode_SaveAndExit, 3);
	}
		
	// if match_menuItem < 3, left/right just clamps between 0 and 2
	if(match_menuItem<3)
	{
		if(Keys_keyDown(keyLeft) && match_menuItem>0)
			match_menuItem--;
		else if(Keys_keyDown(keyRight) && match_menuItem<2)
			match_menuItem++;
	}

	// if match_menuItem <3 down goes to 3, otherwise down goes to 4 if on 3
	if(Keys_keyDown(keyDown))
	{
		if(match_menuItem<3)
			match_menuItem = 3;
		else if(match_menuItem==3)
			match_menuItem = 4;
	}
	else if(Keys_keyDown(keyUp))
	{
		if(match_menuItem==4)
			match_menuItem = 3;
		else if(match_menuItem==3)
			match_menuItem = 1;
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
		else if(match_menuItem==MENU_ITEM_MAP)
			State_changeMode(menuMode_MapSettings, 0);
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
