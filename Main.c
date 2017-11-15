
#include "../Main.h"

void *doubleBuffer;

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
	// enable double-buffering
  // GrayDBufInit(doubleBuffer);
  
  // Sets It Up For _keytest usage..
	INT_HANDLER save_1 = GetIntVec(AUTO_INT_1); 
	INT_HANDLER save_5 = GetIntVec(AUTO_INT_5); 
	SetIntVec(AUTO_INT_1, DUMMY_HANDLER); 
	SetIntVec(AUTO_INT_5, DUMMY_HANDLER); 
	
	void *virtual=malloc (LCD_SIZE);
	
	//char virtual[LCD_SIZE];
	PortSet (virtual, 239, 127);
	
	// main loop!
	while(GameRunning==TRUE)
	{		
		// it all happens here
		//Game_update();
		clrscr();
		Keys_update();
		short i=0;
		for(i=0; i<18; i++)
		{
			long mask = (long)((long)1<<(i));
			DrawChar(0+i*6, 0, (Keys_keyDown(mask)==TRUE ? '1' : '0'), A_NORMAL);
			DrawChar(0+i*6, 10, (Keys_keyState(mask)==TRUE ? '1' : '0'), A_NORMAL);
			DrawChar(0+i*6, 20, (Keys_keyUp(mask)==TRUE ? '1' : '0'), A_NORMAL);
			
		}
		
		if(Keys_keyState(keyAction) && Keys_keyDown(keyEscape))
			GameRunning=FALSE;
			
		memcpy (LCD_MEM, virtual, LCD_SIZE);
	}
	
	LCD_restore (virtual);
	
	// free our double buffering buffer
	// free(doubleBuffer);
	
	//resets key stuff
	SetIntVec(AUTO_INT_1, save_1); 
	SetIntVec(AUTO_INT_5, save_5); 

}
