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


/**
 * draw falling leave wind and cloud speed indicators
*/
void drawLeavesAndClouds()
{
	short i;
	
	// use smaller wind for smoother sprites
	short wind = Game_wind/4;
	
	// no need for external data, only on leaf at a time so we can keep it static
	static short Leaf_x[5] = {10, 30, 60, 90, 120};
	static short Leaf_y[5] = {10, 30, 60, 90, 120};
	
	// only 3 clouds at a time, keep it static:
	static short Cloud_x[] = {0, 53, 106};
	static short Cloud_y[] = {-50, -70, -90};
	
	for(i=0; i<5; i++)
	{
		// leaves always move down at a constant rate, and sideways by the wind rate
		Leaf_y[i]++;
		Leaf_x[i]+=wind;
		
		// get leafs onscreen pos
		short xPos = Leaf_x[i];
		short yPos = Leaf_y[i];
		
		// draw if if on screen
		if(worldToScreen(&xPos, &yPos))
			ClipSprite16_OR_R(xPos, yPos, 8, spr_Leaf, lightPlane);
			
		// or respawn if off screen and far enough below
		else if(yPos>80)
		{
			if(abs(Game_wind)<5 || random(2)==0)
			{
				Leaf_x[i] = camX+random(160)-80;
				Leaf_y[i] = camY-58;
			}else
			{
				Leaf_x[i] = camX + ((Game_wind<0) ? 90 : -90);
				Leaf_y[i] = camY-61 + random(70);
			}
		}		
	} // next l
	
	// loop thru clouds and draw or respawn
	for(i=0; i<3; i++)
	{
		// clouds only move horizontally
		Cloud_x[i] += wind;
		
		// get positions to mutate
		short xPosC = Cloud_x[i];
		short yPosC = Cloud_y[i];
		
		char onScreen = worldToScreen(&xPosC, &yPosC);
		
		// check screen pos:
		if(onScreen)
		{
			// draw the cloud:
			ClipSprite32_MASK_R(xPosC, yPosC, 14, spr_Cloud_Light, spr_Cloud_Mask, lightPlane);
			ClipSprite32_MASK_R(xPosC, yPosC, 14, spr_Cloud_Dark, spr_Cloud_Mask, darkPlane);
		}else if(xPosC < -32 && wind<0)
		{
			Cloud_x[i] = camX+80+31;
			Cloud_y[i] = -50-random(50);
		}else if(xPosC > 160+32 && wind>0)
		{
			Cloud_x[i] = camX-80-31;
			Cloud_y[i] = -50-random(50);
		}
	}// next i
}


/**
 * Draws the X picked in arrow mode
 */
void drawCursorAndXSpot()
{	
	// GTFO if our current weapon doesn't use cursors anyway
	if((Game_currentWeaponProperties & usesCursor)==0)
		return;
	
	short screenX = Game_cursorX;
	short screenY = Game_cursorY;
	
	// if we're actually in cursor mode draw the cursor on screen
	if(Game_mode==gameMode_Cursor)
	{		
		if(worldToScreen(&screenX, &screenY))
		{							
			const unsigned char* cursorOutlineSprite = (Game_currentWeaponState & strikeLeft) ? spr_CursorOutline : spr_CursorOutlineFlipped;
			const unsigned char* cursorFillSprite = (Game_currentWeaponState & strikeLeft) ? spr_CursorFill : spr_CursorFillFlipped;
			
			GrayClipSprite8_AND_R(screenX, screenY, 12, cursorOutlineSprite, cursorOutlineSprite, lightPlane, darkPlane);
			GrayClipSprite8_OR_R(screenX, screenY, 12, cursorFillSprite, cursorFillSprite, lightPlane, darkPlane);
		}
	}
	
	// GTFO if we don't have an x placed
	if((Game_currentWeaponState & targetPicked)==0)
		return;
		
	// convert to screen space & draw the sprites
	screenX = Game_xMarkSpotX;
	screenY = Game_xMarkSpotY;
	if(worldToScreen(&screenX, &screenY))
	{							
		GrayClipSprite8_AND_R(screenX, screenY, 8, spr_XSpotMask, spr_XSpotMask, lightPlane, darkPlane);
		GrayClipSprite8_OR_R(screenX, screenY, 8, spr_XSpot, spr_XSpot+8, lightPlane, darkPlane);

	}// end if on scree
}


