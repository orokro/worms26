/*
	SaveAndExit.c
	-------------
	
	This file is a snippet that is included raw in State.c
	
	This handles the logic for saving the game settings before exiting.

	This state is "headless" as in, it doesn't attempt to render anything to the screen.
*/


/**
 * @brief Writes a string file "wormrun" with the exit status for the wrapper program.
 * 
 * @param App_exitRequested 
 */
void write_wormrun(int App_exitRequested)
{
	FILE *f = fopen("wormrun", "wb");	// BINARY, not "w"
	if (!f) return;

	fputc(0, f);						// leading 0 byte
	fputc(App_exitRequested ? '1' : '0', f);
	fputc(0, f);						// terminating 0 byte
	fputc(STR_TAG, f);					// 0x2D
	fclose(f);
}


/**
	Called on the first-frame when the Games state machine is set to SaveAndExit mode.
*/
static void SaveAndExit_enter()
{
	// unlike other states, we'll only draw when there's changes, so let's draw once on start
	screenIsStale = STALE;

	// attempt to save our settings to the wormsdat file
	FileData_saveData();

	// write the wormrun file for the wrapper program
	write_wormrun(App_exitRequested);

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
