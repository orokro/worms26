/*
	Worms.c
	-------
	
	This file defines the worms. Note that, a separate file will handle controlling
	the currently selected worm during a turn.

	In the previous version of Worms, we had two separate sets of arrays:
	one for the White team and one for the Black team.
	
	In this new implementation, we will have one set of arrays for ALL worms.
	
	This way, we can loop over all of them at once, and update all of them at once.
	
	C Source File
	Created 11/11/2017; 11:34:05 PM
*/


// includes
#include "Main.h"
#include "Draw.h"
#include "SpriteData.h"
#include "Match.h"
#include "Game.h"
#include "Mines.h"
#include "Crates.h"
#include "PhysCol.h"
#include "Worms.h"
#include "Map.h"
#include "StatusBar.h"
#include "Keys.h"
#include "Weapons.h"


// the X/Y position of all the worms
short Worm_x[MAX_WORMS] = {15, 81, 120, 65, 90, 35, 150, 40, 175, 95, 250, 210, 25, 140, 150, 10};
short Worm_y[MAX_WORMS] = {100, 195, 55, 40, 15, 140, 20, 135, 30, 20, 150, 45, 170, 15, 75, 5};

// the current X/Y velocity of the worm, if it was knocked back by an explosion, etc
char Worm_xVelo[MAX_WORMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Worm_yVelo[MAX_WORMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// physics objects for this worm
PhysObj Worm_physObj[MAX_WORMS];

// the direction the worm is currently FACING.. 0 = LEFT 1 = RIGHT
unsigned short Worm_dir = 0;

// the current HEALTH of the worm
short Worm_health[MAX_WORMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

// bit mask if the worm is dead. bit 1 = dead, bit = 0 alive
unsigned short Worm_isDead = 0;

// bit mask if the worm is ACTIVE... different that dead.
unsigned short Worm_active = 0;

// poisoned worms bit mask
unsigned short Worm_poisoned = 0;

// store the current mode for every worm:
char Worm_mode[MAX_WORMS] = {wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };

// every time the team / turn switches a new worm is the "active" worm.
// if worm-select is enabled, during this period the play can change the active worm.
// the game-logic will update this variable, such that worms that are alive, and on the current team, will cycle
char Worm_currentWorm = 0;

// if any of the bits in this mask are 1, then the worm moved within
// the last frame. We can't end a turn until all worms are "stable"
unsigned short Worm_settled = 0;

// if the collision detection for DOWN on a worm was true, then the worm is "on the ground"
// this is important for detecting falling for parachute or for walking
unsigned short Worm_onGround = 0;

// the current worms 10x10 tile. we don't need to store both X and Y, just the tile index
short Worm_tile[MAX_WORMS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

// these sprites will be generated at the beginning and when health changes.
unsigned long healthSprites[16][18];
unsigned long healthMasks[16][18];
unsigned long healthLightGray[16][18];

// reusable msg buffer
static char buffer[32];

// function prototype
void renderHealthSprite(short index);



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Spawns a Worm of given index on the Map.
 * 
 * @param index the worm to spawn.
*/
void spawnWorm(short index)
{
	unsigned short mask = 1;
	mask = mask<<(index);
	
	// find a free place for it on the map
	short sx = 0;
	short sy = 0;
	
	// This function handles retry logic internally, but always good to check
	if(Map_findSpawnPoint(SPAWN_WORM, &sx, &sy)) {
		Worm_x[index] = sx;
		Worm_y[index] = sy;
	} else {
		// Fallback: This should ideally not happen if maxAttempts is high enough.
		// We'll just dump them somewhere "safeish" or leave as is (which is bad).
		// For now, let's just pick a random spot and pray.
		Worm_x[index] = random(300) + 10;
		Worm_y[index] = 10; 
	}
	
	// set active
	Worm_active |= mask;

	// make a new collider and physics object for this worm
	Collider col = new_Collider(COL_UDLR, -4, 6, -2, 2);
	Worm_physObj[index] = new_PhysObj(&Worm_x[index], &Worm_y[index], &Worm_xVelo[index], &Worm_yVelo[index], 0.4f, 1.0f, (char)index, &Worm_settled, col);
	
	// allow worms to bounce of walls, but not land
	Worm_physObj[index].bouncinessY = 0.0f;
	
	// initial health
	Worm_health[index] = Match_wormStartHealth;

	// for debug, give random health:
	// Worm_health[index] = random(200)+1;
	
	// render initial health sprite
	renderHealthSprite(index);
	
	// random direction
	if(random(2)==0)
		Worm_dir |= mask;
}


/**
 * checks if a worm is close to a crate or a mine, based on the tile the worm is in
 */
void checkCratesAndMines(short index)
{
		short i, c;
    
    // Check Mines
    for(i=0; i<MAX_MINES; i++)
    {
        // Check if active, not triggered, and close enough
        if((Mine_active & (1<<i)) && !(Mine_triggered & (1<<i)))
        {
             if(abs(Mine_x[i] - Worm_x[index]) < 13 && abs(Mine_y[i] - Worm_y[index]) < 13)
             {
                 Mines_trigger(i);
             }
        }
    }

    // Check Crates
    for(c=0; c<MAX_CRATES; c++)
    {
        // Check if active, not triggered, and close enough
        if((Crate_active & (1<<c)))
        {
             if(abs(Crate_x[c] - Worm_x[index]) < 15 && abs(Crate_y[c] - Worm_y[index]) < 15)
             {
				Crates_pickUp(c, index);
             }
        }
    }
}



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * spawns worms on the map
 */
void Worm_spawnWorms()
{
	short i=0;
	for(i=0;i<Match_wormCount[0]; i++)
		spawnWorm(i);
	for(i=0;i<Match_wormCount[1]; i++)
		spawnWorm(8+i);
}


/**
 * main update method for all worms
 */
void Worm_update()
{
	// loop over worms, and any active worms should have their gravity and physics applied..
	short i;
	unsigned short wormMask;
	unsigned short Worm_wasOnGround = Worm_onGround;
	for(i=0; i<MAX_WORMS; i++)
	{
		// calculate the bitmask for this worm once, since we'll use it a lot
		wormMask = 1;
		wormMask = (unsigned short)((unsigned short)wormMask<<(i));
		
		// only update worms in the game
		if(Worm_active & wormMask)
		{
			
			//if(i == Worm_currentWorm){
			//	Camera_focusOn(&Worm_x[i], &Worm_y[i]);
				
			//}

			// if the worm is below water, insta-kill it
			if((Worm_isDead & wormMask)==0 && Worm_y[i]>(196-Game_waterLevel)){
				
				Worm_isDead |= wormMask;
				Worm_setHealth(i, 0, FALSE);
				
				sprintf(buffer, "%s drowned!", Match_wormNames[i]);
				StatusBar_showMessage(buffer);				
				
				// immediately end the turn if the current worm drowned
				if(i == Worm_currentWorm)
					Game_changeMode(gameMode_TurnEnd);
				
				continue;
			}
			
			// check all explosions if they are near-by and damaging this worm
			short damage = Physics_checkExplosions(&Worm_physObj[i]);
			if(damage!=0)
				Worm_setHealth(i, -damage*6, TRUE);

			// check active weapons to see if one hit and its not the current worm
			if(Worm_currentWorm!=i)
			{
				damage = Physics_checkWeapons(i, wormMask, &Worm_physObj[i]);
				if(damage!=0)
					Worm_setHealth(i, -damage, TRUE);
			}
				
			// if the worm is considered "settled" no need for physics
			if(!(Worm_settled & wormMask))
			{
				// add gravity to the worm
				Worm_yVelo[i]++;
				
				// if the worm is dead, it's gravestone can only have vertical velocity, no X
				if(Worm_isDead & wormMask)
					Worm_xVelo[i]=0;

				// do physics and collision for worm
				Physics_apply(&Worm_physObj[i]);				
				
				// if we collided with the ground on the last frame, assume the worm is grounded.
				// (it should collide with the ground ever frame its grounded, due to gravity.
				// if the worm was settled on this frame, we won't get here on the next frame
				// so we can just mark it as settled and it will stay that way till moved again
				if(Worm_physObj[i].col.collisions & COL_DOWN || (Worm_settled & wormMask))
					Worm_onGround |= wormMask;
				else
					Worm_onGround &= ~wormMask;

				// reset jump animation if any
				if(!(Worm_wasOnGround & wormMask) && (Worm_onGround & wormMask))
                {
                    if (Game_wormAnimState != ANIM_DRILL && Game_wormAnimState != ANIM_TORCH)
					    Game_wormAnimState = ANIM_NONE;
                }
			}
			
			// calculate the worms tile, and if it changed, we need to check for mine and crate updates
			Worm_tile[i] = (Worm_x[i]/10) * (Worm_y[i]/10);
		
			//check if a worm triggered a crate or a mine
			if(i == Worm_currentWorm)
				checkCratesAndMines(i);
					
		}// end if active worm

	}// next i
}


/**
 * changes the worms health
 */
void Worm_setHealth(short index, short health, char additive)
{
	// cant adjust health if this worms team is frozen
	const short team1frozen = (Game_stateFlags & gs_team1Frozen);
	const short team2frozen = (Game_stateFlags & gs_team2Frozen);
	if( (team1frozen && (index<8)) || (team2frozen && (index>=8)) )
		return;

	if(additive)
		Worm_health[index] += health;
	else
		Worm_health[index] = health;
	
	// don't let go below 0
	if(Worm_health[index]<0)
		Worm_health[index]=0;
		
	// update it's sprite
	renderHealthSprite(index);

	// update team health sprites
	Draw_renderTeamHealth();

	// if it's the current worm & health was lowered, end turn and lock turn end
	if(additive && index==Worm_currentWorm && health<0 && Game_mode==gameMode_Turn)
	{
		Game_stateFlags |= gs_lockTurnEnd;
		Game_changeMode(gameMode_TurnEnd);
	}
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

			// erase this pixel from all rows on our current column...
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


/**
 * draw health sprite for a worm
 * @param index - worm health index to draw
 */
void renderHealthSprite(short index)
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


/**
	Draws all the in-game, on-screen Worms.
*/
void Worm_drawAll()
{
    short screenX, screenY, i;
    unsigned short wormMask;

    // Pointers for the sprite data to draw this frame
    const unsigned short* pMask;
    const unsigned short* pLight;
    const unsigned short* pDark;
    
    // index of the sprite to use (base index, e.g. MASK)
    int spriteIdx = -1;
    char useFlipped = FALSE; // TRUE to use wormsSpritesFlipped

	// Check if walking (Active worm, Turn mode, Keys pressed)
	// Note: CharacterController handles movement, we just visualize it here
	char isWalking = ((Keys_keyState(keyLeft) || Keys_keyState(keyRight)) && (Game_mode == gameMode_Turn));
	char isCurrentWorm;

    // Loop over all worms
    for(i=0; i<MAX_WORMS; i++)
    {
		wormMask = (unsigned short)1<<(i);
		isCurrentWorm = (i == Worm_currentWorm);

        // Check active bitmask
        if(Worm_active & wormMask)
        {
            screenX = Worm_x[i];
            screenY = Worm_y[i];

            // Only draw if on screen
            if(worldToScreen(&screenX, &screenY))
            {
                // Align sprite (centered x, feet y adjustment)
                // Note: Standard width is 16, so center is -8.
                short x = screenX - 8;
                short y = screenY - 6;

                // Reset defaults
                spriteIdx = WORM_IDLE_MASK;
                useFlipped = (Worm_dir & wormMask) > 0; // Default: 0=Right (Normal), 1=Left (Flipped)

                // ============================================================
                // SPRITE SELECTION LOGIC
                // ============================================================
                
				// if the worm is dead, draw gravestone (Legacy sprite, not in new system yet?)
                // Assuming spr_Grave still exists in standard sprites.
				if(Worm_isDead & wormMask){
					GrayClipSprite8_OR_R(x+4, y, 12, spr_Grave, spr_Grave, lightPlane, darkPlane);
					continue;
				}

				// if the current team is invisible, skip drawing them
				const short team1invisible = (Game_stateFlags & gs_team1Invisible);
				const short team2invisible = (Game_stateFlags & gs_team2Invisible);

				if( (team1invisible && (i<8)) || (team2invisible && (i>=8)) )
					continue;
					
				// if the current team is frozen
				const short team1frozen = (Game_stateFlags & gs_team1Frozen);
				const short team2frozen = (Game_stateFlags & gs_team2Frozen);

				if( (team1frozen && (i<8)) || (team2frozen && (i>=8)) )
				{
                    // Use Frozen Sprites
                    spriteIdx = WORM_FROZEN_MASK;
                    useFlipped = (Worm_dir & wormMask) > 0;
                    // Note: Frozen has Light/Dark distinct sprites
				}
                // Is this the dying worm during Death mode?
                else if(Game_mode == gameMode_Death && (Worm_health[i] <= 0) && !((Worm_isDead & wormMask)))
                {
                    // Timer counts 60 -> 30 (Explosion) -> 0
                    if(Game_deathTimer > 45) {
                        spriteIdx = WORM_TNT1_MASK; // Plunger Up
                    } else {
                        spriteIdx = WORM_TNT2_MASK; // Plunger Down
                    }
                    useFlipped = (Worm_dir & wormMask) > 0;
                }
				// if the anim is drill or drill is equipped
				else if(isCurrentWorm &&((!isWalking && Game_currentWeaponSelected == WDrill) || (Game_wormAnimState == ANIM_DRILL)))
				{
					spriteIdx = WORM_DRILL_MASK;
					useFlipped = (Worm_dir & wormMask) > 0;
				}
				// if the anim is torch or torch is equipped
				else if(isCurrentWorm &&((!isWalking && Game_currentWeaponSelected == WBlowTorch) || (Game_wormAnimState == ANIM_TORCH)))
				{
					spriteIdx = WORM_TORCH_MASK;
					useFlipped = (Worm_dir & wormMask) > 0;
				}
                // Is this the Active Worm doing an Animation?
                else if(isCurrentWorm && Game_wormAnimState != ANIM_NONE)
                {
                    // --- BACKFLIP ---
                    if(Game_wormAnimState == ANIM_BACKFLIP)
                    {
                        Game_wormAnimTimer++; // Advance frame
                        
                        // Determine Stage (1-4)
                        int stage = 1;
                        if(Game_wormAnimTimer < 6)       stage = 1; 
                        else if(Game_wormAnimTimer < 8)  stage = 2; 
                        else if(Game_wormAnimTimer < 9)  stage = 3; 
                        else if(Game_wormAnimTimer < 12) stage = 4; 
                        else                             stage = 1; 

                        // Select Base Sprite
                        switch(stage) {
                            case 1: spriteIdx = WORM_BACKFLIP1_MASK; break;
                            case 2: spriteIdx = WORM_BACKFLIP2_MASK; break;
                            case 3: spriteIdx = WORM_BACKFLIP3_MASK; break;
                            case 4: spriteIdx = WORM_BACKFLIP4_MASK; break;
                        }

                        // Determine Orientation
                        // 0 = Right (Normal), >0 = Left (Flipped)
                        useFlipped = (Game_wormFlipStartDir > 0);
                        
                        // Adjust Y for tall sprites so feet stay aligned
                        // We check the height of the selected sprite
                        short h = wormSpriteHeights[spriteIdx];
                        if(h >= 17) y -= 4; 
                        else if(h >= 15) y -= 2;
                    }

                    // --- JUMPING ---
                    else if(Game_wormAnimState == ANIM_JUMP)
                    {
                        spriteIdx = WORM_JUMP_MASK;
                        // Direction handled by default 'useFlipped' logic at top
                        
                        // Manual Y Adjustment for Jump (assumed tall)
                         y -= 4;
                    }
                } // End Animation
                
                // ============================================================
                // PARACHUTE
                // ============================================================
                else if(isCurrentWorm && (Game_stateFlags & gs_parachuteMode))
                {
                    spriteIdx = WORM_CHUTE_MASK;
                    useFlipped = (Worm_dir & wormMask) > 0;
                }
                
                // ============================================================
                // BUNGEE
                // ============================================================
                else if(isCurrentWorm && (Game_stateFlags & gs_bungeeMode))
                {
                    spriteIdx = WORM_BUNGEE_MASK;
                    useFlipped = (Worm_dir & wormMask) > 0;
                }

                // ============================================================
                // STANDARD WALKING / STANDING
                // ============================================================
                else
                {
                    char facing = (Worm_dir & wormMask)>0;
                    if(facing) {
                        // Default Left (Flipped) - will be handled by useFlipped below
                        useFlipped = TRUE;
                    } else {
                        // Default Right (Normal)
                        useFlipped = FALSE;
                    }
                    
                    // -- ANIMATION LOGIC --
                    
                    if(isWalking && isCurrentWorm) {
                        // Walking Animation: Toggle WALK / IDLE fast
                        // 15FPS -> 8 frame cycle (approx 0.5s)
                        if((Game_timer & 0x07) < 4) {
                             spriteIdx = WORM_WALK_MASK;
                        } else {
                             spriteIdx = WORM_IDLE_MASK;
                        }
                    } 
                    else {
                        // Idle Animation: Occasional Blink to IDLE2
                        // Use prime multiplier (37) to desync worms
                        // Interval: 128 ticks (approx 8.5s at 15FPS)
                        // Duration: 10 ticks (approx 0.66s)
                        short timerVal = (Game_timer + (i * 37)) & 127; 
                        
                        if(timerVal < 10) {
                            spriteIdx = WORM_IDLE2_MASK;
                        } else {
                            spriteIdx = WORM_IDLE_MASK;
                        }
                    }
                }

                // ============================================================
                // FETCH DATA & DRAW
                // ============================================================
                
                // 1. Get Height
                short sprHeight = wormSpriteHeights[spriteIdx];
                
                // 2. Get Pointers (Normal or Flipped)
                // Note: The macros like WORM_IDLE_LIGHT point to the index in the array.
                // Since MASK is the base, and Light/Dark follow, we need to resolve them relative to spriteIdx.
                
                // ASSUMPTION: The macros are defined sequentially: MASK, LIGHT, DARK (or LIGHT/DARK same if outline)
                // We can use the defines directly if we know the offset, but since we have a variable 'spriteIdx' (which is the Mask index),
                // we can assume:
                // Mask  = spriteIdx
                // Light = spriteIdx + 1
                // Dark  = spriteIdx + 2 (if distinct) OR spriteIdx + 1 (if Outline)
                
                // However, to be safe and generic without hardcoding +1/+2, we should probably check if it's Frozen.
                // For Frozen, we know we have distinct Light/Dark.
                // For others (Idle, Jump, Backflip), we used Outline (Light == Dark).
                
                int idxMask = spriteIdx;
                int idxLight = spriteIdx + 1;
                int idxDark  = spriteIdx + (spriteIdx == WORM_FROZEN_MASK ? 2 : 1);

                if(useFlipped) {
                    pMask = wormsSpritesFlipped[idxMask];
                    pLight = wormsSpritesFlipped[idxLight];
                    pDark = wormsSpritesFlipped[idxDark];
                } else {
                    pMask = wormsSprites[idxMask];
                    pLight = wormsSprites[idxLight];
                    pDark = wormsSprites[idxDark];
                }

                // 3. Draw
                // SAFETY CHECK:
                if(y > -sprHeight && y < 100) 
                {
                     // Draw Mask (AND logic)
                     ClipSprite16_AND_R(x, y, sprHeight, pMask, darkPlane);
                     ClipSprite16_AND_R(x, y, sprHeight, pMask, lightPlane);
                    
                     // Draw Light Component (OR logic)
                     // (Always drawn to light plane)
                     ClipSprite16_OR_R(x, y, sprHeight, pLight, lightPlane);
                     
                     // Draw Dark Component (OR logic)
                     // (Drawn to dark plane UNLESS poisoned)
					if(!(Worm_poisoned & wormMask)) {
						ClipSprite16_OR_R(x, y, sprHeight, pDark, darkPlane);
                    }

                    // 4. Parachute Canopy (Draw on top of worm)
                    if(isCurrentWorm && (Game_stateFlags & gs_parachuteMode))
                    {
                        // Draw at screenX-8 (centered for 16px) and screenY-20 (above worm)
                        // Use 12px height canopy
                        ClipSprite16_AND_R(screenX-8, screenY-20, 12, spr_Parachute, darkPlane);
                        ClipSprite16_AND_R(screenX-8, screenY-20, 12, spr_Parachute, lightPlane);
                        ClipSprite16_OR_R(screenX-8, screenY-20, 12, spr_Parachute+12, lightPlane);
                        ClipSprite16_OR_R(screenX-8, screenY-20, 12, spr_Parachute+24, darkPlane);
                    }
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
        
        // check if we're the current worm and we need to draw weapons or anything
        
    } // next i
}