// C Source File
// Created 11/11/2017; 11:34:12 PM

#include "Main.h"


#include "SpriteData.c"
  
/*
	Draw
	----
	
	Here we define our global drawing routines, for drawing each kind of scene:
		- Main game
		- Pause Menu
		- Weapons Menu
*/

// Global Variables (Defined in Game.c)
extern char Game_wormAnimState;    // Current animation state (0=None, 1=Jump, 2=Backflip)
extern int  Game_wormAnimTimer;    // Timer to track animation progress
extern char Game_wormFlipStartDir; // Direction worm was facing when flip started (0=Right, Mask=Left)

short foo[5][10];

// these sprites will be generated at the begining of the each turn to match the current wind conditions
unsigned long windSprites[3][3];

// these sprites will be genreated at the beginning and when health changes.
unsigned long healthSprites[16][18];
unsigned long healthMasks[16][18];
unsigned long healthLightGray[16][18];

// static weapon names delcaration since we only really need the names in this draw method
// using flat ID space
static const char weaponNames[65][16] = {
	// row 1
	"Jetpack",
	"Bazooka",
	"Grenade",
	"Shotgun",
	"Fire Punch",
	"Dynamite",
	"Air Strike",
	"Blow Torch",
	"Ninja Rope",
	"Sup. Banana Bomb",
	"Petrol Bomb",
	"Mad Cows",
	"Skip Go",
	
	// row 2
	"Low Gravity",
	"Homing Missile",
	"Cluster Bomb",
	"Handgun",
	"Dragon Ball",
	"Mine",
	"Napalm Strike",
	"Pneumatic Drill",
	"Bungee",
	"Holy Hand G'nade",
	"Skunk",
	"Old Woman",
	"Surrender",
	
	// row 3
	"Fast Walk",
	"Mortar",
	"Banana Bomb",
	"Uzi",
	"Kamikaze",
	"Sheep",
	"Mail Strike",
	"Girder",
	"Parachute",
	"Flame Thrower",
	"Ming Vase",
	"Concrete Donkey",
	"Select Worm",
	
	// row 4
	"Laser Sight",
	"Homing Pigeon",
	"Battle Axe",
	"Minigun",
	"Suicide Bomber",
	"Super Sheep",
	"Mine Strike",
	"Baseball Bat",
	"Teleport",
	"Salvation Army",
	"Sheep Strike",
	"Nuclear Test",
	"Freeze",
	
	// row 5
	"Invisibility",
	"Sheep Launcher",
	"Earthquake",
	"Longbow",
	"Prod",
	"Mole Bomb",
	"Mole Squadron",
	"Girder Pack",
	"ScalesOfJustice",
	"MB Bomb",
	"Carpet Bomb",
	"Armageddon",
	"Magic Bullet"	
};

// buffers for our weapons screen
void *weaponsLight, *weaponsDark;



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Convents world coordinates to screen coordinates, mutates the passed in pointers, and returns TRUE/FLASE if on screen (roughly).
 * 
 * The game is updated in world coordinates, but when it comes time to draw,
 * everything needs to be in screen coordinates.
 *
 * This method takes in the Camera's X/Y position and adjusts the passed in X/Y pointers accordingly.
 * 
 * If the final X/Y screen position determined is out of bounds for the screen, the method will return FALSE.
 * Otherwise, TRUE.
 *
 * The current bounding for the screen is +/- 8 pixels horizzontal on each side of the screen, and
 * +/- 16 pixels vertically.
 *
 * This may need to be changed, as the final sprites are decided
 *
 * @param x a pointer reference to the x value to change to screen coordinates
 * @param y a poitner reference to the y value to change to screen coordinates
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
	Draws all the in-game, on-screen Worms.
