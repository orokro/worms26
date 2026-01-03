/*
	SaveAndExit.c
	-------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the logic for saving the game settings before exiting.
*/


/**
 * main drawing routine for the SaveAndExit menu
 */
void Draw_renderSaveAndExit()
{
	// gtfo if nothing has changed
	if(screenIsStale==0)
		return;

	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	FontSetSys(F_8x10);
	GrayDrawStr2B(21, 3, "Saving Settings", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(20, 2, "Saving Settings", A_XOR, lightPlane, darkPlane);

	// we're done drawing
	screenIsStale--;
}


/**
	Called on the first-frame when the Games state machine is set to SaveAndExit mode.
*/
static void SaveAndExit_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// gtfo of the game after saving settings
	GameRunning = FALSE;
}


/**
	Called every frame that the Games state machine is in SaveAndExit mode.
*/
static void SaveAndExit_update()
{
	Draw_renderSaveAndExit();
}


/**
	Called on the first-frame when the Games state machine leaves SaveAndExit mode.
*/
static void SaveAndExit_exit()
{

}