/**
 * Draws either the selection arrow, or current worm arrow in Worm Select mode.
*/
void drawSelectArrow()
{
	static char frame=0;
	frame++;
	if(frame>7)
		frame=0;
	
	short x=Worm_x[(short)Worm_currentWorm]-9;
	short y=Worm_y[(short)Worm_currentWorm]-32;
	
	if(worldToScreen(&x, &y))
	{
		// take advantage of extgrah's sprite method to handle bit shifting and mem copying in one swoop!
		ClipSprite16_XOR_R(x, y, 16, ((frame<4) ? spr_SelectionArrowFrame1 : spr_SelectionArrowFrame2), darkPlane);
		ClipSprite16_XOR_R(x, y, 16, ((frame<4) ? spr_SelectionArrowFrame1 : spr_SelectionArrowFrame2), lightPlane);	
	}
}


/**
 * Draws a grayscale parallax mountain range in the background.
 */
void drawMountains()
{
	short i;
	short offsetX = (camX-160)/5;
	short offsetY = camY/5;
	for(i=0; i<5; i++)
		ClipSprite32_OR_R(i*32-offsetX, 60-offsetY, 38, spr_Mountain[i], lightPlane);

}


/**
 * Draws the games timer
*/
void drawTimer()
{
	// only draw the timer if the game is in select or turn mode!
	if(Game_timer>0 && (Game_mode==gameMode_WormSelect || Game_mode==gameMode_Turn || Game_mode==gameMode_Cursor || Game_mode==gameMode_WeaponSelect ||  Game_mode==gameMode_Pause))
	{
		// erase space for timer
		ClipSprite16_AND_R(2, 87, 11, spr_timerMask, lightPlane);
		ClipSprite16_AND_R(2, 87, 11, spr_timerMask, darkPlane);
		
		// if we have trace time, blink the border:
		if(Game_graceTimer>0 && (Game_graceTimer/3)%6<3)
		{
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, lightPlane);
		}else
		{
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, lightPlane);
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, darkPlane);
		}
		
		// draw the time in the turn:
		short time = (Game_graceTimer>0) ? (short)(Game_graceTimer/TIME_MULTIPLIER) : (short)(Game_timer/TIME_MULTIPLIER);
		char txt[3];
		sprintf(txt, "%d", time);
		
		GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
		DrawStr(3 + ((time<10) ? 3 : 0), 89, txt, A_NORMAL);
		GrayDBufSetHiddenAMSPlane(DARK_PLANE);
		DrawStr(3 + ((time<10) ? 3 : 0), 89, txt, A_NORMAL);
		
		// if the game timer is less than 5, the inside, not the border
		if((short)(Game_timer/TIME_MULTIPLIER)<=5 && (Game_timer/3)%6<3)
			ClipSprite16_XOR_R(2, 87, 11, spr_timerBlink, darkPlane);
		
	}// end if draw timer	
}


