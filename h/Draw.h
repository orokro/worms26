#ifndef DRAW_H
#define DRAW_H

/* ======================================================================================================================================
   DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++
   ====================================================================================================================================== */

// Draw defines

// Draw globals
extern unsigned long windSprites[3][3];
extern unsigned long healthSprites[16][18];
extern unsigned long healthMasks[16][18];

// draw function prototypes

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
 * Draws the pause menu, with current menu item selected.
 * 
 * The pause menu has three options: continue, quit, show controls.
 * This draws the pause menu instead of the current Game.
 * The pause menu will highlight whichever option is selected.
 *
 * @param menuItem the index of the menu item that is currently selected (continue, quit, show controls)
*/
extern void Draw_renderPauseMenu(char);

/**
 * Draws the weapons menu for when a player is selecting a weapon.
 * 
 * @param wx a char that contatins the x-position of the current weapon selected in the matrix of weapons
 * @param wy a char that contatins the y-position of the current weapon selected in the matrix of weapons
*/
extern void Draw_renderWeaponsMenu(char, char);

/**
 * renders our tiny text font to a serrirs of unsigned longs
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

#endif