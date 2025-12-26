/*
	Draw.c
	------
	
	Here we define our global drawing routines, for drawing each kind of scene:
		- Main game
		- Pause Menu
		- Weapons Menu
		
	C Source File
	Created 11/11/2017; 11:34:12 PM
*/


// includes
#include "Main.h"
#include "Draw.h"
#include "Camera.h"
#include "Keys.h"
#include "SpriteData.h"
#include "Game.h"
#include "Crates.h"
#include "OilDrums.h"
#include "Mines.h"
#include "Worms.h"
#include "Weapons.h"
#include "Explosions.h"
#include "Map.h"
#include "Match.h"
#include "StatusBar.h"

// Global Variables (Defined in Game.c)
extern char Game_wormAnimState;    // Current animation state (0=None, 1=Jump, 2=Backflip)
extern int  Game_wormAnimTimer;    // Timer to track animation progress
extern char Game_wormFlipStartDir; // Direction worm was facing when flip started (0=Right, Mask=Left)

// these sprites will be generated at the begining of the each turn to match the current wind conditions
unsigned long windSprites[3][3];

// these sprites will be updated when a worm takes damage
unsigned long teamHealthSprites_light[3] = { 0, 0, 0 };
unsigned long teamHealthSprites_dark[3] = { 0, 0, 0 };

// flipped weapon sprites
unsigned short spr_weapons_flipped[NUM_WEAPONS][11];

// buffers for our weapons screen
void *weaponsLight, *weaponsDark;

// full screen ref
const SCR_RECT fullScreen = {{0, 0, 159, 99}};

// Lookup table for N bits set to 1 (0 to 15 bits)
// Placed outside or static inside to avoid rebuilding it every call
static const unsigned short BIT_MASKS[] = {
 	0x0000, 0x0001, 0x0003, 0x0007, 0x000F, // 0-4
 	0x001F, 0x003F, 0x007F, 0x00FF,         // 5-8
 	0x01FF, 0x03FF, 0x07FF, 0x0FFF,         // 9-12
 	0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF          // 13-15
};



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Convents world coordinates to screen coordinates, mutates the passed in pointers, and returns TRUE/FALSE if on screen (roughly).
 * 
 * The game is updated in world coordinates, but when it comes time to draw,
 * everything needs to be in screen coordinates.
 *
 * This method takes in the Camera's X/Y position and adjusts the passed in X/Y pointers accordingly.
 * 
 * If the final X/Y screen position determined is out of bounds for the screen, the method will return FALSE.
 * Otherwise, TRUE.
 *
 * The current bounding for the screen is +/- 8 pixels horizontal on each side of the screen, and
 * +/- 16 pixels vertically.
 *
 * This may need to be changed, as the final sprites are decided
 *
 * @param x a pointer reference to the x value to change to screen coordinates
 * @param y a pointer reference to the y value to change to screen coordinates
 * @return a char boolean either TRUE or FALSE if the coordinates are roughly on screen
*/
char worldToScreen(short *x, short *y)
{
	// casting over kill because of mysterious crash
	*x = (short)((short)80 + (short)((short)(*x)-(short)camX));
	*y = (short)((short)50 + (short)((short)(*y)-(short)camY));
	
	// if anything is out of bounds, return false:
	if((*x)<-32 || (*x)>192 || (*y)<-5 || (*y)>105)
    return FALSE;
	else
		return TRUE;
}


// we'll inline-include all the environment drawing code to clean up Draw.c
// this includes leafs, clouds, mountains, and water
#include "Environment.c"

// we'll inline-include all the HUD / UI drawing code to clean up Draw.c
#include "UI.c"


// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * main drawing routine for the game, e.g. map, worms, weapons, etc
 */