*/
void drawWorms()
{
    short screenX, screenY;
    short i;

    // Pointers for the sprite data to draw this frame
    unsigned short* sprOutline;
    unsigned short* sprMask;
    short sprHeight; // New: sprites have different heights now!

    // Loop over all worms
    for(i=0; i<MAX_WORMS; i++)
    {
        // Check active bitmask
        if(Worm_active & (unsigned short)1<<(i))
        {
            screenX = Worm_x[i];
            screenY = Worm_y[i];

            // Only draw if on screen
            if(worldToScreen(&screenX, &screenY))
            {
                // Align sprite (centered x, feet y adjustment)
                short x = screenX - 8;
                short y = screenY - 6;

                // Default to standard height
                sprHeight = 13; 

                // ============================================================
                // SPRITE SELECTION LOGIC
                // ============================================================
                
                // Is this the Active Worm doing an Animation?
                if(i == Worm_currentWorm && Game_wormAnimState != ANIM_NONE)
                {
                    // --- BACKFLIP ---
                    if(Game_wormAnimState == ANIM_BACKFLIP)
                    {
                        Game_wormAnimTimer++; // Advance frame
                        
                        // Determine Stage
                        int stage = 1;
                        if(Game_wormAnimTimer < 6)       stage = 1; 
                        else if(Game_wormAnimTimer < 8)  stage = 2; 
                        else if(Game_wormAnimTimer < 9)  stage = 3; 
                        else if(Game_wormAnimTimer < 12) stage = 4; 
                        else                             stage = 1; 

                        // Handle Directions (Start Dir 0=Right, >0=Left)
                        if(Game_wormFlipStartDir == 0) 
                        {
                            // Started facing RIGHT
                            if(stage==1) { sprOutline=spr_WormFlip1_Right_Outline; sprMask=spr_WormFlip1_Right_Mask; sprHeight=17; }
                            else if(stage==2) { sprOutline=spr_WormFlip2_Right_Outline; sprMask=spr_WormFlip2_Right_Mask; sprHeight=10; }
                            else if(stage==3) { sprOutline=spr_WormFlip3_Left_Outline;  sprMask=spr_WormFlip3_Left_Mask; sprHeight=13; }
                            else if(stage==4) { sprOutline=spr_WormFlip4_Left_Outline;  sprMask=spr_WormFlip4_Left_Mask; sprHeight=10; }
                            else { sprOutline=spr_WormFlip1_Right_Outline; sprMask=spr_WormFlip1_Right_Mask; sprHeight=17; }
                        }
                        else 
                        {
                            // Started facing LEFT
                            if(stage==1) { sprOutline=spr_WormFlip1_Left_Outline; sprMask=spr_WormFlip1_Left_Mask; sprHeight=17; }
                            else if(stage==2) { sprOutline=spr_WormFlip2_Left_Outline; sprMask=spr_WormFlip2_Left_Mask; sprHeight=10; }
                            else if(stage==3) { sprOutline=spr_WormFlip3_Right_Outline; sprMask=spr_WormFlip3_Right_Mask; sprHeight=13; }
                            else if(stage==4) { sprOutline=spr_WormFlip4_Right_Outline; sprMask=spr_WormFlip4_Right_Mask; sprHeight=10; }
                            else { sprOutline=spr_WormFlip1_Left_Outline; sprMask=spr_WormFlip1_Left_Mask; sprHeight=17; }
                        }
                        
                        // Adjust Y for tall sprites so feet stay aligned
                        // (17px tall needs to be drawn 4px higher than 13px tall)
                        if(sprHeight == 17) y -= 4; 
                        if(sprHeight == 15) y -= 2;
                    }
                    // --- JUMPING ---
                    else if(Game_wormAnimState == ANIM_JUMP)
                    {
                        sprHeight = 15; // Jump sprites are tall
                        y -= 4;         // Align feet
                        
                        // Check Direction
                        if(Worm_dir & (unsigned short)1<<(i)) {
                            sprOutline = spr_WormJump_Left_Outline;
                            sprMask = spr_WormJump_Left_Mask;
                        } else {
                            sprOutline = spr_WormJump_Right_Outline;
                            sprMask = spr_WormJump_Right_Mask;
                        }
                    }
                    // Safety Fallback
                    else {
                        sprOutline = spr_WormRight_Outline;
                        sprMask = spr_WormRight_Mask;
                    }
                }
                // ============================================================
                // STANDARD WALKING / STANDING
                // ============================================================
                else
                {
                    char facing = (Worm_dir & (unsigned short)1<<(i))>0;
                    if(facing) {
                        sprOutline = spr_WormLeft_Outline; // Make sure these match SpriteData.c names!
                        sprMask = spr_WormLeft_Mask;
                    } else {
                        sprOutline = spr_WormRight_Outline;
                        sprMask = spr_WormRight_Mask;
                    }
                }

                // ============================================================
                // DRAWING
                // ============================================================

                // 1. Calculate the final Bottom-Right coordinate
                // short finalY = y + sprHeight;

                // 2. SAFETY CHECK:
                // Ensure we do not draw to negative memory addresses (Fatal Crash)
                // AND ensure we don't draw off the bottom (Visual Glitch)
                if(y > -sprHeight && y < 100) 
                {
                     // Draw Mask (AND logic)
                     ClipSprite16_AND_R(x, y, sprHeight, sprMask, darkPlane);
                     ClipSprite16_AND_R(x, y, sprHeight, sprMask, lightPlane);
                    
                     // Draw Outline (OR logic)
                     ClipSprite16_OR_R(x, y, sprHeight, sprOutline, darkPlane);
                     ClipSprite16_OR_R(x, y, sprHeight, sprOutline, lightPlane);
                }
                
                // ============================================================
                // HUD (Name / Health)
                // ============================================================
                // Only draw if NOT the current worm during a turn
                if(!(i==Worm_currentWorm && Game_mode==gameMode_Turn))
                {
                    short n;
                    for(n=0; n<2; n++)
                    {
                        short hX = x-24+n*32;
                        short hY = y-10; // Might need adjustment if sprite is tall (jumping)
                    
                        // mask out both planes
                        ClipSprite32_AND_R(hX, hY, 9, healthMasks[i]+(n*9), lightPlane);
                        ClipSprite32_AND_R(hX, hY, 9, healthMasks[i]+(n*9), darkPlane);
                        
                        // if its a light worm (Team 2?), make the light plane gray
                        if(i>7)
                            ClipSprite32_OR_R(hX, hY, 9, healthLightGray[i]+(n*9), lightPlane);
                            
                        // OR the sprite to both buffers
                        ClipSprite32_OR_R(hX, hY, 9, healthSprites[i]+(n*9), lightPlane);
                        ClipSprite32_OR_R(hX, hY, 9, healthSprites[i]+(n*9), darkPlane);
                    }
                } // end HUD
                
            } // end if on screen
        } // end if active
    } // next i
}

