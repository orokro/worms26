/*
	Draw.h
	------
	
	Main header file for the Draw.c module
*/

#ifndef DRAW_H
#define DRAW_H


#include "SpriteData.h"

// Draw defines
#define BTN_CHECK  0b00000001
#define BTN_ACCEPT 0b00000001
#define BTN_CLOSE  0b00000010


// Draw globals
extern unsigned long windSprites[3][3];
extern unsigned long teamHealthSprites_light[3];
extern unsigned long teamHealthSprites_dark[3];
extern unsigned long healthSprites[16][18];
extern unsigned long healthMasks[16][18];

// This is the new RAM buffer (takes 0 bytes of ROM space)
extern unsigned short spr_weapons_flipped[NUM_WEAPONS][11];

// This is the new RAM buffer for flipped worm sprites
extern unsigned short* wormsSpritesFlipped[NUM_WORM_SPRITES];
extern unsigned short wormFlipBuffer[WORM_FLIP_BUFFER_SIZE];
extern unsigned short wormMaskBuffer[WORM_GENERATED_MASK_BUFFER_SIZE];

// if the user fired a gun-type weapon we should draw a line on screen for a frame
extern short ray_sx;
extern short ray_sy;
extern short ray_ex;
extern short ray_ey;
extern char ray_active;

// draw function prototypes

/**
 * @brief converts world coordinates to screen coordinates
 * 
 * @param x a pointer reference to the x value to change to screen coordinates
 * @param y a pointer reference to the y value to change to screen coordinates
 * @return a char boolean either TRUE or FALSE if the coordinates are roughly on screen
*/
extern char worldToScreen(short *x, short *y);


/**
 * Draws the cake loading screen
 *
 * @param amount the current progress out of the current total
 * @param total amount of progress to be completed
*/
extern void Draw_cake(short amount, short total);


/**
 * Renders the map, items on it, and all game elements in general.
 *
 * Renders what is essential for the Game play, including:
 * 	- Map
 * 	- Worms
 * 	- Crates
 * 	- Mines
 * 	- Oil Drums
 * 	- Active Weapons
 * 	- Active particles
 *	- Game timer
 * 	- Wind
 * 	- etc
*/
extern void Draw_renderGame();


/**
 * Draws the games timer
*/
extern void Draw_timer();


/**
 * Draws the games HUD (i.e. timer, wind, etc)
*/
extern void Draw_HUD();


/**
 * Draws the weapons menu for when a player is selecting a weapon.
 * 
 * @param wx a char that contains the x-position of the current weapon selected in the matrix of weapons
 * @param wy a char that contains the y-position of the current weapon selected in the matrix of weapons
*/
extern void Draw_renderWeaponsMenu(char wx, char wy);


/**
 * renders our tiny text font to a serries of unsigned longs
 *
 * @param *buffer a pointer to at least 3 unsigned longs to render the text to
 * @param size the number of unsigned longs this buffer is wide. There should be 3*size unsigned longs in the buffer.
 * @param txt the test to write
 * @param color TRUE for white on black, FALSE for black on white
 * @return the length of the text in pixels
*/
extern short Draw_renderText(unsigned long *buffer, char size, char *txt, char color);


/**
 * renders all the worms name sprites once at the beginning of the game
 *
 * @param the index of the worm to draw the health sprite for
*/
extern void Draw_healthSprite(short index);


/**
 * Renders the sprite to use for team health bar
*/
extern void Draw_renderTeamHealth();


/**
 * @brief Sets up the ray coordinates for drawing
 * 
 * @param sx - start x
 * @param sy - start y
 * @param ex - end x
 * @param ey - end y
 */
extern void Draw_setRayLine(short sx, short sy, short ex, short ey);


/**
 * @brief Draws girder line at given world coords
 * 
 * @param x - world x
 * @param y - world y
 * @param onMap - is this being drawn on the map?
 */
extern void Draw_girder(short x, short y, char onMap);


#endif
