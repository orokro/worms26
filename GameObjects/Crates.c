#include "Main.h"

/*
	Crates
	------
	
	This defines Crates on the map.
	
	About Crates:
	
	There are 3 types of crates: weapon, health, tool.
	Each type can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	Crates have no velocity - they can fall down, but not moved otherwise.
	
	Crates have health, and will explode if drained of health.
*/

// x/y positions of our Crates
short Crate_x[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};
short Crate_y[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};

// health of our crates
char Crate_health[MAX_CRATES] = {-1, -1, -1, -1, -1, -1, -1, -1};

// type of each Crate
char Crate_type[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};

// is the crate active?
int Crate_active=0;



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Spawns a single crate of the given type
 *
 *@ param type the type of crate to spawn
*/
static void spawnCrate(char type)
{

	// find a free spot in our array of crates to spawn one, 
	// we will assume that crates with less than 0 health are unactive
	short index=-1;
	short i=0;
	for(i=0; i<MAX_CRATES; i++)
	{
		char enabled = (char)((Crate_active & (int)1<<(i)) > 0);
		if(enabled==TRUE)
		{
			index = i;
			break;	
		}
	}

	// if we didn't find a free Crate slot, that means the maximum of 8 are already
	// spawned on the map. Just exit
	if(index==-1)
		return;

	// find a free place for it on the map
	Map_getSpawnPoint();

	// save the last requested point:
	Crate_x[index] = Map_lastRequestedSpawnX;
	Crate_y[index] = Map_lastRequestedSpawnY;

	// save the type!
	Crate_type[index] = type;

	// reset health:
	Crate_health[index] = 30;

	// set active!
	Crate_active |= (int)1<<(index);
}


/**
 * Checks if nearby explosions affect any of the given Crates index.
 *
 * @param index the index of the Crate to check for nearby explosions.
 */
static void checkExplosions(short index)
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
			short d = dist(Crate_x[index], Crate_y[index], Explosion_x[i], Explosion_y[i]);
			
			// if we're withing the blast radius, take full damage:
			if(d<Explosion_size[i])
			{
				Crate_health[index] -= Explosion_power[i];
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
				Crate_health[index] -= (Explosion_power[i] * distFallOffRatio);
				
				continue;
			}// end if within larger radius
		}// end if first frame
	}// next i
}


// --------------------------------------------------------------------------------------------------------------------------------------


// spawns a Crate on the map, if they're enabled
void Crates_spawnCrate(char type)
{
	if(
			(type==crateHealth && Match_healthCratesEnabled==TRUE)
			||
			(type==crateWeapon && Match_weaponCratesEnabled==TRUE)
			||
			(type==crateTool && Match_toolCratesEnabled==TRUE)
		)
			spawnCrate(type);
}

// main update for Crates
void Crates_update()
{
	// if any of the active Crates have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<MAX_CRATES; i++)
	{
		// check if enabled and health is <= 0... then boom
		char enabled = (char)((Crate_active & (int)1<<(i)) > 0);
		if(enabled==TRUE)
		{

			// check all explosions if they are near-by and damaging this crate
			checkExplosions(i);

			// if ded
			if(Crate_health[i]<=0)
			{
				// boom
				Explosion_spawn(OilDrum_x[i], OilDrum_y[i], 10, 10, TRUE);

				// no longer active
				Crate_active &= ~((int)1<<(i));

				// nothing left to check on this drum
				continue;
			}// end if health<=0
		}// end if active
	}// next i
}
