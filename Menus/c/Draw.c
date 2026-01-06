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
#include "State.h"


// --------------------------------------------------------------------------------------------------------------------------------------


/**
 * @brief Clears both drawing buffers
 */
void Draw_clearBuffers()
{
	FastClearScreen_R(lightPlane);
	FastClearScreen_R(darkPlane);
}


/**
 * @brief Draws a tab bar w/ two named tabs
 * 
 * @param selectedTab - 0 or 1 for which tab is selected
 * @param tab1Name - label for tab 1
 * @param tab2Name - label for tab 2
 */
void Draw_tabBar(char selectedTab, const char* tab1Name, const char* tab2Name)
{
	// black line across bottom
	GrayDrawClipLine2B(0, 88, 135, 88, 3, lightPlane, darkPlane);

	// skip the first row of of the tab sprite and draw it twice on the light plane
	ClipSprite32_OR_R(2, 89, 10, spr_TeamTab+1, lightPlane);
	ClipSprite32_OR_R(20, 89, 10, spr_TeamTabR+1, lightPlane);

	ClipSprite32_OR_R(51, 89, 10, spr_TeamTab+1, lightPlane);	
	ClipSprite32_OR_R(69, 89, 10, spr_TeamTabR+1, lightPlane);

	// draw the selected tab on the dark plane
	short tabX = (short)(2 + (selectedTab * 49));
	GrayClipSprite32_AND_R(tabX, 88, 11, spr_TeamTab, spr_TeamTab, lightPlane, darkPlane);
	GrayClipSprite32_AND_R(tabX+18, 88, 11, spr_TeamTabR, spr_TeamTabR, lightPlane, darkPlane);
	GrayClipSprite32_OR_R(tabX, 88, 11, spr_TeamTab, spr_TeamTab, lightPlane, darkPlane);
	GrayClipSprite32_OR_R(tabX+18, 88, 11, spr_TeamTabR, spr_TeamTabR, lightPlane, darkPlane);

	// tiny font draw tab labels
	FontSetSys(F_4x6);
	GrayDrawStr2B(5, 91, tab1Name, A_NORMAL, darkPlane, darkPlane);
	GrayDrawStr2B(54, 91, tab2Name, A_NORMAL, darkPlane, darkPlane);
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
		const char offset = ((State_transitionTime>0) && (State_transitionButton & BTN_CLOSE)) ? 2 : 0;
		ClipSprite16_OR_R(4+offset, 80+offset, 16, spr_Close_Fill, lightPlane);
		GrayClipSprite16_OR_R(4+offset, 80+offset, 16, spr_Close_Outline, spr_Close_Outline, lightPlane, darkPlane);
	}

	// draw accept check on right
	if(flags & BTN_ACCEPT)
	{
		const char offset = ((State_transitionTime>0) && (State_transitionButton & BTN_ACCEPT)) ? 2 : 0;
		ClipSprite16_OR_R(140+offset, 76+offset, 20, spr_Check_Fill, lightPlane);
		GrayClipSprite16_OR_R(140+offset, 76+offset, 20, spr_Check_Outline, spr_Check_Outline, lightPlane, darkPlane);
	}
}


/**
 * @brief draws help text at bottom of screen	
 * 
 * @param y - y position
 * @param text - text to dra
 */
void Draw_helpText(char y, const char* text)
{
	// draw help text at bottom of screen
	FontSetSys(F_4x6);
	const short left = (160 - DrawStrWidth(text, F_4x6)) / 2;
	GrayDrawStr2B(left, y, text, A_NORMAL, lightPlane, darkPlane);
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
	// clear under box
	GrayFastFillRect_R(lightPlane, darkPlane, (short)x, (short)y, (short)(x+width), (short)(y+11), 0);

	// draw a box for the text box
	Draw_RectOutlineColor((short)x, (short)y, width, 11, isSelected ? 3 : 1);

	// draw the text in the box
	GrayDrawStr2B((short)x+2, (short)y+3, text, A_NORMAL, lightPlane, darkPlane);

	// if we're selected, also draw the inverted box on both planes
	if(isSelected)
	{
		short nameLen = DrawStrWidth(text, F_4x6);
		FastFilledRect_Invert_R(lightPlane, (short)x+2, (short)y+2, (short)(x+2+nameLen), (short)(y+8));
		FastFilledRect_Invert_R(darkPlane, (short)x+2, (short)y+2, (short)(x+2+nameLen), (short)(y+8));
	}
}


/**
 * @brief Draws a toggle switch
 * 
 * @param x - x
 * @param y - y
 * @param enabled - true if enabled
 */
void Draw_toggle(char x, char y, char enabled)
{	
	// draw black if selected
	if(enabled)
		GrayClipSprite16_OR_R(x, y, 8, spr_Toggle, spr_Toggle, lightPlane, darkPlane);
	else
		GrayClipSprite16_OR_R(x, y, 8, spr_Toggle+8, spr_Toggle+8, lightPlane, lightPlane);
}
