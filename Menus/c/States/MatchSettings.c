/*
	MatchSettings.c
	---------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MatchSettings state machine specific code.
*/

// help strings
const char *settingsMenuText[12] = {
	"Worm Starting Health",
	"Select Worms On Turn Start",
	"Place Worms on  Game Start",
	"Turn Time",
	"Artillery Mode",
	"Enable Mines",
	"Mine Fuse Length",
	"Enable Dud Mines",
	"Enable Oil Drums",
	"Enable Tool Crates",
	"Enable Health Crates",
	"Enable Weapon Crates",
};

// pointers to the various health setting sprites

// local var now that we have a new app
char matchSettings_menuItem = 0;

// define menu item ids
#define MENU_ITEM_WORM_HEALTH         0
#define MENU_ITEM_SELECT_WORMS        1
#define MENU_ITEM_PLACE_WORMS         2
#define MENU_ITEM_TURN_TIME           3
#define MENU_ITEM_ARTILLERY_MODE      4
#define MENU_ITEM_ENABLE_MINES        5
#define MENU_ITEM_MINE_FUSE_LENGTH    6
#define MENU_ITEM_ENABLE_DUD_MINES    7
#define MENU_ITEM_ENABLE_OIL_DRUMS    8
#define MENU_ITEM_ENABLE_TOOL_CRATES  9
#define MENU_ITEM_ENABLE_HEALTH_CRATES 10
#define MENU_ITEM_ENABLE_WEAPON_CRATES 11

// layout defines
#define GENERAL_FRAME_TOP 20
#define GENERAL_FRAME_LEFT 80-52
#define GENERAL_FRAME_WIDTH 104
#define OBJECT_FRAME_TOP 49
#define OBJECT_FRAME_LEFT 4
#define OBJECT_FRAME_WIDTH 84
#define CRATE_FRAME_TOP 49
#define CRATE_FRAME_LEFT 92
#define CRATE_FRAME_WIDTH 64


/**
 * @brief Draws the settings frame
 * 
 * @param x - x position
 * @param y - y position
 * @param width - width of frame
 * @param title - title text
 */
void drawSettingsFrame(char x, char y, char width, const char *title)
{
	// Draw the text centered at the top
	short titleLen = DrawStrWidth(title, F_4x6);
	short titleX = x + (width - titleLen) / 2;
	GrayDrawStr2B(titleX, y, title, A_NORMAL, lightPlane, darkPlane);

	// the frame is made out of 5 lines - two on top on either side of the text:
	// from x+1 to titleX-1, y+3, and from x+width-1 to titleX+titleLen+1, y+3
	FastDrawHLine_R(lightPlane, (short)(x+1), (short)(titleX-1), (short)(y+3), A_NORMAL);
	FastDrawHLine_R(lightPlane, (short)(titleX+titleLen+1), (short)(x+width-2), (short)(y+3), A_NORMAL);

	// then two on either side, from x, y+4 to x, y+255, and from x+width-1, y+4 to x+width-1, y+22
	FastDrawVLine_R(lightPlane, (short)x, (short)(y+4), (short)(y+24), A_NORMAL);
	FastDrawVLine_R(lightPlane, (short)(x+width-1), (short)(y+4), (short)(y+24), A_NORMAL);

	// lastly one on the bottom from x+1, y+22 to x+width-1, y+22
	FastDrawHLine_R(lightPlane, (short)(x+1), (short)(x+width-2), (short)(y+25), A_NORMAL);
}


/**
 * @brief Draws an options box w/ sprite and optional disabled X
 * 
 * @param x - x
 * @param y - y
 * @param isSelected - true if selected
 * @param isDisabled - true if disabled setting
 * @param optionSpriteHeight - height of option sprite
 * @param spriteOffsetY - y offset to draw sprite at
 * @param optionSpriteLight - light plane sprite to draw inside box
 * @param optionSpriteDark - dark plane sprite to draw inside box
 */