void Draw_renderGame()
{
	// maindraw

	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	// small fonts used in game, pause menu different
	FontSetSys(0);

	// draw background mountains
	drawMountains();
	
	// leaves before map...
	drawLeavesAndClouds();
	
	// draw the map
	Map_draw();	
	
	// draw oil drums first, as everything else should overlap them
	OilDrums_drawAll();
	
	// draw crates on top of oil drums...
	Crates_drawAll();
	
	// draw our wormy bros
	Worm_drawAll();
	
	// mines are important, so draw them on top of everything else
	Mines_drawAll();
	
	// draw water that's infront of everything but the HUD
	drawWater(FALSE);
	
	// draw explosions over everything except HUD, since they reverse the pixels
	Explosion_drawAll();
	
	// draw weapon objects in the game...
	Weapons_drawAll();
	
	// if a mark was picked with the cursor this will render it
	drawCursorAndXSpot();
	
	// Draw status bar messages
	StatusBar_draw();

	// HUD stuff is drawn last since it needs to go on top of all game elements
	Draw_HUD();
	
	// if the game mode is worm select, draw the selection arrow
	if(Game_mode==gameMode_WormSelect)
		drawSelectArrow();
	
	// extra stuff to draw if the worm has certain weapons selected
	if(Game_currentWeaponSelected!=-1)
		drawWeaponDetails();
		
	// for now, we will output a bunch of debug info on the screen
	
	// game modes by name	{"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	static const char modes[9][16] = {"Select", "Turn", "", "", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// draw the current and previous game mode on the screen
	DrawStr(60,8,modes[(short)Game_mode], A_XOR);

	// static char weaponActiveStr[5];
	// sprintf(weaponActiveStr, "%d", (short)Weapon_active);
	// DrawStr(1, 10, weaponActiveStr, A_XOR);	

	// draw the current grace time, turn time, and retreat time on the screen
	// NOTE: for some reason, drawing sudden death time instead of retreat time crashes the game)
	//char timeStr[40];
	//sprintf(timeStr, "time: %d, %d, %d", (short)(Game_graceTimer/TIME_MULTIPLIER), (short)(Game_timer/TIME_MULTIPLIER), (short)(Game_retreatTimer));
	//DrawStr(60,1,timeStr, A_XOR);	
	
	//unsigned short currentMask = 1;
	//currentMask = (unsigned short)((unsigned short)currentMask<<(Worm_currentWorm));
	//DrawStr(80,10, (((Worm_onGround & currentMask)>0) ? "Grounded" : "Air"), A_XOR);
	
	// draw the current team on the screen	
	//DrawStr(0,40, (Game_currentTeam ? "Team: Black" : "Team: White") , A_XOR);	
	
	// draw the current worm on the screen
	//char wormStr[20];
	//sprintf(wormStr, "Worm Up: %d", (short)Game_currentWormUp[(short)Game_currentTeam]);
	//DrawStr(0,50, wormStr , A_XOR);	
	
	// draw the camera's position on the screen
	//char camStr[40];
	//sprintf(camStr, "Cam: %d, %d", (short)camX, (short)camY);
	//DrawStr(0,60, camStr , A_XOR);
	
	// draw our free memory on the screen, only when shift is held for debuging
	// if(Keys_keyState(keyCameraControl))
	// {
	// 	char heapStr[40];
	// 	sprintf(heapStr, "%lu", (unsigned long)HeapAvail());
	// 	DrawStr(0,1, heapStr , A_XOR);
	// }
}




/**
 * render a text message to the buffers
 * @param buffer - buffer to draw to
 * @param size - which font size to use
 * @param txt - text to write
 * @param color - which grayscale color to render
 */
short Draw_renderText(unsigned long *buffer, char size, char *txt, char color)
{
	/*
		The characters are stored in a serries of unsigned longs, with 8 characters every 3 rows
		the characters are arranged as so:
		ABCD EFGH
		IJKL MNOP
		QRST UVWX
		YZ01 2345
		6789 -.?
		
		Thus:
			- letters are 0-25
			- numbers are 26-35
			- . is 36
			- - is 37
			- ? is 38
			- space is 39
	*/
	
	// not all characters are 4 bits wide, for proper spacing we need to save all the char widths
	static char widths[] = {	
		3, 2, 2, 3,  3, 3, 4, 3,
		1, 2, 3, 2,  3, 4, 3, 2,
		4, 3, 3, 3,  3, 3, 3, 3,
		3, 3, 3, 1,  3, 3, 3, 3,
		2, 3, 2, 2,  1, 2, 2, 2
	};

	int i;
	
	// save the length of the text
	short len = strlen(txt);

	// as we copy bits to each pixel of the buffers, we will move from 0 left, to right
	// keep track of the pixel we're on
	short pixColumn=0;

	// loop to copy all chars
	for(i=0; i<len; i++)
	{
		// get the char at this index
		char chr = txt[i];
		
		// we need to convert the character to our mapped sprite, space will default for space or unknown chars
		if(chr>=65 && chr<=90)
			chr -= 65;
		else if(chr>=48 && chr<=57)
			chr -= 22;
		else if(chr==45 || chr==47)
			chr -= 9;
		else if(chr==63)
			chr = 38;
		else
			chr = 39;
		
		// get width of this char, so we know how many bits to copy
		char chrWidth = widths[(short)chr];
		
		// get the bit index this char lives at horizontally in the sprites
		char charStartBit = 3+((7-chr%8)*4);
		
		// make a mask starting at this bit:
		unsigned long charBitMask = 1;
		charBitMask = charBitMask<<charStartBit;

		/*
			get the unsigned long in which the top row of the char lives.
			
			Note: every 8 chars take 3 rows of unsigned longs.
		*/
		const unsigned long *charStartUL = spr_tinyFont + (((chr-(chr%8))/8)*3);
	
		// add leading space
		pixColumn++;
		
		// now we loop to copy the chars pixels from its row, to our row and location
		int chrPix, row;
		for(chrPix=0; chrPix<chrWidth; chrPix++)
		{
			// increment the our buffers column
			pixColumn++;
			
			// make a mask for our current pixel column
			unsigned long pixBitMask = 1;
			pixBitMask = pixBitMask<<(31-(pixColumn%32));
			
			// calculate the current long we're on width wise, and get reference to it
			short ulIndex = ((pixColumn-(pixColumn%32))/32)*5+1;
			unsigned long *currentUL = buffer+ulIndex;
			
			// copy the current char row pixels to each buffer row pixels
			for(row=0; row<3; row++)
			{
					// get status of char bit:
					unsigned long chrBit = *(charStartUL+row) & charBitMask;
					
					// set bit in our buffer, based on black or white mode
					if(!color)
						chrBit ? (*(currentUL) |= pixBitMask) : (*(currentUL) &= ~pixBitMask);
					else
						chrBit ? (*(currentUL) &= ~pixBitMask) : (*(currentUL) |= pixBitMask);
						
					currentUL++;
			}// next row			
			
			// move the char mask
			charBitMask = charBitMask>>1;
			
		}// next chrPix
			
	}// next i

	/*
	 	now we need to center the text.
	 	
	 	note: we couldn't have done this earlier, because we needed to convert the chars to our
	 	sizes, and total the length in the main loop above...
	*/
	
	// get total width of our buffer in pixels
	short bufferWidth = size * 32;
	
	// pixColumn should make a fine text width...
	// so lets get the difference and half it. that should be the left position of the text
	short txtLeft = (bufferWidth-pixColumn)/2;
	
	// shift everything over that many pixels
	for(i=0; i<(txtLeft-2); i++)
	{
		int col;
		for(col=size; col>=0; col--)
		{
			// shift all rows int he column
			int row;
			for(row=0; row<3; row++)
			{
				// get pointer to our unsigned long
				unsigned long *current = (buffer + (((col)*5)+(row+1)));
				
				// unsigned long on this row in previous column, or 0 if left-most
				unsigned long leftUL=1;
				leftUL = leftUL<<31;
				if(col-1>=0)
					leftUL = *(current-5);
				
				// get upper bit of previous col on this row
				unsigned long upperBit = leftUL<<31;
	
				// shift our value and move the previous unsigned longs value over
				*current = ((*current)>>1) | upperBit;
					
			}// next row
		}// next col
	}// next i
	
	// return the length of the text
	return (short)pixColumn;
}