/**
 * Draws the games HUD (i.e. timer, wind, etc)
*/
void drawHUD()
{		
	// if the user has a weapon selected and the current mode is TURN, draw it's icon in the top right:
	if(Game_currentWeaponSelected!=-1 && Game_mode==gameMode_Turn)
	{
		// draw the box sprite for the selected weapon
		ClipSprite16_AND_R(2, 2, 13, spr_selectedWeaponBoxMask, lightPlane);
		ClipSprite16_AND_R(2, 2, 13, spr_selectedWeaponBoxMask, darkPlane);
		ClipSprite16_OR_R(2, 2, 13, spr_selectedWeaponBox, lightPlane);
		ClipSprite16_OR_R(2, 2, 13, spr_selectedWeaponBox, darkPlane);
		
		// draw the icon for the selected weapon
		ClipSprite16_OR_R(3, 3, 11, spr_weapons[(short)Game_currentWeaponSelected], lightPlane);
		ClipSprite16_OR_R(3, 3, 11, spr_weapons[(short)Game_currentWeaponSelected], darkPlane);
	}
	
	// copy the wind bar to both buffers:
	ClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, lightPlane);
	ClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, darkPlane);
	ClipSprite32_OR_R(126, 93, 5, spr_WindMeter, lightPlane);
	ClipSprite32_OR_R(126, 93, 5, spr_WindMeter, darkPlane);

	// GrayClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, spr_WindMeter_Mask, lightPlane, darkPlane);
	// GrayClipSprite32_OR_R(126, 93, 5, spr_WindMeter, spr_WindMeter, lightPlane, darkPlane);
	
	// current frame of animation:
	static char frame=0;
	if(++frame>33)
		frame=0;
	
	// draw current frame of animated bar
	ClipSprite32_OR_R(127, 94, 3, windSprites[(short)((frame/3)%3)], darkPlane);
	
	// reuse the wind bar in the center of the screen to draw the team health:
	// GrayClipSprite32_AND_R(64, 93, 5, spr_WindMeter_Mask, spr_WindMeter_Mask, lightPlane, darkPlane);
	// GrayClipSprite32_OR_R(64, 93, 5, spr_WindMeter, spr_WindMeter, lightPlane, darkPlane);
	ClipSprite32_AND_R(64, 93, 5, spr_WindMeter_Mask, lightPlane);
	ClipSprite32_AND_R(64, 93, 5, spr_WindMeter_Mask, darkPlane);
	ClipSprite32_OR_R(64, 93, 5, spr_WindMeter, lightPlane);
	ClipSprite32_OR_R(64, 93, 5, spr_WindMeter, darkPlane);

	// // draw the health for both teams:
	GrayClipSprite32_OR_R(65, 94, 3, teamHealthSprites_light, teamHealthSprites_dark, lightPlane, darkPlane);

	// draw the timer
	drawTimer();	
}


/**
 * draws the water infront of the map / objects
*/
void drawWater()
{
	// frame and frame timer
	static char frame=0;
	static char frameTimer=0;
	if(++frameTimer>=2)
	{
		frameTimer=0;
		if(++frame>7)
			frame=0;
	}		
	
	static char frames[] = {0, 1, 2, 3, 4, 3, 2, 1};
	short f = frames[(short)frame];
	
	short waterY = 241-camY - Game_waterLevel;
	short i;
	for(i=0; i<7; i++)
	{
		short waterX = i*32 - (camX+328)%32;
	
		// draw just the and dark light planes:
		ClipSprite32_MASK_R(waterX, waterY, 9, &spr_Water_Dark[0+(f*9)], &spr_Water_Mask[0+(f*9)], darkPlane);
		ClipSprite32_AND_R(waterX, waterY, 9, &spr_Water_Mask[0+(f*9)], lightPlane);
		ClipSprite32_OR_R(waterX, waterY, 9, &spr_Water_Light[0+(f*23)], lightPlane);
	}

	// draw the current worms y position on screen
	//const short currentWormY = Worm_y[(short)Worm_currentWorm];
	//static char buffer[32];
	//sprintf(buffer, "Worm Y: %d", currentWormY);
	//GrayDrawStr2B(50, 1, buffer, A_XOR, lightPlane, darkPlane);

	// if the top of the under-area is offscreen, no need to draw the rest
	if((waterY + 9) >= 99)
		return;

	// draw the water rectangle to fill in below the waterline
	SCR_RECT waterRect = {{0, waterY + 9, 159, 99}};

	// Draw Light Gray: LightPlane = 1 (Black), DarkPlane = 0 (White)
	PortSet(lightPlane, 239, 127);
	ScrRectFill(&waterRect, &fullScreen, A_NORMAL);  // Force 1s
	PortRestore();
	PortSet(darkPlane, 239, 127);
	ScrRectFill(&waterRect, &fullScreen, A_REVERSE);  // Force 0s
	PortRestore();
}


