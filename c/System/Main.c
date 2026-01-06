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
#include "FileData.h"
#include "Match.h"


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
unsigned short generateMaskRow(unsigned short outline)
{
    if (outline == 0)
		return 0xFFFF;
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
void GenerateWormMasks()
{
    int w, r;
    unsigned short* currentBufferPos = wormMaskBuffer;
    unsigned short* bufferEnd = wormMaskBuffer + WORM_GENERATED_MASK_BUFFER_SIZE;
    
    for (w = 0; w < NUM_WORM_SPRITES; w++) {
        // Only generate if it's a mask (0)
        if (wormSpriteTypes[w] == 0) {
            // Check if it's missing (0) OR if it points to our RAM buffer (from a previous run)
            int needsGeneration = 0;
            if (wormsSprites[w] == (unsigned short*)0) {
                needsGeneration = 1;
            } else if (wormsSprites[w] >= wormMaskBuffer && wormsSprites[w] < bufferEnd) {
                needsGeneration = 1;
            }

            if (needsGeneration) {
                unsigned char h = wormSpriteHeights[w];
                wormsSprites[w] = currentBufferPos;
                
                for (r = 0; r < h; r++) {
                    // Safety check to prevent overflow
                    if (currentBufferPos >= bufferEnd) break;

                    // OR all components that follow this mask until we hit another mask or end of array
                    unsigned short shape = 0;
                    int c = w + 1;
                    while (c < NUM_WORM_SPRITES && wormSpriteTypes[c]==1) {
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
}


// Call this ONCE in _main()
void GenerateFlippedSprites()
{
    int w, r;
    unsigned short* bufferEnd = wormFlipBuffer + WORM_FLIP_BUFFER_SIZE;
    
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
            // Safety check to prevent overflow
            if (currentBufferPos >= bufferEnd) break;

            if (src != NULL) {
                *currentBufferPos = reverseBits16(src[r]);
            } else {
                *currentBufferPos = 0;
            }
            currentBufferPos++;
        }
    }
}


/**
 * @brief calculates the distance between two points	
 * 
 * @param x1 - x1
 * @param y1 - y1
 * @param x2 - x2
 * @param y2 - y2
 * @return short - distance between points
 */
short dist(short x1, short y1, short x2, short y2)
{
	long dx = (long)x2 - (long)x1;
	long dy = (long)y2 - (long)y1;
	long a = dx < 0 ? -dx : dx;
	long b = dy < 0 ? -dy : dy;
	long aSquared = a*a;
	long bSquared = b*b;
	long cSquared = aSquared+bSquared;
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
	
	// Safety check for malloc failure
	if (mapLight == NULL || mapDark == NULL) {
		if(mapLight) free(mapLight);
		if(mapDark) free(mapDark);
		GrayOff();
		SetIntVec(AUTO_INT_1, save_1); 
		SetIntVec(AUTO_INT_5, save_5); 
		return;
	}

	// enable grayscale
	GrayOn();

	// allocate space for double-buffering
	void *dbuffer=malloc(GRAYDBUFFER_SIZE);

	// enable double-buffering
	GrayDBufInit(dbuffer);
	GblDBuffer=dbuffer;

	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);

	// load our settings file to get match settings from the menu executable
	FileData_loadData();

	// init state related to starting match
	Game_initRound();
	
	// show the cake loading screen
	Draw_cake(0, 100);
	
	// render the map and spawn items on the map (worms, oil drums, etc)
	Map_makeMap();

	// before we can do the main game update loop, we need to change the state machine into the first state
	// if we have to place worms, go to that mode first
	if(Match_strategicPlacement)
		// start placing worms
		Game_changeMode(gameMode_PlaceWorms);
	else
		// otherwise, go to worm select mode
		Game_changeMode(gameMode_WormSelect);
	
	Draw_cake(50,100);
	
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
		if(i==0)
			Game_update();
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