/**
 * Renders the sprite to use for team health bar
*/
void Draw_renderTeamHealth()
{
    // 1. Sum Health
    // We use longs to prevent overflow during the *15 calculation later, 
    // though shorts would likely suffice given standard worm health.
    unsigned long hSum1 = 0;
    unsigned long hSum2 = 0;
    int i;

    // Team 1 (Indices 0 to 7)
    // Only iterate up to the active count to save cycles
    for(i = 0; i < Match_wormCount[0]; i++) {
        hSum1 += Worm_health[i];
    }

    // Team 2 (Indices 8 to 15)
    // Offset by MAX_WORMS/2 (assuming 8 is the split based on your snippet)
    // Adjust '8' to whatever your specific team split index is.
    for(i = 0; i < Match_wormCount[1]; i++) {
        hSum2 += Worm_health[8 + i];
    }

    // 2. Calculate Pixels (Integer Math)
    // Formula: (CurrentSum * 15) / MaxSum
    // We calculate MaxSum dynamically to handle different team sizes correctly.
    // We check for divide by zero just in case a team has 0 worms.
    
    unsigned short count1 = Match_wormCount[0];
    unsigned short count2 = Match_wormCount[1];
    
    unsigned short bars1 = 0;
    if(count1) {
        unsigned long max1 = count1 * Match_wormStartHealth;
        bars1 = (hSum1 * 15) / max1; 
        if(bars1 > 15) bars1 = 15; // Clamp for safety
    }

    unsigned short bars2 = 0;
    if(count2) {
        unsigned long max2 = count2 * Match_wormStartHealth;
        bars2 = (hSum2 * 15) / max2;
        if(bars2 > 15) bars2 = 15;
    }

    // 3. Generate Bit Masks
    // Team 1: Left side (Upper 16 bits). Grows left.
    // We take the mask and shift it into the upper word.
    unsigned long team1pixels = (unsigned long)BIT_MASKS[bars1] << 16;

    // Team 2: Right side (Lower 16 bits). Grows right from center.
    // To grow right from bit 15: Shift the mask LEFT by (16 - N).
    // Ex: 1 bit -> 0000...0001 << 15 -> 1000...0000 (Bit 15 set)
    unsigned long team2pixels = (unsigned long)BIT_MASKS[bars2] << (16 - bars2);
	
    // 4. Combine for Planes
    // Dark  = Plane 0 & Plane 1 (Team 1)
    // Light = Plane 1 Only      (Team 2)
    
    // Plane 0 (Dark Plane): Only Team 1 needs to be here.
    unsigned long p0 = team1pixels; 
    
    // Plane 1 (Light Plane): Team 1 (to make it dark) + Team 2 (to make it light)
    unsigned long p1 = team1pixels | team2pixels;

    // 5. Fill Buffers (Unrolled)
    // Direct assignment is faster than a loop for 3 elements
    teamHealthSprites_dark[0] = p0;
    teamHealthSprites_dark[1] = p0;
    teamHealthSprites_dark[2] = p0;

    teamHealthSprites_light[0] = p1;
    teamHealthSprites_light[1] = p1;
    teamHealthSprites_light[2] = p1;
}
