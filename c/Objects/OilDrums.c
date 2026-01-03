/*
	OilDrums.c
	----------
	
	This defines our OilDrums on the map.
	
	About OilDrums:
	
	They can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	OilDrums have no velocity - they can fall down, but not moved otherwise.
	
	OilDrums have health, and will explode if drained of health.
	
	C Source File
	Created 11/12/2017; 8:33:19 PM
*/


// includes
#include "Main.h"
#include "Match.h"
#include "OilDrums.h"
#include "PhysCol.h"
#include "Map.h"
#include "Explosions.h"
#include "SpriteData.h"
#include "Draw.h"


// x/y positions of our oil drums
short OilDrum_x[MAX_OILDRUMS] = {0, 0, 0, 0, 0, 0};
short OilDrum_y[MAX_OILDRUMS] = {0, 0, 0, 0, 0, 0};

// health of our oil drums
char OilDrum_health[MAX_OILDRUMS] = {30, 30, 30, 30, 30, 30};

// this int will be a bit-wise mask for the drums
// the first 8 bits will represent if the drums are active or not
unsigned short OilDrum_active = 0;

// is the oil drum settled on the map?
unsigned short OilDrum_settled = 0;

PhysObj OilDrum_physObj[MAX_OILDRUMS];
char OilDrum_xVelo[MAX_OILDRUMS] = {0, 0, 0, 0, 0, 0};
char OilDrum_yVelo[MAX_OILDRUMS] = {0, 0, 0, 0, 0, 0};



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * spawns an OilDrum with the given index
 *
 * @param the index of the OilDrum to spawn
*/
static void spawnDrum(short index)
{
	// find a free place for it on the map
	short sx, sy;
	if(Map_findSpawnPoint(SPAWN_DRUM, &sx, &sy)) {
		OilDrum_x[index] = sx;
		OilDrum_y[index] = sy;
	} else {
		return;
	}
	
	// health is already set in the header file
	// so let's just enable it:
	OilDrum_active |= (unsigned short)1<<(index);
	
	// make a new collider and physics object for this oil drum
	new_Collider(&(OilDrum_physObj[index].col), COL_DOWN, 0, 5, 0, 0);
	new_PhysObj(&OilDrum_physObj[index], &OilDrum_x[index], &OilDrum_y[index], &OilDrum_xVelo[index], &OilDrum_yVelo[index], 0, 100, (char)index, &OilDrum_settled);
	
}



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * spawns OilDrums on the map, if they're enabled
 */
void OilDrums_spawnDrums()
{
	short i=0;
	for(i=0; i<MAX_OILDRUMS; i++)
		spawnDrum(i);
}


/**
 * main update for oil drums
 */
void OilDrums_update()
{
	// if any of the active OilDrums have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<MAX_OILDRUMS; i++)
	{
		// check if enabled and health is <= 0... then boom
		char enabled = (char)((OilDrum_active & (unsigned short)1<<(i)) > 0);
		if(enabled)
		{
			// check all explosions if they are near-by and damaging this oil drum
			short damage = Physics_checkExplosions(&OilDrum_physObj[i]);
			OilDrum_health[i] -= damage;
			
			// if ded
			if(OilDrum_health[i]<=0)
			{
				// boom
				Explosion_spawn(OilDrum_x[i], OilDrum_y[i], 10, 10, TRUE);
				
				// no longer active
				OilDrum_active &= ~((unsigned short)1<<(i));
				
				// nothing left to check on this drum
				continue;
			}// end if health<=0
			
			// if the OilDrum is considered "settled" no need for physics
			if(!(OilDrum_settled & (unsigned short)1<<(i)))
			{
				// add gravity to oil drum
				OilDrum_yVelo[i]++;
				
				// do physics and collision for OilDrum
				Physics_apply(&OilDrum_physObj[i]);
			}
			
		}// end if active		
	}// next i
}


/**
	Draws all the in-game, on-screen Oil Drums.
*/
void OilDrums_drawAll()
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
				GrayClipSprite16_AND_R(screenX-4, screenY-5, 12, spr_OilMask, spr_OilMask, lightPlane, darkPlane);
				GrayClipSprite16_OR_R(screenX-4, screenY-5, 12, spr_OilLight, spr_OilDark, lightPlane, darkPlane);
				
				
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