void drawOptionsBox(short x, short y, char isSelected, char isDisabled, char optionSpriteHeight, char spriteOffsetY, const unsigned short* optionSpriteLight, const unsigned short* optionSpriteDark)
{
	// draw the box
	if(isSelected)
		GrayClipSprite32_OR_R(x, y, 16, settingsIconBox, settingsIconBox, lightPlane, darkPlane);
	else
		GrayClipSprite32_OR_R(x, y, 16, settingsIconBox, settingsIconBox, lightPlane, lightPlane);

	// draw the sprite inside the box, of set to +1, +1
	GrayClipSprite16_OR_R(x+1, y+1+spriteOffsetY, optionSpriteHeight, optionSpriteLight, optionSpriteDark, lightPlane, darkPlane);

	// if it's disabled, draw an x on top of the sprite
	if(isDisabled)
	{
		GrayClipSprite16_AND_R(x+1, y+2, 12, spr_Disabled_Mask, spr_Disabled_Mask, lightPlane, darkPlane);
		GrayClipSprite16_OR_R(x+1, y+2, 12, spr_Disabled_Outline, spr_Disabled_Outline, lightPlane, darkPlane);
	}
}


/**
 * main drawing routine for the MatchSettings menu
 */
void Draw_renderMatchSettingsMenu()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Match Settings");

	// draw help text based on menu item
	Draw_helpText(86, settingsMenuText[(short)matchSettings_menuItem]);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// -----------------

	// draw the three settings frames
	drawSettingsFrame(GENERAL_FRAME_LEFT, GENERAL_FRAME_TOP, GENERAL_FRAME_WIDTH, "General Options:");
	drawSettingsFrame(OBJECT_FRAME_LEFT, OBJECT_FRAME_TOP, OBJECT_FRAME_WIDTH, "Object Options:");
	drawSettingsFrame(CRATE_FRAME_LEFT, CRATE_FRAME_TOP, CRATE_FRAME_WIDTH, "Crate Options:");

	// -----------------

	// draw the row of 5 general options in the general frame

	// Worm Starting Health
	const unsigned short* healthSprite = Match_wormStartHealth==50 ? spr_Option_Health[0] : (Match_wormStartHealth==100 ? spr_Option_Health[1] : spr_Option_Health[2]);
	drawOptionsBox(GENERAL_FRAME_LEFT+3, GENERAL_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_WORM_HEALTH,
		FALSE, 14, 0, healthSprite, healthSprite);
	
	// Select Worms On Turn Start
	const unsigned short* selectionSprite = Match_allowWormSelection ? spr_SelectionArrowFrame1 : spr_NoSelectArrow;
	const char offset = (Match_allowWormSelection ? 1 : 0);
	drawOptionsBox(GENERAL_FRAME_LEFT+23, GENERAL_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_SELECT_WORMS,
		FALSE, 14, 0, selectionSprite+offset, selectionSprite+offset);

	// Place Worms on Game Start
	drawOptionsBox(GENERAL_FRAME_LEFT+43, GENERAL_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_PLACE_WORMS,
		!Match_strategicPlacement, 13, 0, spr_Option_PlaceWorms, spr_Option_PlaceWorms);

	// Turn Time
	const unsigned short* turnTimeSprite = (Match_turnTime==30 ? spr_Option_Timer[0] : (Match_turnTime==45 ? spr_Option_Timer[1] : spr_Option_Timer[2]));
	drawOptionsBox(GENERAL_FRAME_LEFT+63, GENERAL_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_TURN_TIME,
		FALSE, 14, 0, turnTimeSprite, turnTimeSprite);

	// Artillery Mode
	drawOptionsBox(GENERAL_FRAME_LEFT+83, GENERAL_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ARTILLERY_MODE,
		!Match_artilleryMode, 12, 1, spr_Option_Artillery, spr_Option_Artillery);

	// -----------------

	// draw the row of 4 object options in the object frame

	// Enable Mines
	drawOptionsBox(OBJECT_FRAME_LEFT+3, OBJECT_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ENABLE_MINES,
		!Match_minesEnabled, 11, 0, spr_Option_MineBlank_Light, spr_Option_MineBlank_Dark);
	
	// Mine Fuse Length
	drawOptionsBox(OBJECT_FRAME_LEFT+23, OBJECT_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_MINE_FUSE_LENGTH,
		FALSE, 14, 0, spr_Option_MineLight[(short)Match_mineFuseLength], spr_Option_MineDark[(short)Match_mineFuseLength]);

	// Enable Dud Mines
	drawOptionsBox(OBJECT_FRAME_LEFT+43, OBJECT_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ENABLE_DUD_MINES,
		!Match_dudMines, 14, 0, spr_Option_DudSmoke_Light, spr_Option_DudSmoke_Dark);

	// Enable Oil Drums
	drawOptionsBox(OBJECT_FRAME_LEFT+63, OBJECT_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ENABLE_OIL_DRUMS,
		!Match_oilDrumsEnabled, 12, 1, spr_OilLight, spr_OilDark);

	// -----------------

	// draw the row of 3 crate options in the crate frame

	// Enable Tool Crates
	drawOptionsBox(CRATE_FRAME_LEFT+3, CRATE_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ENABLE_TOOL_CRATES,
		!Match_toolCratesEnabled, 12, 1, spr_CrateTool+12, spr_CrateTool+24);

	// Enable Health Crates
	drawOptionsBox(CRATE_FRAME_LEFT+23, CRATE_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ENABLE_HEALTH_CRATES,
		!Match_healthCratesEnabled, 12, 1, spr_CrateHealth+12, spr_CrateHealth+24);

	// Enable Weapon Crates
	drawOptionsBox(CRATE_FRAME_LEFT+43, CRATE_FRAME_TOP+7, matchSettings_menuItem==MENU_ITEM_ENABLE_WEAPON_CRATES,
		!Match_weaponCratesEnabled, 12, 1, spr_CrateWeapon+12, spr_CrateWeapon+24);			

	// -----------------

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to MatchSettings mode.
*/
static void MatchSettings_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;
}


