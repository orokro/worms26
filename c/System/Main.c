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
#include "Game.h"
#include "Map.h"
#include "Keys.h"
#include "Draw.h"


// the main buffer for the map
void *mapBuffer, *mapLight, *mapDark;

// the main double buffer for gray scale rendering
void *GblDBuffer;
void *lightPlane;
void *darkPlane;

// is the game running?
char GameRunning = TRUE;



// --------------------------------------------------------------------------------------------------------------------------------------


// Efficient 16-bit bit reversal algorithm
unsigned short reverseBits16(unsigned short n)
{
    n = ((n >> 1) & 0x5555) | ((n & 0x5555) << 1);
    n = ((n >> 2) & 0x3333) | ((n & 0x3333) << 2);
    n = ((n >> 4) & 0x0F0F) | ((n & 0x0F0F) << 4);
    return (n >> 8) | (n << 8);
}


// helper for mask generation
unsigned short generateMaskRow(unsigned short outline) {
    if (outline == 0) return 0xFFFF;
    unsigned short left = 15, right = 0;
    
    // Find rightmost bit
    unsigned short temp = outline;
    while (!(temp & 1)) {
        temp >>= 1;
        right++;
    }
    // Find leftmost bit
    temp = outline;
    while (!(temp & 0x8000)) {
        temp <<= 1;
        left--;
    }
    
    unsigned short mask = (0xFFFF >> (15 - left)) & (0xFFFF << right);
    return ~mask;
}


// programmatically generate missing worm masks from their components
void GenerateWormMasks() {
    int w, r;
    unsigned short* currentBufferPos = wormMaskBuffer;
    
    for (w = 0; w < NUM_WORM_SPRITES; w++) {
        // Only generate if it's a mask (0) AND it's NULL (missing in ROM)
        if (wormSpriteTypes[w] == 0 && wormsSprites[w] == NULL) {
            unsigned char h = wormSpriteHeights[w];
            wormsSprites[w] = currentBufferPos;
            
            for (r = 0; r < h; r++) {
                // OR all components that follow this mask until we hit another mask or end of array
                unsigned short shape = 0;
                int c = w + 1;
                while (c < NUM_WORM_SPRITES && wormSpriteTypes[c] == 1) {
                    if (wormsSprites[c] != NULL) {
                        shape |= wormsSprites[c][r];
                    }
                    c++;
                }
                
                *currentBufferPos = generateMaskRow(shape);
                currentBufferPos++;
            }
        }
    }
}


// Call this ONCE in _main()
void GenerateFlippedSprites() {
    int w, r;
    
    // 1. Flip weapons
    for(w = 0; w < NUM_WEAPONS; w++) {
        for(r = 0; r < 11; r++) {
            // Read ROM, flip bits, write to RAM
            spr_weapons_flipped[w][r] = reverseBits16(spr_weapons[w][r]);
        }
    }

    // 2. Flip worms
    unsigned short* currentBufferPos = wormFlipBuffer;
    for(w = 0; w < NUM_WORM_SPRITES; w++) {
        // save the pointer to the start of this flipped sprite
        wormsSpritesFlipped[w] = currentBufferPos;

        // get info about the original sprite
        unsigned short* src = wormsSprites[w];
        unsigned char h = wormSpriteHeights[w];

        // flip each row
        for(r = 0; r < h; r++) {
            if (src != NULL) {
                *currentBufferPos = reverseBits16(src[r]);
            } else {
                *currentBufferPos = 0;
            }
            currentBufferPos++;
        }
    }
}



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
	mapLight = malloc(200*10*sizeof(unsigned long));
	mapDark = malloc(200*10*sizeof(unsigned long));
	
	// enable grayscale
	GrayOn();

	// allocate space for double-buffering
	void *dbuffer=malloc(GRAYDBUFFER_SIZE);

	// init state related to starting match
	Game_initRound();
	
	// show the cake loading screen
	Draw_cake(0, 100);
	
	// render the map and spawn items on the map (worms, oil drums, etc)
	Map_makeMap();
	
	// enable double-buffering
	GrayDBufInit(dbuffer);
	GblDBuffer=dbuffer;
  
  	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
	
	// before we can do the main game update loop, we need to change the state machine into the first state
	Game_changeMode(gameMode_WormSelect);

	// generates masks for worms that are missing them in ROM
	GenerateWormMasks();

	// flips weapon sprites in ram so we can draw them facing left when needed
	GenerateFlippedSprites();
	
	// done loading
	Draw_cake(1,1);

	int i=0;

	// main loop!
	GameRunning=TRUE;
	while(GameRunning)
	{	
		// temporary (maybe permanent) short cut to always exit the game
		// (the pause menu also provides a method to exit the game)
		//if(Keys_keyState(keyAction) && Keys_keyDown(keyEscape))
		//	GameRunning=FALSE;
		if(Keys_keyDown(keyExit))
			GameRunning=FALSE;

		// update game logic
		if(i==0) Game_update();
		//i++;
		//if(i>400)
		//	i = 0;
			
		// now flip the planes
		GrayDBufToggleSync();
		
	  	/* On HW1, the flip will be effective only after 1 or 2 plane switches
	       depending on the phase. We must not draw to the "hidden" planes before
	       they are really hidden! */
	  	if (!_GrayIsRealHW2()) GrayWaitNSwitches(1);
	  	
	  	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
		darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);

		if(Game_debugFreeze)
		{
			while(!Keys_keyDown(keyF5))
				Keys_update();

			Game_debugFreeze = FALSE;
		}
	}
	
	// disable grayscale
	GrayOff();

	// free our buffers:
	free(mapLight);
	free(mapDark);
	free(GblDBuffer);
	
	//resets key stuff
	SetIntVec(AUTO_INT_1, save_1); 
	SetIntVec(AUTO_INT_5, save_5); 
}
