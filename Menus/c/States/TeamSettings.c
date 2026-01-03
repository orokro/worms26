/*
	TeamSettings.c
	--------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the TeamSettings state machine specific code.
*/

// help strings
const char *teamMenuText[5] = {
	"Edit Team Name",
	"+/- To Change Grave Stone",
	"Edit Worm Name",	
};

// which team tab is currently focused
char tab = 0;

// pointers to the various grave sprites we have
const unsigned char* graves[] = {
	spr_Grave,
	spr_Grave2,
	spr_Grave3
};

// local var now that we have a new app
char teamSettings_menuItem = 0;

// define menu item ids
#define MENU_ITEM_TEAM_NAME        0
#define MENU_ITEM_GRAVESTONE       1
#define MENU_ITEM_TEAM_MEMBER_1    2
#define MENU_ITEM_TEAM_MEMBER_2    3
#define MENU_ITEM_TEAM_MEMBER_3    4
#define MENU_ITEM_TEAM_MEMBER_4    5
#define MENU_ITEM_TEAM_MEMBER_5    6
#define MENU_ITEM_TEAM_MEMBER_6    7
#define MENU_ITEM_TEAM_MEMBER_7    8
#define MENU_ITEM_TEAM_MEMBER_8    9

// editing state
char teamSettings_isEditing = 0;
char teamSettings_lastAlpha = 0;
char teamSettings_lastBackspace = 0;



/**
 * Stop editing text and fill in default if empty
 */
void stopTextEdit(char* buffer)
{
	teamSettings_isEditing = 0;

	if (buffer && buffer[0] == '\0') {

		strcpy(buffer, "UNNAMED");
		screenIsStale = STALE;
	}
}


/**
 * Handles text input for the given string buffer
 */
void handleTextEdit(char* buffer, short maxLen)
{
    // Check keys
    char currentAlpha = Keys_getAlphaChar();
    char backspaceDown = Keys_keyDown(keyBackspace);
    char clearDown = Keys_keyDown(keyClear);

    // 1. Check for entering edit mode
    if (!teamSettings_isEditing) {

        if (currentAlpha || backspaceDown || clearDown) {

            // Enter edit mode
            teamSettings_isEditing = 1;

            // Always start fresh
            buffer[0] = '\0';
      
            // If it was an alpha key, add it
            if (currentAlpha) {
                buffer[0] = currentAlpha;
                buffer[1] = '\0';
            }
            screenIsStale = STALE;

            // Update latches
            teamSettings_lastAlpha = currentAlpha;

            return;
        }
    }

    // Handle Alpha Input (while editing)
    if (currentAlpha)
	{
        if (currentAlpha != teamSettings_lastAlpha)
		{
            // Append char
            short len = (short)strlen(buffer);

            if (len < maxLen) {

                buffer[len] = currentAlpha;
                buffer[len+1] = '\0';
                screenIsStale = STALE;
            }
        }
    }

    teamSettings_lastAlpha = currentAlpha;

    // Handle Backspace (while editing)
    if (backspaceDown)
	{
        short len = (short)strlen(buffer);

        if (len > 0) 
		{
            buffer[len - 1] = '\0';
            screenIsStale = STALE;
        }
    }

	// 4. Handle Clear (while editing)
	if (clearDown) {
		buffer[0] = '\0';
		screenIsStale = STALE;
	}
}


/**
 * main drawing routine for the TeamSettings menu
 */
