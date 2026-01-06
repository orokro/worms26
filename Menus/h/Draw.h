/*
	Draw.h
	------

	Main header for the Draw.c file.
*/

#ifndef DRAW_H
#define DRAW_H


// Draw defines

// which buttons to show in the menus (the x or check/accept)
#define BTN_ACCEPT 0b00000001
#define BTN_CLOSE  0b00000010


// draw function prototypes


/**
 * @brief Clears both drawing buffers
 */
void Draw_clearBuffers();


/**
 * @brief Draws a tab bar w/ two named tabs
 * 
 * @param selectedTab - 0 or 1 for which tab is selected
 * @param tab1Name - label for tab 1
 * @param tab2Name - label for tab 2
 */
extern void Draw_tabBar(char selectedTab, const char* tab1Name, const char* tab2Name);


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


/**
 * @brief draws help text at bottom of screen	
 * 
 * @param text - text to draw
 */
extern void Draw_helpText(char y, const char* text);


/**
 * @brief Draws title text centered at top of screen
 * 
 * @param text - text to draw
 */
extern void Draw_titleText(const char* text);


/**
 * @brief Draws one big menu button, with shadow if selected
 * 
 * @param x - x
 * @param y - y
 * @param itemId - item id 
 * @param currentItem - currently selected item id
 * @param sprite - sprite data pointer
 */
void Draw_bigMenuButton(short x, short y, char itemId, char currentItem, const unsigned long* sprite);


/**
 * @brief Draws text box
 * 
 * @param x - x
 * @param y - y
 * @param width - width 
 * @param isSelected - is selected
 * @param text - text to draw
 */
void Draw_textBox(char x, char y, char width, char isSelected, const char* text);


/**
 * @brief Draws a toggle switch
 * 
 * @param x - x
 * @param y - y
 * @param enabled - true if enabled
 */
extern void Draw_toggle(char x, char y, char enabled);

#endif