/**
	Draws all the in-game, on-screen Mines.
*/
void drawMines()
{
	short screenX, screenY;
	
	char fuseStr[8];
	
	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<MAX_MINES; i++)
	{
		if(Mine_active & (unsigned short)1<<(i))
		{
			screenX=Mine_x[i];
			screenY=Mine_y[i];
			if(worldToScreen(&screenX, &screenY))
			{				
				// draw the mines fill and outline
				ClipSprite8_OR_R(screenX-3, screenY-1, 4, spr_Mine_Dark, darkPlane);
				ClipSprite8_OR_R(screenX-3, screenY-1, 4, spr_Mine_Light, lightPlane);
				
				// if the mine has an active fuse, draw that too
				if(Mine_fuse[i]>0)
				{
					sprintf(fuseStr, "%d", (Mine_fuse[i]/TIME_MULTIPLIER));
					GrayDrawStr2B(screenX-4, screenY-16, fuseStr, A_NORMAL, lightPlane, darkPlane);
				}// end if fuse
				
				// if the oil drum is "settled" draw an arrow above it, for debug
				//char foo = (Mine_settled & (unsigned short)1<<(i));
				//DrawChar(screenX, screenY-8, (foo ? (char)20 : 'X'), A_NORMAL);
				
				/*
				char txt[4];
				sprintf(txt, "%d", (short)Mine_yVelo[i]);
				DrawStr(screenX-3, screenY-8, txt, A_NORMAL);
				*/
				
			}// end if on screen
		}// end if active
	}// next i
}

