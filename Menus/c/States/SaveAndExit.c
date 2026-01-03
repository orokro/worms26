/*
	SaveAndExit.c
	-------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the logic for saving the game settings before exiting.

	This state is "headless" as in, it doesn't attempt to render anything to the screen.
*/


/**
	Called on the first-frame when the Games state machine is set to SaveAndExit mode.
*/
static void SaveAndExit_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// attempt to save our settings to the wormsdat file
	FileData_saveData();

	// gtfo of the game after saving settings
	GameRunning = FALSE;
}


/**
	Called every frame that the Games state machine is in SaveAndExit mode.
*/
static void SaveAndExit_update()
{

}


/**
	Called on the first-frame when the Games state machine leaves SaveAndExit mode.
*/
static void SaveAndExit_exit()
{

}
