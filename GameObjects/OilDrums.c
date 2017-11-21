// C Source File
// Created 11/12/2017; 8:33:19 PM

#include "../Headers/System/Main.h"

/*
	OilDrums
	--------
	
	This defines our OilDrums on the map.
	
	About OilDrums:
	
	They can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	OilDrums have no velocity - they can fall down, but not moved otherwise.
	
	OilDrums have health, and will explode if drained of health.
*/

// x/y positions of our oil drums
short OilDrum_x[8] = {0, 0, 0, 0, 0, 0, 0, 0};
short OilDrum_y[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// health of our oil drums
char OilDrum_health[8] = {30, 30, 30, 30, 30, 30, 30, 30};

// this int will be a bit-wise mask for the drums
// the first 8 bits will represent if the drums are active or not
int OilDrum_active = 0;

// local function prototypes
void spawnDrum(short);
void checkExplosions(short);



// --------------------------------------------------------------------------------------------------------------------------------------



// spawns OilDrums on the map, if they're enabled
void OilDrums_spawnDrums()
{
	if(Match_oilDrumsEnabled==TRUE)
	{
		short i=0;
		for(i=0; i<8; i++)
			spawnDrum(i);
	}
}

// spawns a single OilDrum
void spawnDrum(short index)
{
	// find a free place for it on the map
	Map_getSpawnPoint();
	
	// save the last requested point:
	OilDrum_x[index] = Map_lastRequestedSpawnX;
	OilDrum_y[index] = Map_lastRequestedSpawnY;
	
	// health is already set in the header file
	// so let's just enable it:
	OilDrum_active |= (int)1<<(index);
}

// main update for oil drums
void OilDrums_update()
{
	// if any of the active OilDrums have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<8; i++)
	{
		// check if enabled and health is <= 0... then boom
		char enabled = (char)((OilDrum_active & (int)1<<(i)) > 0);
		if(enabled==TRUE)
		{
			
			// check all explosions if they are near-by and damaging this oildrum
			checkExplosions(i);
			
			// if ded
			if(OilDrum_health[i]<=0)
			{
				// boom
				Explosion_spawn(OilDrum_x[i], OilDrum_y[i], 10, 10, TRUE);
				
				// no longer active
				OilDrum_active &= ~((int)1<<(i));
				
				// nothing left to check on this drum
				continue;
			}// end if health<=0
		}// end if active		
	}// next i
}

// loop over explosions, and if there are any first-frame explosions, see if they hit us
void checkExplosions(short index)
{
	short i=0; 
	for(i=0; i<8; i++)
	{
		// check if the explosion is in it's first-frame
		char firstFrame = (char)((Explosion_firstFrame & (int)1<<(i))>0);
		
		// only do shit if first frame, yo
		if(firstFrame==TRUE)
		{
				
			// if it's in it's first frame, calculate the distance from us to it:
			short d = dist(OilDrum_x[index], OilDrum_y[index], Explosion_x[i], Explosion_y[i]);
			
			// if we're withing the blast radius, take full damage:
			if(d<Explosion_size[i])
			{
				OilDrum_health[index] -= Explosion_power[i];
				continue;
			}// end if within exp radius
			
			// calculate the extranius radius, with a power-fall off
			short largerRadius = (short)(1.5f * Explosion_size[i]);
			if(d<largerRadius)
			{
				// subract the minimum radius from both:
				short minD = d - Explosion_size[i];
				short minL = largerRadius - Explosion_size[i];
				
				// calculate how far away we are:
				float distFallOffRatio = (1.0f - ((float)minD/(float)minL));
				
				// apply only this much damage:
				OilDrum_health[index] -= (Explosion_power[i] * distFallOffRatio);
				
				continue;
			}// end if within larger radius			
		}// end if first frame		
	}// next i
}