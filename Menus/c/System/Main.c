/*
	Main.c
	------

	It all begins here... as any C program.

	Here we kick off the gray scale rendering, interrupt hooks for key presses
	and other INIT.

	The main loop, though sparse, is also here.
*/


// includes
#include "Main.h"
#include "Keys.h"
#include  "State.h"

// the main double buffer for gray scale rendering
void *GblDBuffer;
void *lightPlane;
void *darkPlane;

// is the game running?
char GameRunning = TRUE;


// --------------------------------------------------------------------------------------------------------------------------------------


// Main Function
void _main(void)
{
	// Sets It Up For _keytest usage..
	INT_HANDLER save_1 = GetIntVec(AUTO_INT_1); 
	INT_HANDLER save_5 = GetIntVec(AUTO_INT_5); 
	SetIntVec(AUTO_INT_1, DUMMY_HANDLER); 
	SetIntVec(AUTO_INT_5, DUMMY_HANDLER); 

	// enable grayscale
	GrayOn();

	// allocate space for double-buffering
	void *dbuffer=malloc(GRAYDBUFFER_SIZE);

	// enable double-buffering
	GrayDBufInit(dbuffer);
	GblDBuffer=dbuffer;
  
  	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
	
	// before we can do the main game update loop, we need to change the state machine into the first state
	// Game_changeMode(menuMode_WormSelect);
	State_changeMode(menuMode_MainMenu, 0);
	
	// main loop!
	GameRunning = TRUE;
	while(GameRunning)
	{	
		// temporary (maybe permanent) short cut to always exit the game
		// (the pause menu also provides a method to exit the game)
		//if(Keys_keyState(keyAction) && Keys_keyDown(keyEscape))
		//	GameRunning=FALSE;
		if(Keys_keyDown(keyExit))
		{
			App_exitRequested = TRUE;
			State_changeMode(menuMode_SaveAndExit, 3);
		}

		// update game logic
		State_update();

		// now flip the planes
		GrayDBufToggleSync();
		
	  	/* On HW1, the flip will be effective only after 1 or 2 plane switches
	       depending on the phase. We must not draw to the "hidden" planes before
	       they are really hidden! */
	  	if (!_GrayIsRealHW2()) 
			GrayWaitNSwitches(1);
	  	
	  	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
		darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
	}
	
	// disable grayscale
	GrayOff();

	// free our buffers:
	free(GblDBuffer);
	
	//resets key stuff
	SetIntVec(AUTO_INT_1, save_1); 
	SetIntVec(AUTO_INT_5, save_5); 
}