void Draw_renderTeamSettingsMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Team Settings");

	// draw help text based on menu item
	Draw_helpText(76, teamSettings_menuItem<2 ? teamMenuText[(short)teamSettings_menuItem] : teamMenuText[2]);

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
	Draw_textBox(5, 26, 47, teamSettings_menuItem==MENU_ITEM_TEAM_NAME, Match_teamNames[(short)tab]);

	// square box for grave stone with sprite inside
	Draw_RectOutlineColor(17, 46, 18, 18, teamSettings_menuItem==MENU_ITEM_GRAVESTONE ? 3 : 1);
	GrayClipSprite8_OR_R(22, 49, 12, graves[(short)Match_gravestones[(short)tab]], graves[(short)Match_gravestones[(short)tab]], lightPlane, darkPlane);

	// draw text for minus and + on both sides of the gravestone box
	GrayDrawStr2B(11, 52, "-", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(36, 52, "+", A_NORMAL, lightPlane, darkPlane);

	// grid of 2x4 boxes for team members
	short i;
	for(i=0; i<8; i++)
	{
		char x = 55 + ((i%2) * 49);
		char y = 26 + ((i/2) * 10);
		Draw_textBox(x, y, 50, (teamSettings_menuItem-2)==i, Match_wormNames[(short)(tab*8)+i]);

		Draw_RectOutlineColor(x, y, 50, 11, (teamSettings_menuItem-2)==i ? 3 : 1);
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

	teamSettings_menuItem = 0;
	teamSettings_isEditing = 0;
	teamSettings_lastAlpha = 0;
}


/**
	Called every frame that the Games state machine is in TeamSettings mode.
*/
static void TeamSettings_update()
{
	Draw_renderTeamSettingsMenu();

	// Text Editing Logic
	char* editBuffer = NULL;
	short editMaxLen = 0;

	if (teamSettings_menuItem == MENU_ITEM_TEAM_NAME) {
		editBuffer = Match_teamNames[(short)tab];
		editMaxLen = 8;
	} else if (teamSettings_menuItem >= MENU_ITEM_TEAM_MEMBER_1 && teamSettings_menuItem <= MENU_ITEM_TEAM_MEMBER_8) {
		editBuffer = Match_wormNames[(short)(tab * 8) + (teamSettings_menuItem - MENU_ITEM_TEAM_MEMBER_1)];
		editMaxLen = 11;
	}

	if (editBuffer) {
		handleTextEdit(editBuffer, editMaxLen);
	}

	// this only has check, so F5 returns to MatchMenu
	if(Keys_keyUp(keyF5|keyEscape))
	{
		stopTextEdit(editBuffer);
		State_transitionButton = BTN_ACCEPT;
		State_changeMode(menuMode_MainMenu, 3);
	}
		
	// increase / decrease menu item with left/right
	if(Keys_keyDown(keyLeft))
	{
		stopTextEdit(editBuffer);
		switch (teamSettings_menuItem)
		{
			case MENU_ITEM_TEAM_MEMBER_2:
			case MENU_ITEM_TEAM_MEMBER_4:
			case MENU_ITEM_TEAM_MEMBER_6:
			case MENU_ITEM_TEAM_MEMBER_8:
				teamSettings_menuItem--;
				break;
			case MENU_ITEM_TEAM_MEMBER_1:
				teamSettings_menuItem = MENU_ITEM_TEAM_NAME;
				break;
			case MENU_ITEM_TEAM_MEMBER_3:
			case MENU_ITEM_TEAM_MEMBER_5:
			case MENU_ITEM_TEAM_MEMBER_7:
				teamSettings_menuItem = MENU_ITEM_GRAVESTONE;
				break;
		}

	}
	else if(Keys_keyDown(keyRight))
	{
		stopTextEdit(editBuffer);
		switch(teamSettings_menuItem)
		{
			case MENU_ITEM_TEAM_NAME:
				teamSettings_menuItem = MENU_ITEM_TEAM_MEMBER_1;
				break;
			case MENU_ITEM_GRAVESTONE:
				teamSettings_menuItem = MENU_ITEM_TEAM_MEMBER_3;
				break;
			case MENU_ITEM_TEAM_MEMBER_1:
			case MENU_ITEM_TEAM_MEMBER_3:
			case MENU_ITEM_TEAM_MEMBER_5:
			case MENU_ITEM_TEAM_MEMBER_7:
				teamSettings_menuItem++;
				break;
		}
	}else if(Keys_keyDown(keyDown))
	{
		stopTextEdit(editBuffer);
		if(teamSettings_menuItem==MENU_ITEM_TEAM_NAME)
			teamSettings_menuItem = MENU_ITEM_GRAVESTONE;
		else if(teamSettings_menuItem>=MENU_ITEM_TEAM_MEMBER_1 && teamSettings_menuItem<=MENU_ITEM_TEAM_MEMBER_6)
			teamSettings_menuItem+=2;
	}
	else if(Keys_keyDown(keyUp))
	{
		stopTextEdit(editBuffer);
		if(teamSettings_menuItem==MENU_ITEM_GRAVESTONE)
			teamSettings_menuItem = MENU_ITEM_TEAM_NAME;
		else if(teamSettings_menuItem>=MENU_ITEM_TEAM_MEMBER_3 && teamSettings_menuItem<=MENU_ITEM_TEAM_MEMBER_8)
			teamSettings_menuItem-=2;
	}

	// f1 and f1 select team 0 and 1 respectively
	if(Keys_keyUp(keyF1)) {
		tab = 0;
		stopTextEdit(editBuffer);
	}
	else if(Keys_keyUp(keyF2)) {
		tab = 1;
		stopTextEdit(editBuffer);
	}

	// if plus or minus pressed while gravestone selected, change it
	if(teamSettings_menuItem==1)
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
	teamSettings_isEditing = 0;
}