/**
 * Draw's the cross hair and/or charge bar for the selected weapon
*/
void drawWeaponDetails()
{
	short i, x, y;

	// get the direction our current worm is facing
	char facingLeft = (Worm_dir & (unsigned short)1<<(Worm_currentWorm))>0;
	
	// get the x/y position of the current worm
	short wormX = Worm_x[(short)Worm_currentWorm];
	short wormY = Worm_y[(short)Worm_currentWorm];
	
	// the center is slightly off center
	if(facingLeft)
			wormX-=4;
			
	// if the weapon requires aiming or charge
	if(
		(Game_currentWeaponProperties & usesAim)
		||
		(Game_currentWeaponProperties & usesCharge)
		)
	{
	
		/*
			get the x/y components to use for the crosshair or the charge
			
			Charge is stored as a value between 0 and 18 (19 positions, using center twice)
			but our array only has 10 values... so we gotta pick values from this.
			
			The first value represents position 18, so the first 10 are:
				19-(Game_aimAngle-9), which would be 0-9 in the array
				
			Therefore, if the Game_aimAngle is less than 9, we want to use:
				Game_aimAngle, but flip the y component.
				
			The x component will be flipped if facing left
		*/
		char xComponent = Weapon_aimPosList[(Game_aimAngle<9) ? Game_aimAngle : 9-(Game_aimAngle-9)][0];
		char yComponent = Weapon_aimPosList[(Game_aimAngle<9) ? Game_aimAngle : 9-(Game_aimAngle-9)][1];
		
		if(Game_aimAngle>=10)
			yComponent *= -1;
		if(facingLeft)
			xComponent *= -1;
		
		// get screen pos
		x=wormX+xComponent;
		y=wormY+yComponent+4;
		worldToScreen(&x, &y);

		// if the weapon is being charged, draw the charge circles
		if(Game_currentWeaponCharge>0)
		{
			/*
				Drawing the charge:
				
				We want to draw in reverse order, from closer to the cross hairs
				to the worm, so the sprites overlap nicely.
				
				Every other sprite will will alternate white and black.
				
				We have 9 total positions:
					- 3 large
					- 3 med
					- 3 small
					
				The charge total goes between 0 and 255, so if we integer divide by 31,
				we should get a value between 0-8
				
				we need to interpolate the sprites positions from the center of the worm
				to the target on the steps 0-8
				
				Note that x and y are already converted to screen coords, so for this
				we'll just convert the worms coords to screen and work just in screen
			*/
			
			// get worms screen pos (worldToScreen mutates)
			short wx = wormX;
			short wy = wormY+4;
			worldToScreen(&wx, &wy);
			
			// list of sprites based on size from 0-2
			const char *spritePtrs[] = {spr_chargeSmall, spr_chargeMed, spr_chargeLarge};
			
			// calc the charge amount in our scale
			char chargeAmt = (Game_currentWeaponCharge/31);
	
			// loop from furthest to nearest
			for(i=chargeAmt; i>=0; i--)
			{
				// calc the scale index we're in
				char scaleIdex = (i/3);
				
				// calculate the size of the sprite for this step
				char spriteHeight = 4 + (2*scaleIdex);
				
				// interpolate sprite position and offset:
				short sx = wx + (x-wx)*(float)((float)i/8.0f) - (spriteHeight/2);
				short sy = wy + (y-wy)*(float)((float)i/8.0f) - (spriteHeight/2);
				
				// on even frames we draw dark gray
				if(i%2==0)
				{
					ClipSprite8_AND_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex]+spriteHeight, lightPlane);
					ClipSprite8_OR_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex], darkPlane);
				}else
				{
					ClipSprite8_OR_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex], lightPlane);
					ClipSprite8_AND_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex]+spriteHeight, darkPlane);
				}
				
			
				// draw sprite based on size
			}// next i
		
		}// end if has charge
		
		// draw the crosshair last so they're always on top of the charge
		ClipSprite8_OR_R(x-4, y-4, 8, spr_CrossHair, lightPlane);
		ClipSprite8_OR_R(x-4, y-4, 8, spr_CrossHair, darkPlane);
		
	}// end if uses aim or charge
	
	// if the weapon needs to draw itself being held, do that now
	if(Game_currentWeaponProperties & holdsSelf)
	{

		const unsigned short* heldSprite = facingLeft ? spr_weapons_flipped[(short)Game_currentWeaponSelected] : spr_weapons[(short)Game_currentWeaponSelected];
		short wx = facingLeft ? wormX-14 : wormX+2;
		short wy = wormY-5;
		worldToScreen(&wx, &wy);
		
		ClipSprite16_OR_R(wx, wy, 11, heldSprite, lightPlane);
		ClipSprite16_OR_R(wx, wy, 11, heldSprite, darkPlane);
					
	}else if(Game_currentWeaponProperties & holdsLauncher)
	{

		short wx = facingLeft ? wormX-2 : wormX-2;
		short wy = wormY-1;
		worldToScreen(&wx, &wy);
		
		GrayClipSprite8_AND_R(wx, wy, 5, spr_launcher, spr_launcher, lightPlane, darkPlane);
		GrayClipSprite8_OR_R(wx, wy, 5, spr_launcher, spr_launcher, lightPlane, darkPlane);
					
	}else if((Game_currentWeaponProperties & usesCursor) && (Game_currentWeaponState & targetPicked)==0)
	{

		short wx = facingLeft ? wormX-2 : wormX-2;
		short wy = wormY-2;
		worldToScreen(&wx, &wy);
		
		// CORRECT: 6 arguments
		GrayClipSprite8_AND_R(wx, wy, 8, spr_remoteMask, spr_remoteMask, lightPlane, darkPlane);
		GrayClipSprite8_OR_R(wx, wy, 8, spr_remote, spr_remote, lightPlane, darkPlane);
					
	}
}



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * draws cake loading screen
 * @param amount - progress
 * @param total - total to calculate percentage
 */
