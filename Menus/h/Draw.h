/*
	Draw.h
	------

	Main header for the Draw.c file.
*/

#ifndef DRAW_H
#define DRAW_H


// Draw defines

// which buttons to show in the menus (the x or check/accept)
#define BTN_CHECK  0b00000001
#define BTN_ACCEPT 0b00000001
#define BTN_CLOSE  0b00000010


// draw function prototypes


/**
 * @brief Clears both drawing buffers
 */
void Draw_clearBuffers();


/**
 * @brief Draws a rectangle outline in the specified color
 * 
 * @param x - x
 * @param y - y
 * @param w - width
 * @param h - height
 * @param color - color code (0-3)
 */
void Draw_RectOutlineColor(short x, short y, short w, short h, short color);


/**
 * @brief Draws the F1 X and F5 check mark for menus
 * 
 * @param flags - which icons to draw
 */
extern void Draw_XandCheck(char flags);

#endif