/**
	Draws all the in-game, on-screen Oil Drums.
*/
void drawOilDrums()
{
	short screenX, screenY;

	// loop over all mines and draw them if active:
	short i;
	for(i=0; i<MAX_OILDRUMS; i++)
	{
		if(OilDrum_active & (unsigned short)1<<(i))
		{
			screenX=OilDrum_x[i];
			screenY=OilDrum_y[i];
			if(worldToScreen(&screenX, &screenY))
			{
				// draw the oil drums fill and outline
				ClipSprite16_OR_R(screenX-4, screenY-5, 11, spr_Oil_Dark, darkPlane);
				ClipSprite16_OR_R(screenX-4, screenY-5, 11, spr_Oil_Light, lightPlane);
				
				
				// if the oil drum is "settled" draw an arrow above it, for debug
				//if(OilDrum_settled & (unsigned short)1<<(i))
				//	DrawChar(screenX, screenY-10, (char)20, A_NORMAL);
				
				/*
				char txt[4];
				sprintf(txt, "%d", (short)OilDrum_yVelo[i]);
				DrawStr(screenX-4, screenY-20, txt, A_NORMAL);
				*/
				
			}// end if on screen
		}// end if active
	}// next i
	
}


/**
	Draws all the in-game, on-screen Crates.
*/
void drawCrates()
{
	short screenX, screenY;
	
	// loop over all crates and draw them if active:
	short i;
	for(i=0; i<MAX_CRATES; i++)
	{
		if(Crate_active & ((unsigned short)1<<(i)))
		{
			screenX=Crate_x[i];
			screenY=Crate_y[i];
			if(worldToScreen(&screenX, &screenY))
			{
				// if crate has parachute
				if(i==parachuteCrate)
				{
					ClipSprite16_MASK_R(screenX-6, screenY-16, 12, spr_Parachute+12, spr_Parachute, lightPlane);
					ClipSprite16_MASK_R(screenX-6, screenY-16, 12, spr_Parachute+24, spr_Parachute, darkPlane);
				}
				
				// draw crate (this comes after parachut so it can slightly overlap)
				unsigned short *sprite = ((Crate_type[i]==crateHealth) ? (unsigned short*)&spr_CrateHealth[0] : ((Crate_type[i]==crateWeapon) ? (unsigned short*)&spr_CrateWeapon[0] : (unsigned short*)&spr_CrateTool[0]));					
				ClipSprite16_MASK_R(screenX-6, screenY-6, 12, sprite+12, sprite, lightPlane);
				ClipSprite16_MASK_R(screenX-6, screenY-6, 12, sprite+24, sprite, darkPlane);

			}// end if on screen
		}// end if active
	}// next i
}

/**
	Draws all the in-game, on-screen Weapon objects.
*/
void drawWeapons()
{
	short screenX, screenY;
	
	// loop over all weapons and draw them if active:
	short i;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		if(Weapon_active & ((unsigned short)1<<(i)))
		{
			screenX=Weapon_x[i];
			screenY=Weapon_y[i];
			if(worldToScreen(&screenX, &screenY))
			{	
				// if weapon uses spawnSelf, we can use it's same sprite as from the menu
				if(Weapon_props[(short)Weapon_type[i]] & spawnsSelf)
				{

					// for debug we will just draw a generic circle (borrowed from the charge sprites) for the weapon
					ClipSprite16_OR_R(screenX-2, screenY-2, 11, spr_weapons[Weapon_type[i]], lightPlane);
					ClipSprite16_OR_R(screenX-2, screenY-2, 11, spr_weapons[Weapon_type[i]], darkPlane);

				}else{	
					// for debug we will just draw a generic circle (borrowed from the charge sprites) for the weapon
					ClipSprite8_OR_R(screenX-2, screenY-2, 4, spr_chargeSmall, lightPlane);
					ClipSprite8_OR_R(screenX-2, screenY-2, 4, spr_chargeSmall, darkPlane);
				}


			}// end if on screen
			
		}// end if active

	}// next i
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
			
		// or respawn if off screen and far enoug bloe
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
		
		// get positions to muttate
		short xPosC = Cloud_x[i];
		short yPosC = Cloud_y[i];
		
		char onScreen = worldToScreen(&xPosC, &yPosC);
		
		// check scren pos:
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


