/*
	TeamSettings.c
	--------------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the TeamSettings state machine specific code.
*/


char tab = 0;

const unsigned char* graves[] = {
	spr_Grave,
	spr_Grave2,
	spr_Grave3
};


/**
 * main drawing routine for the TeamSettings menu
 * 
 * @param menuItem - which menu item to be selected
 */
void Draw_renderTeamSettingsMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	FontSetSys(F_8x10);
	GrayDrawStr2B(37, 3, "Team Config", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(36, 2, "Team Config", A_XOR, lightPlane, darkPlane);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);
	
	// draw the tabs along the bottom

	// black line across bottom
	GrayDrawClipLine2B(0, 88, 135, 88, 3, lightPlane, darkPlane);

	// skip the first row of of the tab sprite and draw it twice on the light plane
	ClipSprite32_OR_R(2, 89, 10, spr_TeamTab+1, lightPlane);
	ClipSprite32_OR_R(33, 89, 10, spr_TeamTab+1, lightPlane);

	// draw the selected tab on the dark plane
	short tabX = (short)(2 + (tab * 31));
	GrayClipSprite32_AND_R(tabX, 88, 11, spr_TeamTab, spr_TeamTab, lightPlane, darkPlane);
	GrayClipSprite32_OR_R(tabX, 88, 11, spr_TeamTab, spr_TeamTab, lightPlane, darkPlane);

	// tiny font draw tab labels
	FontSetSys(F_4x6);
	GrayDrawStr2B(5, 91, "Tm1 F1", A_NORMAL, darkPlane, darkPlane);
	GrayDrawStr2B(36, 91, "Tm2 F2", A_NORMAL, darkPlane, darkPlane);

	// -----------------
	
	// draw the text header labels for the boxes
	GrayDrawStr2B(5, 20, "Team Name:", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(5,  40, "Gravestone:", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(55,  20, "Team members:", A_NORMAL, lightPlane, darkPlane);

	// draw the text box rectangles
	// team name
	Draw_RectOutlineColor(5, 26, 47, 11, menuItem==0 ? 3 : 1);

	// square box for grave stone
	Draw_RectOutlineColor(17, 46, 18, 18, menuItem==1 ? 3 : 1);

	// draw text for minus and + on both sides of the gravestone box
	GrayDrawStr2B(11, 52, "-", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(36, 52, "+", A_NORMAL, lightPlane, darkPlane);

	// grid of 2x4 boxes for team members
	short i;
	for(i=0; i<8; i++)
	{
		short x = 55 + ((i%2) * 49);
		short y = 26 + ((i/2) * 10);
		Draw_RectOutlineColor(x, y, 50, 11, 1);
	}

	// draw selected box last in black
	short selectedBox = menuItem-2;
	if(selectedBox>=0 && selectedBox<8)
	{
		short x = 55 + ((selectedBox%2) * 49);
		short y = 26 + ((selectedBox/2) * 10);
		Draw_RectOutlineColor(x, y, 50, 11, 3);
	}

	// draw the current team name text
	GrayDrawStr2B(7, 29, Match_teamNames[(short)tab], A_NORMAL, lightPlane, darkPlane);

	// draw the team member names
	for(i=0; i<8; i++)
	{
		short x = 57 + ((i%2) * 49);
		short y = 29 + ((i/2) * 10);
		GrayDrawStr2B(x, y, Match_wormNames[(short)(tab*8)+i], A_NORMAL, lightPlane, darkPlane);
	}

	// draw the gravestone sprite in the box
	GrayClipSprite8_OR_R(22, 49, 12, graves[(short)Match_gravestones[(short)tab]], graves[(short)Match_gravestones[(short)tab]], lightPlane, darkPlane);

	// if we're over a text box (0, or 2-9), draw inverted box on both planes,
	// using strlen for either team name or worm name
	if(menuItem==0)
	{
		// team name box
		short nameLen = (short)strlen(Match_teamNames[(short)tab]);
		FastFilledRect_Invert_R(lightPlane, 7, 28, 8 + (nameLen * 4), 34);
		FastFilledRect_Invert_R(darkPlane, 7, 28, 8 + (nameLen * 4), 34);
		

	}
	else if(menuItem>=2 && menuItem<=9)
	{
		// worm name box
		short wormIndex = (short)(tab*8 + (menuItem-2));
		short nameLen = (short)strlen(Match_wormNames[wormIndex]);
		short x = 57 + (((menuItem-2)%2) * 49);
		short y = 28 + (((menuItem-2)/2) * 10);
		FastFilledRect_Invert_R(lightPlane, x, y, x + (nameLen * 4) + 3, y + 6);
		FastFilledRect_Invert_R(darkPlane, x, y, x + (nameLen * 4) + 3, y + 6);
	}

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to TeamSettings mode.
*/
static void TeamSettings_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// start with first menu item selected
	menuItem = 0;
}


/**
	Called every frame that the Games state machine is in TeamSettings mode.
*/
static void TeamSettings_update()
{
	Draw_renderTeamSettingsMenu();

	// this only has check, so F5 returns to MatchMenu
	if(Keys_keyUp(keyF5))
		Game_changeMode(gameMode_MainMenu);

	// increase / decrease menu item with left/right
	if(Keys_keyDown(keyLeft))
	{
		if(menuItem>0)
			menuItem--;
	}
	else if(Keys_keyDown(keyRight))
	{
		if(menuItem<9)
			menuItem++;
	}

	// f1 and f1 select team 0 and 1 respectively
	if(Keys_keyUp(keyF1))
		tab = 0;
	else if(Keys_keyUp(keyF2))
		tab = 1;

	// if plus or minus pressed while gravestone selected, change it
	if(menuItem==1)
	{
		if(Keys_keyUp(keyMinus) && Match_gravestones[(short)tab]>0)
			Match_gravestones[(short)tab]--;
		
		else if(Keys_keyUp(keyPlus) && Match_gravestones[(short)tab]<2)
			Match_gravestones[(short)tab]++;
	}
	
	// redraw for any keypress
	if(Keys_keyState(keyAny))
		screenIsStale = STALE;
}


/**
	Called on the first-frame when the Games state machine leaves TeamSettings mode.
*/
static void TeamSettings_exit()
{

}
