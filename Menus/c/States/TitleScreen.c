/*
	TitleScreen.c
	-------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the TitleScreen state machine specific code.
*/


/**
 * main drawing routine for the TitleScreen menu
 */
void Draw_renderTitleScreen()
{
	// start fresh
	Draw_clearBuffers();

	short i=0;
	for(i=0; i<5; i++)
		GrayClipSprite32_OR_R(i*32, 0, 100, spr_TitleLight[i], spr_TitleDark[i], lightPlane, darkPlane);
	
	// hackishly draw mask for the button
	const char offset = ((State_transitionTime>0) && (State_transitionButton & BTN_ACCEPT)) ? 2 : 0;
	GrayClipSprite16_OR_R(140+offset, 76+offset, 20, spr_Check_Fill, spr_Check_Fill, lightPlane, darkPlane);
	GrayClipSprite16_XOR_R(140+offset, 76+offset, 20, spr_Check_Fill, spr_Check_Fill, lightPlane, darkPlane);
	GrayClipSprite16_OR_R(140+offset, 76+offset, 20, spr_Check_Outline, spr_Check_Outline, lightPlane, darkPlane);
}


/**
	Called on the first-frame when the Games state machine is set to TitleScreen mode.
*/
static void TitleScreen_enter()
{

}


/**
	Called every frame that the Games state machine is in TitleScreen mode.
*/
static void TitleScreen_update()
{
	Draw_renderTitleScreen();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyAction|keyF1|keyF5|keyEscape))
	{
		State_transitionButton = BTN_ACCEPT;
		State_changeMode(menuMode_MainMenu, 3);
	}
		
}


/**
	Called on the first-frame when the Games state machine leaves TitleScreen mode.
*/
static void TitleScreen_exit()
{

}