// draw the map.. but I can't call this, crashes
void drawMap()
{
	/*
		How this works:
		
		Our mapBuffer pointer points a buffer that's four times the normal LCD_SIZE.
		
		We only want to draw a sub-section of that buffer on the screen.
		
		For vertical:
		
		The calculators screen height is 100 pixels, so we should draw 50 pixels above and below
		the camera's "center"
		
		So: CameraTop = CameraY-50, and CameraBottom = CameraY+50
		
		The rows we draw on the screen will always be between 0 and 99 inclusive.
		
		So we need to determine the following:
			- The top most row on the screen to draw
			- The bottom most row on the screen to draw
			- The top row from the buffer we should start copying.
			
		If the camera goes out of bounds of the map buffer, we can still draw part of the map
		In this case, there are a few conditions:
			- If the Camera goes above the map (CameraTop is less than 0 in the map-buffer)
				then we can still draw the map, but ScreenTop will be lower on the screen
			- If the Camera goes below the map (CameraBottom is more than 200 in the map-buffer)
				then we can still draw the map. ScreenTop will be 0, but we'll only draw till the end of the buffer
			- If either the CameraBottom is less than 0 or CameraTop is greater than 200 then the map is compeltey
				off screen, and we don't need to draw it at all.
				
		This way, the camera can still pan outside the map, into nothingness.
		This is useful for following weapons or worms that fly far off the map, or projectiles that fly high
		into the sky.
	*/
	
	// camera top position, in world-space
	short camTop = camY-50;
	
	// camera bottom position, in wrold-space
	short camBottom = camY+50;
	
	// camera left position, in world-space
	short camLeft = camX-80;
	
	// camera right position, in world-space
	short camRight = camX+80;
	
	// we shouldn't continue if either is out of bounds
	if((camBottom>0) && (camTop<200) && (camLeft<320) && (camRight>0))
	{ 
		
		// the of the screen we should start copying the buffer to
		short screenTop=0;
		
		// the bottom of the screen we should stop copying the buffer to
		short screenBottom=99;
		
		// the top row from the buffer we should start copying from
		// note: we don't need a buffer bottom, since the loop is controlled from screenTop to screenBottom
		// so we only really need bufferTop in the loop
		short bufferTop=camTop;

		// if both the top and the bottom of the buffer points are inbounds
		// we can simplly draw the whole screen worth!
		// we don't need to change any variables
		
		// if, however, the top of the camera is is above the map
		// we need to change our render bounds..
		if(camTop<0)
		{
			// whatever we draw, will be from the top row of the buffer
			bufferTop = 0;
			
			// we want to draw lower on the screen... by how far the camera is beyond
			screenTop = (camTop*-1);
			
			// we want to only copy pixels for the rest of the screen
			screenBottom = 99;

		// also check if the bottom of the camera is beyond the map buffer
		}else if(camBottom>199)
		{
			// always draw on the top of the screen:
			screenTop = 0;
			
			// always draw from the camera-top in the buffer:
			bufferTop = camTop;

			// only draw that many rows:
			screenBottom = (200-bufferTop);
		}
		
		// the left side of the screen, we should start drawing to
		short screenLeft = 0;
		
		// the right side of the screen, we should stop drawing at
		short screenRight = 160;
		
		// the position in the buffer we should start copying left-to-right
		short bufferLeft = camLeft;
		
		// if the screen's left is out of bounds of our map buffer
		// then we should start drawing in the middle of the screen...
		if(camLeft<0)
		{
			// the screen left should be the same as how far OB the camera is.
			// if the camera is 10 pixels to the left of the map, the map
			// should draw at 10px right on the screen...
			screenLeft = (camLeft*-1);
			
			// since we're starting at the left of the map, we will draw
			// all the way to the right edge of the screen:
			screenRight = 160;
			
			// since we're starting at the left of the screen, we will
			// start at the left of the buffer:
			bufferLeft=0;
			
		// if the right of the camera is beyond the right edge of the buffer...
		}else if(camRight>319)
		{
			// we will definately start drawing on the left side of the screen
			screenLeft = 0;
			
			// we will start the buffer at the left-camera edge:
			bufferLeft = camLeft;
			
			// we only need to draw until the buffer runs out..
			screenRight = 160; //320-bufferLeft;
		}
		
		// because we are going to be copying bits in blocks of 32 at a time below
		// we need to convert our screenLeft, screenRight and bufferLeft into columns
		short colLeft = (screenLeft-(screenLeft%32))/32;
		short colRight = (screenRight-(screenRight%32))/32+1;
		short colBuff = (bufferLeft-(bufferLeft%32))/32;

		// loop to manually copy memory from a sub-section of our map, via ClipSprite32
		unsigned long *light = (unsigned long*)mapLight;
		unsigned long *dark = (unsigned long*)mapDark;
		short x;
		short bufferCol = 0;
		
		// loop over the visible columns of the map on screen, or till our buffer runs out
		for(x=colLeft; (x<colRight && (colBuff+bufferCol)<10); x++)
		{
			short offset = screenLeft==0 ? camLeft%32 : 0;

			// take advantage of extgrah's sprite method to handle bit shiting and mem copying in one swoop!
			ClipSprite32_OR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &light[((colBuff+bufferCol)*200)+bufferTop], lightPlane);
			ClipSprite32_XOR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &dark[((colBuff+bufferCol)*200)+bufferTop], lightPlane);
			ClipSprite32_OR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &dark[((colBuff+bufferCol)*200)+bufferTop], darkPlane);

			// on the next iteration we will be on the next buffer 32 bit colum
			bufferCol++;
		
		}// next x
		
	}// end if draw map
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
		// take advantage of extgrah's sprite method to handle bit shiting and mem copying in one swoop!
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
		// exrase space for timer
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
	
	// current frame of annimation:
	static char frame=0;
	if(++frame>33)
		frame=0;
	
	// draw current frame of animated bar
	ClipSprite32_OR_R(127, 94, 3, windSprites[(short)((frame/3)%3)], darkPlane);
	
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
	
	short waterY = 241-camY;
	short i;
	for(i=0; i<7; i++)
	{
		short waterX = i*32 - (camX+328)%32;
	
		// draw just the and dark light planes:
		ClipSprite32_MASK_R(waterX, waterY, 9, &spr_Water_Dark[0+(f*9)], &spr_Water_Mask[0+(f*9)], darkPlane);
		ClipSprite32_AND_R(waterX, waterY, 9, &spr_Water_Mask[0+(f*9)], lightPlane);
		ClipSprite32_OR_R(waterX, waterY, 23, &spr_Water_Light[0+(f*23)], lightPlane);
	}
	
}