void Draw_cake(short amount, short total)
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	ClipSprite32_OR_R(64, 38, 23, spi_CandleCake_Dark, darkPlane);
	ClipSprite32_OR_R(64, 38, 23, spi_CandleCake_Light, lightPlane);	
	
	char progStr[40];
	sprintf(progStr, "Loading... %d%%", (short)((float)((float)amount/(float)total)*100));
	DrawStr(40,70, progStr, A_NORMAL);	

	// now flip the planes
	GrayDBufToggleSync();
	
	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
}


/**
 * main drawing routine for the game, e.g. map, worms, weapons, etc
 */
void Draw_renderGame()
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
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
	
	// HUD stuff is drawn last since it needs to go on top of all game elements
	drawHUD();
	
	// Draw status bar messages
	// StatusBar_draw();
	
	// if the game mode is worm select, draw the selection arrow
	if(Game_mode==gameMode_WormSelect)
		drawSelectArrow();
	
	// extra stuff to draw if the worm has certain weapons selected
	if(Game_currentWeaponSelected!=-1)
		drawWeaponDetails();
		
	// for now, we will output a bunch of debug info on the screen
	
	// game modes by name	{"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	// static const char modes[9][16] = {"Select", "Turn", "", "", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// draw the current and previous game mode on the screen
	// DrawStr(60,1,modes[(short)Game_mode], A_XOR);

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
 * main drawing routine for the pause menu
 * @param menuItem - which menu item to be selected
 */
