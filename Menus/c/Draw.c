/*
	Draw.c
	------

	I suspect the menu system will have lots of re-usable drawing code in common,
	so I'll put some of the reusable stuff here, but note that a lot of drawing
	happens directly inside the state machine modules.
*/

// includes
#include "Main.h"
#include "Draw.h"
#include "SpriteData.h"


// --------------------------------------------------------------------------------------------------------------------------------------


/**
 * @brief Clears both drawing buffers
 */
void Draw_clearBuffers()
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
}


/**
 * @brief Draws a rectangle outline in the specified color
 * 
 * @param x - x
 * @param y - y
 * @param w - width
 * @param h - height
 * @param color - color code (0-3)
 */
void Draw_RectOutlineColor(short x, short y, short w, short h, short color)
{
    // Calculate right and bottom edges once
    unsigned short r = x + w - 1;
    unsigned short b = y + h - 1;

    // ---------------------------------------------------------
    // 1. HORIZONTAL LINES (Top & Bottom)
    // Use the optimized "2B" function which handles both planes at once.
    // ---------------------------------------------------------
    // Prototype: GrayFastDrawHLine2B_R(plane0, plane1, x1, x2, y, color)
    
    GrayFastDrawHLine2B_R(lightPlane, darkPlane, x, r, y, color);
    GrayFastDrawHLine2B_R(lightPlane, darkPlane, x, r, b, color);

    // ---------------------------------------------------------
    // 2. VERTICAL LINES (Sides)
    // Since VLine2B doesn't exist, we must draw each plane individually.
    // We use FastDrawVLine_R (Register version) for smallest code size.
    // ---------------------------------------------------------

    // -- LIGHT PLANE (Controlled by Bit 0 of color) --
    // If Bit 0 is set, we Draw (A_NORMAL). If 0, we Clear (A_REVERSE).
    short modeL = (color & 1) ? A_NORMAL : A_REVERSE;
    
    FastDrawVLine_R(lightPlane, x, y, b, modeL);
    FastDrawVLine_R(lightPlane, r, y, b, modeL);

    // -- DARK PLANE (Controlled by Bit 1 of color) --
    // If Bit 1 is set, we Draw (A_NORMAL). If 0, we Clear (A_REVERSE).
    short modeD = (color & 2) ? A_NORMAL : A_REVERSE;
    
    FastDrawVLine_R(darkPlane, x, y, b, modeD);
    FastDrawVLine_R(darkPlane, r, y, b, modeD);
}


/**
 * @brief Draws the F1 X and F5 check mark for menus
 * 
 * @param flags - which icons to draw
 */
void Draw_XandCheck(char flags)
{
	// draw close X on left
	if(flags & BTN_CLOSE)
	{		
		ClipSprite16_OR_R(4, 80, 16, spr_Close_Fill, lightPlane);
		GrayClipSprite16_OR_R(4, 80, 16, spr_Close_Outline, spr_Close_Outline, lightPlane, darkPlane);
	}

	// draw accept check on right
	if(flags & BTN_CHECK)
	{
		ClipSprite16_OR_R(140, 76, 20, spr_Check_Fill, lightPlane);
		GrayClipSprite16_OR_R(140, 76, 20, spr_Check_Outline, spr_Check_Outline, lightPlane, darkPlane);
	}
}


/**
 * @brief draws help text at bottom of screen	
 * 
 * @param text - text to draw
 */
void Draw_helpText(const char* text)
{
	// draw help text at bottom of screen
	FontSetSys(F_4x6);
	const short left = (160 - (strlen(text) * 4)) / 2;
	GrayDrawStr2B(left, 86, text, A_NORMAL, lightPlane, darkPlane);
}


/**
 * @brief Draws title text centered at top of screen
 * 
 * @param text - text to draw
 */
void Draw_titleText(const char* text)
{
	// draw title text centered at top of screen
	FontSetSys(F_8x10);
	const short left = (160 - (strlen(text) * 8)) / 2;
	GrayDrawStr2B(left, 3, text, A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(left-1, 2, text, A_XOR, lightPlane, darkPlane);
	FontSetSys(F_4x6);
}


/**
 * @brief Draws one big menu button, with shadow if selected
 * 
 * @param x - x
 * @param y - y
 * @param itemId - item id 
 * @param currentItem - currently selected item id
 * @param sprite - sprite data pointer
 */
void Draw_bigMenuButton(short x, short y, char itemId, char currentItem, const unsigned long* sprite)
{
	// draw menu item sprites, with a shadow for the selected one
	GrayClipSprite32_OR_R(x, y, 21, sprite, sprite, lightPlane, darkPlane);
	if(currentItem == itemId)
	{
		ClipSprite32_OR_R(x, y+1, 21, sprite, lightPlane);
		ClipSprite32_OR_R(x+1, y+1, 21, sprite, lightPlane);
	}
}


/**
 * @brief Draws text box
 * 
 * @param x - x
 * @param y - y
 * @param width - width 
 * @param isSelected - is selected
 * @param text - text to draw
 */
void Draw_textBox(char x, char y, char width, char isSelected, const char* text)
{

	// draw a box for the text box
	Draw_RectOutlineColor((short)x, (short)y, width, 11, isSelected ? 3 : 1);

	// draw the text in the box
	GrayDrawStr2B((short)x+2, (short)y+3, text, A_NORMAL, lightPlane, darkPlane);

	// if we're selected, also draw the inverted box on both planes
	if(isSelected)
	{
		short nameLen = (short)strlen(text) * 4;
		FastFilledRect_Invert_R(lightPlane, (short)x+2, (short)y+2, (short)(x+3+nameLen), (short)(y+8));
		FastFilledRect_Invert_R(darkPlane, (short)x+2, (short)y+2, (short)(x+3+nameLen), (short)(y+8));
	}
}
