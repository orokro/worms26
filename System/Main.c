#include "Main.h"

/*
	Main
	----

	It all begins here... as any C program.

	Here we kick off the gray scale rendering, interupt hooks for key presses
	and other INIT.

	The main loop, though sparse, is also here.
*/

// the main buffer for the map
void *mapBuffer;

// the main double buffer for gray scale rendering
void *GblDBuffer;

// is the game running?
char GameRunning = TRUE;



// --------------------------------------------------------------------------------------------------------------------------------------



// global method to calculate distance between two points
// maybe I'll move this into a math file one day
short dist(short x1, short y1, short x2, short y2)
{
	short a = abs(x2-x1);
	short b = abs(y2-y1);
	short aSquared = a*a;
	short bSquared = b*b;
	short cSquared = aSquared+bSquared;
	short c = (short)sqrt((float)cSquared);
	return c;
}


// Main Function
void _main(void)
{
	// Sets It Up For _keytest usage..
	INT_HANDLER save_1 = GetIntVec(AUTO_INT_1); 
	INT_HANDLER save_5 = GetIntVec(AUTO_INT_5); 
	SetIntVec(AUTO_INT_1, DUMMY_HANDLER); 
	SetIntVec(AUTO_INT_5, DUMMY_HANDLER); 

	// allocate our map buffer
	mapBuffer = malloc(200*10*sizeof(unsigned long));// malloc(LCD_SIZE*4);

	// render the map and spawn items on the map (worms, oil drums, etc)
	Map_makeMap();
	
	// enable grayscale
	GrayOn();

	// allocate space for double-buffering
	void *dbuffer=malloc(GRAYDBUFFER_SIZE);
	
	// enable double-buffering
	GrayDBufInit(dbuffer);
	GblDBuffer=dbuffer;
  
	// before we can do the main game update loop, we need to change the state machine into the first state
	Game_changeMode(gameMode_WormSelect);

	// main loop!
	GameRunning=TRUE;
	while(GameRunning==TRUE)
	{	
		// temporary (maybe perminant) short cut to always exit the game
		// (the pause menu also provides a method to exit the game)
		if(Keys_keyState(keyAction) && Keys_keyDown(keyEscape))
			GameRunning=FALSE;

		// handles all updates for the game!
		Game_update();
		
		// now flip the planes
		GrayDBufToggleSync();
	    
	  	/* On HW1, the flip will be effective only after 1 or 2 plane switches
	       depending on the phase. We must not draw to the "hidden" planes before
	       they are really hidden! */
	  	if (!_GrayIsRealHW2()) GrayWaitNSwitches(2);
	}
	
	// disable grayscale
	GrayOff();

	// free our buffers:
	free(mapBuffer);
	free(GblDBuffer);
	
	//resets key stuff
	SetIntVec(AUTO_INT_1, save_1); 
	SetIntVec(AUTO_INT_5, save_5); 

}