/**
 * Draws active explosions, if there are any
*/
void drawExplosions()
{
	// if no explosion is active, just gtfo
	if(Explosion_active==0)
		return;
		
	// loop over explosions..
	short i;
	for(i=0; i<MAX_EXPLOSIONS; i++)
	{
		if(Explosion_active & (unsigned short)1<<i)
		{
			short x=Explosion_x[i];
			short y=Explosion_y[i];
			short r=Explosion_size[i]-Explosion_time[i];
			
			if(worldToScreen(&x, &y))
			{
				short e;
				for(e=1; e<r; e++)
				{
					GrayDBufSetHiddenAMSPlane(DARK_PLANE);
					DrawClipEllipse(x, y, e, e, &(SCR_RECT){{0, 0, 159, 99}}, A_XOR);
					GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
					DrawClipEllipse(x, y, e, e, &(SCR_RECT){{0, 0, 159, 99}}, A_XOR);
				}
			}
		}// end if active
	}// next i
	
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
					
				The charge total goes between 0 and 255, so if we integer devide by 31,
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
			char *spritePtrs[] = {spr_chargeSmall, spr_chargeMed, spr_chargeLarge};
			
			// calc the charge amount in our scale
			char chargeAmt = (Game_currentWeaponCharge/31);
	
			// loop from furtherst to nearest
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
		
		// draw the crosshairs last so they're always on top of the charge
		ClipSprite8_OR_R(x-4, y-4, 8, spr_CrossHair, lightPlane);
		ClipSprite8_OR_R(x-4, y-4, 8, spr_CrossHair, darkPlane);
		
	}// end if uses aim or charge
	
}



// --------------------------------------------------------------------------------------------------------------------------------------



// draws cake loading screen
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
	DrawStr(40,70,progStr, A_NORMAL);	

	// now flip the planes
	GrayDBufToggleSync();
	
	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
}