/**
	Called every frame that the Games state machine is in MatchSettings mode.
*/
static void MatchSettings_update()
{
	Draw_renderMatchSettingsMenu();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyF5|keyEscape))
		State_changeMode(menuMode_MatchMenu);

	// left/right simply increment and decrement the menu item:
	if(Keys_keyDown(keyLeft))
	{
		if(matchSettings_menuItem>0)
			matchSettings_menuItem--;
	}
	else if(Keys_keyDown(keyRight))
	{
		if(matchSettings_menuItem<11)
			matchSettings_menuItem++;
	}

	// if down is pushed it should add 5 if matchSettings_menuItem <5
	// if up is pushed, it should subtract 5 if matchSettings_menuItem >5 and <11
	// if up is pushed and exactly 5, it should go to 0
	// if up is pushed and exactly 11, it should go to 4
	if(Keys_keyDown(keyDown))
	{
		if(matchSettings_menuItem<7)
			matchSettings_menuItem+=5;
	}
	else if(Keys_keyDown(keyUp))
	{
		if(matchSettings_menuItem>6 && matchSettings_menuItem<12)
			matchSettings_menuItem-=5;
		else if(matchSettings_menuItem==5)
			matchSettings_menuItem=0;
		else if(matchSettings_menuItem==11)
			matchSettings_menuItem=4;
	}

	// if action is pressed it should toggle the corresponding setting
	if(Keys_keyUp(keyAction))
	{
		switch(matchSettings_menuItem)
		{
			case 0: // Worm Starting Health
				if(Match_wormStartHealth==50)
					Match_wormStartHealth=100;
				else if(Match_wormStartHealth==100)
					Match_wormStartHealth=200;
				else
					Match_wormStartHealth=50;
				break;

			case 1: // Select Worms On Turn Start
				Match_allowWormSelection = !Match_allowWormSelection;
				break;

			case 2: // Place Worms on Game Start
				Match_strategicPlacement = !Match_strategicPlacement;
				break;

			case 3: // Turn Time
				if(Match_turnTime==30)
					Match_turnTime=45;
				else if(Match_turnTime==45)
					Match_turnTime=60;
				else
					Match_turnTime=30;
				break;

			case 4: // Artillery Mode
				Match_artilleryMode = !Match_artilleryMode;
				break;

			case 5: // Enable Mines
				Match_minesEnabled = !Match_minesEnabled;
				break;

			case 6: // Mine Fuse Length
				Match_mineFuseLength++;
				if(Match_mineFuseLength>4)
					Match_mineFuseLength=0;
				break;

			case 7: // Enable Dud Mines
				Match_dudMines = !Match_dudMines;
				break;

			case 8: // Enable Oil Drums
				Match_oilDrumsEnabled = !Match_oilDrumsEnabled;
				break;

			case 9: // Enable Tool Crates
				Match_toolCratesEnabled = !Match_toolCratesEnabled;
				break;

			case 10: // Enable Health Crates
				Match_healthCratesEnabled = !Match_healthCratesEnabled;
				break;

			case 11: // Enable Weapon Crates
				Match_weaponCratesEnabled = !Match_weaponCratesEnabled;
				break;
		}

		// mark screen as stale to redraw
		screenIsStale = STALE;
	}

	// redraw for any keypress
	if(Keys_keyUp(keyAny))
		screenIsStale = STALE;
}


/**
	Called on the first-frame when the Games state machine leaves MatchSettings mode.
*/
static void MatchSettings_exit()
{

}
