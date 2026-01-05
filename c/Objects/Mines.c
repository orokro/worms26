/*
	Mines.c
	-------
	
	This defines Mines on the map.
	
	About Mines:
	
	Mines can either be enabled or disabled.
	If disabled, they will not spawn on the map.
	
	Mines can have pre-set fuse lengths, or random fuse lengths.
	
	Mines can optionally have duds, which fizzle out and do not explode
	when their fuse runs out.
	
	Note about dudes: since 6 mines will spawn per round, randomly all over
	the map, we can say: the mine at index 0 will be a dud, if dudes are enabled.
	
	There will never be more than one dud, and if duds are enabled we can
	just use index 0, which will randomly be placed - no need to choose
	a random index.
	
*/


// includes
#include "Main.h"
#include "Camera.h"
#include "Keys.h"
#include "Match.h"
#include "Mines.h"
#include "PhysCol.h"
#include "Map.h"
#include "Explosions.h"
#include "SpriteData.h"
#include "Draw.h"


// x/y positions of our Mines
short Mine_x[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short Mine_y[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// velocity of mines
char Mine_xVelo[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Mine_yVelo[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// fuse of mines
char Mine_fuse[MAX_MINES] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// is the mine active? using bits for booleans
unsigned short Mine_active=0;

// is the mine triggered, with a countdown?
unsigned short Mine_triggered=0;

// is the mine settled on the map?
unsigned short Mine_settled=0;

// dude mines
unsigned short Mine_dud=0;

// local vars
PhysObj Mine_physObj[MAX_MINES];



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Spawns a single mine of the given index.
 * 
 * @param index the mine to spawn.
*/
void spawnMine(char index)
{
	// find a free place for it on the map
	short sx, sy;
	if(Map_findSpawnPoint(SPAWN_MINE, &sx, &sy)) {
		Mine_x[(short)index] = sx;
		Mine_y[(short)index] = sy;
	} else {
		return; // If we can't spawn, just don't spawn it.
	}
	
	// make this mine active:
	Mine_active |= (unsigned short)1<<(index);
	
	// make sure not dud (yet)
	Mine_dud &= ~((unsigned short)1<<(index));

	// make a new collider and physics object for this mine
	new_Collider(&(Mine_physObj[(short)index].col), COL_UDLR, 2, 2, 3, 3);
	new_PhysObj(&Mine_physObj[(short)index], &Mine_x[(short)index], &Mine_y[(short)index], &Mine_xVelo[(short)index], &Mine_yVelo[(short)index], 65, 100, (char)index, &Mine_settled);
}


/**
 * Updates a Mine of given index, should be called every frame by Mines_update().
 * 
 * @param index the mine to update.
*/
void updateMine(short index)
{
	// if the mine has time on it, decrement it
	if(Mine_fuse[index]>0)
	{
		Mine_fuse[index]--;
		
		// if it reaches 0, boom
		if(Mine_fuse[index]==0)
		{
			// disable the mine
			Mine_active &= ~((unsigned short)1<<(index));
			Mine_triggered &= ~((unsigned short)1<<(index));

			// explode, unless it's a dud
			if((Mine_dud & (unsigned short)1<<(index))==FALSE)
				Explosion_spawn(Mine_x[index], Mine_y[index], 12, 14, FALSE);
				
		}// end if 
		
	}//end if active fuse
	
	// check if explosions moved this mine
	Physics_checkExplosions(&Mine_physObj[index]);
			
	// if the Mine is considered "settled" no need for physics
	if(!(Mine_settled & (unsigned short)1<<(index)))
	{
		// add gravity to mine
		Mine_yVelo[index]++;
		
		// do physics and collision for mine
		Physics_apply(&Mine_physObj[index]);
	}
}



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * spawns Mines on the map, if they're enabled
 */
void Mines_spawnMines()
{
	char i=0;
	
	// only spawn 6 mines, leaving 4 free slots for user placeable mines
	for(i=0; i<3; i++)
		spawnMine(i);
}


/**
 * main update for Mines
 */
void Mines_update()
{		
	// for debugging, we'll allow the user to switch between mines, and add velocity to
	// test physics
	static short testMine = 0;
	if(!Keys_keyState(keyCameraControl))
	{
		if(Keys_keyDown(key1))
		{
			testMine++;
			if(testMine>5)
				testMine=0;
			Camera_focusOn(&Mine_x[(short)testMine], &Mine_y[(short)testMine]);
		}
		if(Keys_keyDown(key2))
		{
			Physics_setVelocity(&Mine_physObj[testMine], -4, -7, FALSE, TRUE);
		}
		if(Keys_keyDown(key3))
		{
			Physics_setVelocity(&Mine_physObj[testMine], 4, -7, FALSE, TRUE);
		}
		if(Keys_keyDown(key5))
		{
			Mines_trigger(testMine);
		}
	}
	
	// if any of the active Crates have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<MAX_MINES; i++)
	{
		// only update active mines
		if(Mine_active & (unsigned short)1<<(i))
		{
			// updates the mine (if it has a timer, decrease it and explode it if necessary)
			updateMine(i);
		}	
	}// next i
}


/**
 * triggers a mine
 * 
 * @param index - the mine to trigger
 */
void Mines_trigger(short index)
{
	// if this mine is already triggered, just gtfo
	if(Mine_triggered & (unsigned short)1<<(index))
		return;
		
	// mine triggered
	Mine_triggered |= (unsigned short)1<<(index);
	
	// set fuse
	Mine_fuse[index] = ((Match_mineFuseLength<4) ? (Match_mineFuseLength*TIME_MULTIPLIER) : (random(5)*TIME_MULTIPLIER));

	// if dudes are enabled, pick a number 1-4 and if it's 1, make this mine a dud
	if(Match_dudMines)
	{
		if(random(4)==0)
		{
			// make dud
			Mine_fuse[index] = 10;
			Mine_dud |= (unsigned short)1<<(index);
			return;
		}
	}

	// if fuse length is 0, just immediately detonate
	if(Mine_fuse[index]==0)
	{
		Mine_fuse[index] = 1;
		updateMine(index);
		return;
	}
}


/**
	Draws all the in-game, on-screen Mines.
*/
void Mines_drawAll()
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
					if((Mine_dud & (unsigned short)1<<(i))==FALSE)
					{
						sprintf(fuseStr, "%d", (Mine_fuse[i]/TIME_MULTIPLIER));
						GrayDrawStr2B(screenX-2, screenY-8, fuseStr, A_NORMAL, lightPlane, darkPlane);
					}else
					{
						ClipSprite8_OR_R(screenX-3, screenY-7, 4, sprDudSmoke, darkPlane);
					}
				}// end if fuse			
			}
		}
	}// next i
}
			
			
/**
 * Spawns a mine at a specific location
 * 
 * @param x x position
 * @param y y position
 */
void Mines_spawnAt(short x, short y)
{
	short i;
	for(i=0; i<MAX_MINES; i++)
	{
		// if not active, we can use this slot
		if(!(Mine_active & (unsigned short)((unsigned short)1<<i)))
		{
			Mine_x[i] = x;
			Mine_y[i] = y;
			Mine_xVelo[i] = 0;
			Mine_yVelo[i] = 0;
			Mine_fuse[i] = -1; // No fuse initially
			
			Mine_active |= (unsigned short)1<<i;
			Mine_triggered &= ~((unsigned short)1<<i);
			Mine_settled &= ~((unsigned short)1<<i); // Assume unsettled initially

			new_Collider(&(Mine_physObj[i].col), COL_UDLR, 2, 2, 3, 3);
			new_PhysObj(&Mine_physObj[i], &Mine_x[i], &Mine_y[i], &Mine_xVelo[i], &Mine_yVelo[i], 65, 100, (char)i, &Mine_settled);
			return;
		}
	}
}
