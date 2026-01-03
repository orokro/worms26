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
