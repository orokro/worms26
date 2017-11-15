#include "../Main.h"

// function prototypes
void spawnCrate(char);
void checkExplosions(short);

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

// spawns a single Crate
void spawnCrate(char type)
{

	// find a free spot in our array of crates to spawn one, 
	// we will assume that crates with less than 0 health are unactive
	short index=-1;
	short i=0;
	for(i=0; i<8; i++)
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

// main update for Crates
void Crates_update()
{
	// if any of the active Crates have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<8; i++)
	{
		// check if enabled and health is <= 0... then boom
		char enabled = (char)((Crate_active & (int)1<<(i)) > 0);
		if(enabled==TRUE)
		{
			
			// check all explosions if they are near-by and damaging this oildrum
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