// main drawing routine for the game, e.g. map, worms, weapons, etc
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
	drawMap();	
	
	// draw oil drums first, as everything else should overlap them
	drawOilDrums();
	
	// draw crates ontop of oil drums...
	drawCrates();
	
	// draw our wormy bros
	drawWorms();
	
	// mines are important, so draw them on top of everything else
	drawMines();
	
	// draw water that's infront of everything but the HUD
	drawWater(FALSE);
	
	// draw explosions over everthing except HUD, since they reverse the pixels
	drawExplosions();
	
	// draw weapon objects in the game...
	drawWeapons();
	
	// HUD stuff is drawn last since it needs to go on top of all game elements
	drawHUD();
	
	// if the game mode is worm select, draw the selection arrow
	if(Game_mode==gameMode_WormSelect)
		drawSelectArrow();
	
	// extra stuff to draw if the worm has certain wepaons selected
	if(Game_currentWeaponSelected!=-1)
		drawWeaponDetails();
		
	// for now, we will output a bunch of debug info on the screen
	
	// game modes by name	{"Select", "Turn", "WeaponSel", "Pause", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	static const char modes[9][16] = {"", "", "", "", "Cursor", "TurnEnd", "Death", "AfterTurn", "GameOver"};
	
	// draw the current and previous game mode on the scren
	DrawStr(60,1,modes[(short)Game_mode], A_XOR);

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
	
	// drwa the current worm on the screen
	//char wormStr[20];
	//sprintf(wormStr, "Worm Up: %d", (short)Game_currentWormUp[(short)Game_currentTeam]);
	//DrawStr(0,50, wormStr , A_XOR);	
	
	// draw the camera's position on the screen
	//char camStr[40];
	//sprintf(camStr, "Cam: %d, %d", (short)camX, (short)camY);
	//DrawStr(0,60, camStr , A_XOR);
	
	// draw our free memory on the screen, only when shift is held for debbuging
	if(Keys_keyState(keyCameraControl))
	{
		char heapStr[40];
		sprintf(heapStr, "%lu", (unsigned long)HeapAvail());
		DrawStr(0,1, heapStr , A_XOR);
	}
}


// main drawing routine for the pause menu
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
	char controlsStr[] = "[2nd] confirm / use weapon\n[?] jump\n[a] back flip\n[?]+[?]/[?]/[?]/[?] move camera\n[F1]/[CAT] weapons menu\n[APPS] select worm (if enabled)\n[ESC] pause\n[1]/[2]/[3]/[4]/[5] fuse lengh / opts";
	
	// use tiny font to draw controls
	FontSetSys(0);

	// draw dark on light plane, then xor to erase
	GrayDrawStr2B(10, 30, controlsStr, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(10, 30, controlsStr, A_XOR, lightPlane, darkPlane);
	
	FontSetSys(1);
	*/
	
}


// main drawing routine for the weapons menu
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
	for(x=1; x<14; x++)
			GrayFastDrawLine2B(0+(x*12), 13, 0+(x*12), 73, 1, lightPlane, darkPlane);
	for(y=1; y<6; y++)
			GrayFastDrawHLine2B(1, 156, 12+(y*12), 1, lightPlane, darkPlane);
	
	// drawk black grid lies
	for(x=0; x<14; x++)
		GrayFastDrawLine2B(1+(x*12), 13, 1+(x*12), 73, 3, lightPlane, darkPlane);
	for(y=0; y<6; y++)
		GrayFastDrawHLine2B(1, 156, 13+(y*12), 3, lightPlane, darkPlane);
	
	// draw drop shadow for weapons box (also to make it "centered" perfectly
	GrayFastDrawHLine2B(2, 157, 74, 1, lightPlane, darkPlane);
	GrayFastDrawLine2B(158, 14, 158, 74, 1, lightPlane, darkPlane);
	
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