void Draw_renderPauseMenu(char menuItem)
{
	// draw the game as normal..
	Draw_renderGame();
	
	// make it darker
	FastFillRect(darkPlane, 0, 0, 159, 99, A_NORMAL);
	
	// draw hud normally
	drawHUD();

	// menu item text
	char continueStr[] = " Continue ";
	char exitStr[] = " Exit ";
	
	// if selected, add markers
	if(menuItem==0)
	{
		continueStr[0] = 26;
		continueStr[9] = 25;
	}else
	{
		exitStr[0] = 26;
		exitStr[5] = 25;
	}
	
	// draw menu options
	GrayDrawStr2B(50, 20, continueStr, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(50, 20, continueStr, A_XOR, lightPlane, darkPlane);
	GrayDrawStr2B(62, 30, exitStr, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(62, 30, exitStr, A_XOR, lightPlane, darkPlane);
	
	// draw title
	FontSetSys(2);
	
	// dark plane is solid, so we can XOR the shadow area to make it WHITE on the dark plane
	GrayDrawStr2B(57, 2, "Paused", A_XOR, darkPlane, darkPlane);
	
	// draw shadow JUST on light plane
	GrayDrawStr2B(57, 2, "Paused", A_NORMAL, lightPlane, lightPlane);
	
	// draw it to both planes in black, then XOR it to make it WHITE
	GrayDrawStr2B(56, 1, "Paused", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(56, 1, "Paused", A_XOR, lightPlane, darkPlane);
	
	FontSetSys(1);
	
	/*
		NOTE:
		
		I was gonna draw control info below the options, but it runs too slow.
		
		Perhaps I'll use sprites one day if there's space.
	*/
	
	/*
	// controls text:
	char controlsStr[] = "[2nd] confirm / use weapon\n[?] jump\n[a] back flip\n[?]+[?]/[?]/[?]/[?] move camera\n[F1]/[CAT] weapons menu\n[APPS] select worm (if enabled)\n[ESC] pause\n[1]/[2]/[3]/[4]/[5] fuse length / opts";
	
	// use tiny font to draw controls
	FontSetSys(0);

	// draw dark on light plane, then xor to erase
	GrayDrawStr2B(10, 30, controlsStr, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(10, 30, controlsStr, A_XOR, lightPlane, darkPlane);
	
	FontSetSys(1);
	*/
	
}


/**
 * main drawing routine for the weapons menu
 * @param wx - selected weapon x grid pos
 * @param wy - selected weapon y grid pos
 */
void Draw_renderWeaponsMenu(char wx, char wy)
{
	short x, y;
	
	// clear both our buffers
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	// draw title with shadow
	FontSetSys(2);
	GrayDrawStr2B(54, 2, "Weapons", A_NORMAL, lightPlane, lightPlane);
	GrayDrawStr2B(53, 1, "Weapons", A_NORMAL, lightPlane, darkPlane);
	FontSetSys(1);
	
	// draw light and dark grid lines
	// for(x=1; x<14; x++)
	//		GrayFastDrawLine2B(0+(x*12), 13, 0+(x*12), 73, 1, lightPlane, darkPlane);
	//for(y=1; y<6; y++)
	//		GrayFastDrawHLine2B(1, 156, 12+(y*12), 1, lightPlane, darkPlane);
	
	// draw black grid lies
	// for(x=0; x<14; x++)
	// 	GrayFastDrawLine2B(1+(x*12), 13, 1+(x*12), 73, 3, lightPlane, darkPlane);
	// for(y=0; y<6; y++)
	// 	GrayFastDrawHLine2B(1, 156, 13+(y*12), 3, lightPlane, darkPlane);
	
	// draw drop shadow for weapons box (also to make it "centered" perfectly
	// GrayFastDrawHLine2B(2, 157, 74, 1, lightPlane, darkPlane);
	// GrayFastDrawLine2B(158, 14, 158, 74, 1, lightPlane, darkPlane);
	
	// get the weapon ID at this POS
	short weapID = Game_weapInventory[(short)wy][(short)wx];
	
	// if we have the weapon in our inventory...
	if(weapID!=-1)
	{
		// copy name to buffer with extra bit for null termination
		char nameBuffer[32] = "";
		memcpy(nameBuffer, weaponNames[weapID], 15);
		nameBuffer[16] = 0;
		
		// the amount of the weapon
		char countStr[8];
		sprintf(countStr, " (x%d)", Match_teamWeapons[(short)Game_currentTeam][weapID]);
		strcat(nameBuffer, countStr);
		
		// draw the name of the currently selected weapon and its stock count
		GrayDrawStr2B(1, 78, nameBuffer, A_NORMAL, lightPlane, darkPlane);
	}
	
	// loop over our available weapons and draw their sprites, only on dark plane
	for(x=0; x<13; x++)
	{
		for(y=0; y<5; y++)
		{
			// weapon index in this visual slot
			short weapID = Game_weapInventory[y][x];
			
			// if its a valid weapon, draw it's sprite
			if(weapID!=-1)
			{
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], lightPlane);
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weapons[weapID], darkPlane);
			}
			else
				ClipSprite16_OR_R(2+(x*12), 14+(y*12), 11, spr_weaponSelect, lightPlane);

		}// next y
	}// next x
			
	// draw the current cursor location
	ClipSprite16_XOR_R(2+(wx*12), 14+(wy*12), 11, spr_weaponSelect, lightPlane);
	ClipSprite16_XOR_R(2+(wx*12), 14+(wy*12), 11, spr_weaponSelect, darkPlane);
	
	// draw instructions
	GrayDrawStr2B(19, 89, "[2nd] Select [ESC] Exit", A_NORMAL, lightPlane, lightPlane);
	
	// draw the current timer so the user can see time while picking a weapon
	drawTimer();
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
