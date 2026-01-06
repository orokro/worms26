/*
	MapSettings.c
	-------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the MapSettings state machine specific code.
*/

// define positions for layout
#define MAP_LEFT 38
#define MAP_TOP 20
#define LAND_LEFT 88
#define LAND_TOP 20

// define menu item ids
#define MENU_MAP_TEXTURE      	0
#define MENU_LAND_TYPE        	1

// help strings
const char *mapMenuText[5] = {
	"+/- To Adjust Map Texture",
	"+/- To Adjust Land Style",
};

// local var now that we have a new app
char map_menuItem = 0;


/**
 * main drawing routine for the MapSettings menu
 */
void Draw_renderMapSettings()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Map Setup");

	// draw help text based on menu item
	Draw_helpText(86, mapMenuText[(short)map_menuItem]);

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// -----------------

	// draw the map box label & current map texture
	GrayDrawStr2B(MAP_LEFT,  MAP_TOP, "Texture:", A_NORMAL, lightPlane, darkPlane);
	Draw_RectOutlineColor(MAP_LEFT, MAP_TOP+6, 34, 34, map_menuItem==MENU_MAP_TEXTURE ? 3 : 1);
	ClipSprite32_OR_R(MAP_LEFT+1, MAP_TOP+7, 32, tex_Ground[(short)Match_mapType], lightPlane);
	FastFilledRect_Invert_R(lightPlane, MAP_LEFT+1, MAP_TOP+7, MAP_LEFT+1+31, MAP_TOP+7+31);	
	ClipSprite32_OR_R(MAP_LEFT+1, MAP_TOP+7, 32, tex_Ground[(short)Match_mapType], darkPlane);

	// draw the land box label & current map texture
	GrayDrawStr2B(LAND_LEFT,  LAND_TOP, "Terrain:", A_NORMAL, lightPlane, darkPlane);
	Draw_RectOutlineColor(LAND_LEFT, LAND_TOP+6, 34, 34, map_menuItem==MENU_LAND_TYPE ? 3 : 1);
	ClipSprite32_OR_R(LAND_LEFT+1, LAND_TOP+7, 32, spr_Land[(short)Match_mapLandType], lightPlane);
	ClipSprite32_OR_R(LAND_LEFT+1, LAND_TOP+7, 32, spr_Land[(short)Match_mapLandType], darkPlane);
	// FastFilledRect_Invert_R(lightPlane, LAND_LEFT+1, LAND_TOP+7, LAND_LEFT+1+31, LAND_TOP+7+31);	
	
}


/**
	Called on the first-frame when the Games state machine is set to MapSettings mode.
*/
static void MapSettings_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;
}


/**
	Called every frame that the Games state machine is in MapSettings mode.
*/
static void MapSettings_update()
{
	Draw_renderMapSettings();

	// this only has check, so F5 returns to MatchMenu
	if(Keys_keyUp(keyF5|keyEscape))
	{
		State_transitionButton = BTN_ACCEPT;
		State_changeMode(menuMode_MatchMenu, 3);
	}	

	// left and right just toggle the map_menuItem from 0 to 1
	if(Keys_keyDown(keyLeft) || Keys_keyDown(keyRight))
	{
		map_menuItem = (map_menuItem==0) ? 1 : 0;
	}

	// if we're on map type, +/- changes it
	if(map_menuItem==MENU_MAP_TEXTURE)
	{
		if(Keys_keyUp(keyMinus) && Match_mapType>0)
			Match_mapType--;
		
		else if(Keys_keyUp(keyPlus) && Match_mapType<4)
			Match_mapType++;
	}

	// if we're on the land type, +/- changes it
	else if(map_menuItem==MENU_LAND_TYPE)
	{
		if(Keys_keyUp(keyMinus) && Match_mapLandType>0)
			Match_mapLandType--;
		
		else if(Keys_keyUp(keyPlus) && Match_mapLandType<2)
			Match_mapLandType++;
	}

	// redraw for any keypress
	if(Keys_keyUp(keyAny))
		screenIsStale = STALE;
}


/**
	Called on the first-frame when the Games state machine leaves MapSettings mode.
*/
static void MapSettings_exit()
{

}
