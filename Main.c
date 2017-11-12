
#include "Main.h"

void *doubleBuffer;

// Main Function
void _main(void)
{
	// Place your code here.
	
	// enable double-buffering
  //GrayDBufInit(doubleBuffer);
  
  	// Sets It Up For _keytest usage..
	INT_HANDLER save_1 = GetIntVec(AUTO_INT_1); 
	INT_HANDLER save_5 = GetIntVec(AUTO_INT_5); 
	SetIntVec(AUTO_INT_1, DUMMY_HANDLER); 
	SetIntVec(AUTO_INT_5, DUMMY_HANDLER); 
	
	while(!(_keytest(RR_2ND) && _keytest(RR_ESC)))
	{	
		long i;
		
		// clear the screen
		ClrScr();
		
		// show the headers for our states:
		DrawStr(0, 0, "Keys Down:", A_NORMAL);
		DrawStr(0, 30, "Keys State:", A_NORMAL);
		DrawStr(0, 60, "Keys Up:", A_NORMAL);
		
		
		// update keystates
		Keys_update();
		
		// loop from 1 to 20 and show the states of our keys on the screen
		for(i=0; i<19; i++)
		{
			long testValue = ((long)1<<(i));
			
			// we can use the i instead of our defines for now
			char kDown = Keys_keyDown(testValue);
			char kState = Keys_keyState(testValue);
			char kUp = Keys_keyUp(testValue);
			
			// the position on screen will be in reverse order, because
			// the lowest bit is right-most
			short screenX = 114-(short)i*6;
			
			// print the binary for this key..
			DrawChar(screenX, 15, kDown ? '1' : '0', A_NORMAL);
			DrawChar(screenX, 45, kState ? '1' : '0', A_NORMAL);
			DrawChar(screenX, 75, kUp ? '1' : '0', A_NORMAL);

		}
		
//
	}
	
	//free(doubleBuffer);
	//resets some stuff
	SetIntVec(AUTO_INT_1, save_1); 
	SetIntVec(AUTO_INT_5, save_5); 

}
