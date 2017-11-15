
#include "../Headers/System/Main.h"

void keyTestRoutine();

void *doubleBuffer;
void *virtual;

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

// debug routine for the key-system. Looks like it's working!
void keyTestRoutine()
{
	// read keys
	Keys_update();
	
	// loop and display the Down, State, and Up states of each key
	short i=0;
	for(i=0; i<18; i++)
	{
		long mask = (long)((long)1<<(i));
		DrawChar(0+i*6, 0, (Keys_keyDown(mask)==TRUE ? '1' : '0'), A_NORMAL);
		DrawChar(0+i*6, 10, (Keys_keyState(mask)==TRUE ? '1' : '0'), A_NORMAL);
		DrawChar(0+i*6, 20, (Keys_keyUp(mask)==TRUE ? '1' : '0'), A_NORMAL);
	}
	
	// exit with this key-combo
	if(Keys_keyState(keyAction) && Keys_keyDown(keyEscape))
		GameRunning=FALSE;
	
	// copy or gfx to the screen
	memcpy (LCD_MEM, virtual, LCD_SIZE);
}


// Main Function
void _main(void)
{
	// enable double-buffering
  // GrayDBufInit(doubleBuffer);
 
  // Sets It Up For _keytest usage..
	INT_HANDLER save_1 = GetIntVec(AUTO_INT_1); 
	INT_HANDLER save_5 = GetIntVec(AUTO_INT_5); 
	SetIntVec(AUTO_INT_1, DUMMY_HANDLER); 
	SetIntVec(AUTO_INT_5, DUMMY_HANDLER); 
	
	// allocat and set a virtual screen to render to
	// (this will be black and white until I implement graphics and double buffering)
	virtual = malloc(LCD_SIZE);
	PortSet (virtual, 239, 127);

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
		
		// copy whtaever was rendered to the screen!
		memcpy (LCD_MEM, virtual, LCD_SIZE);
	}
	
	// retstore our virtual screen and free the memory before we return to the OS
	LCD_restore (virtual);
	PortRestore();
	
	// free our double buffering buffer
	// free(doubleBuffer);
	
	//resets key stuff
	SetIntVec(AUTO_INT_1, save_1); 
	SetIntVec(AUTO_INT_5, save_5); 

}