// render a text message to the buffers
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
		unsigned long *charStartUL = spr_tinyFont + (((chr-(chr%8))/8)*3);
	
		// add leading space
		pixColumn++;
		
		// now we loop to copy the chars pixels from its row, to our row and location
		int chrPix, row;
		for(chrPix=0; chrPix<chrWidth; chrPix++)
		{
			// increment the our buffers colum
			pixColumn++;
			
			// make a mask for our current pixel colum
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
	 	
	 	note: we couldn't have done this earlier, becauwe we needed to convert the chars to our
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
				
				// unsigned long on this row in previous colum, or 0 if left-most
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
 * draws text to a 5 row high, 2 column wide unsigned long buffer
 *
 * for usage when rendering name and health sprites, which are 64 bits wide and 7 bits tall
 * @param buffer the buffer to render the text to
 * @param mask the mask buffer to render the mask to
 * @param txt the text to write
 * @param color the white on black or black on white mode
*/
void txtToBuffer(unsigned long *buffer, unsigned long *mask, char *txt, char color)
{
	// draw text to our buffer
	short txtLen = Draw_renderText(buffer, 2, (char*)txt, color);
	
	// loop to create masks we can AND to clip the sprites wasted space on either side...
	short txtLeft = ((64-txtLen)/2)-2;
	txtLen+=2;
	
	short px;
	unsigned long eraseMask=1;
	eraseMask = eraseMask<<31;
	for(px=0; px<64; px++)
	{
		// everytime we loop over a long, reset the erase mask
		if(px%32==0)
		{
			eraseMask = 1;
			eraseMask = eraseMask<<31;
		}
			
		// only erase the row, if its outside the text:
		if(px<=txtLeft || px>=(txtLeft+txtLen))
		{
			// convert px to our column
			short col = (px<32) ? 0 : 5;

			// erase this pixel from all rows on our current colum...
			int row;
			for(row=0; row<5; row++)
			{
				// only erase rows 0 and 4 from our edges
				if(!((px==txtLeft || px==(txtLeft+txtLen)) && (row>0 && row<4)))
				{
					*(buffer+col+row) &= ~eraseMask;
					*(mask+col+row) |= eraseMask;
				}				
			}// next row				
		}// end if erasable
		
		// shift our erase mask right (when it hits the edge, px%32 will be 0 and reset the mask)
		eraseMask = eraseMask>>1;
	}// next px
}


// draw health sprite for a worm
void Draw_healthSprite(short index)
{
	/*
		make four buffers total:
			- two sprite buffers for the name and health
			- two mask buffers for the name and health
	*/
	unsigned long sn[10];
	unsigned long snm[10];
	unsigned long sh[10];
	unsigned long shm[10];
	
	// clear all rows in our buffer, and mask buffer before we draw the text
	int ul;
	for(ul=0; ul<18; ul++)
	{
		healthSprites[index][ul] = (index<8) ? 0b11111111111111111111111111111111 : 0;
		healthMasks[index][ul] = 0;// 0b11111111111111111111111111111111;
		healthLightGray[index][ul] = 0;
		
		if(ul<10)
		{
			sn[ul] = (index<8) ? 0b11111111111111111111111111111111 : 0;
			snm[ul] = 0;
			sh[ul] = (index<8) ? 0b11111111111111111111111111111111 : 0;
			shm[ul] = 0;
		}
	}
	
	// make health text to draw
	char txtHealth[20];
	sprintf(txtHealth, "%d", Worm_health[index]);
	
	// draw the name and mask to each set of buffers
	txtToBuffer(sn, snm, (char*)Match_wormNames[index], (index<8));
	txtToBuffer(sh, shm, (char*)txtHealth, (index<8));
	
	// now we need to combine the name and health into our single sprite,
	// for faster drawing
	int row, col;
	for(col=0; col<2; col++)
	{
		// copy the health rows...
		for(row=0; row<5; row++)
		{
			healthSprites[index][(col*9)+row+4] = sh[(col*5)+row];
			healthMasks[index][(col*9)+row+4] = shm[(col*5)+row];
			healthLightGray[index][(col*9)+row+4] = ~(shm[(col*5)+row]);
		}
		
		// add the rows for the sprite:
		for(row=0; row<5; row++)
		{
			healthSprites[index][(col*9)+row] = sn[(col*5)+row];
			healthMasks[index][(col*9)+row] = snm[(col*5)+row];
			healthLightGray[index][(col*9)+row] = ~(snm[(col*5)+row]);
		}
	}// next col
}


