// C Source File
// Created 11/12/2017; 8:33:19 PM

#include "../MainSubFolder.h"

// function prototypes
void spawnDrum(short);

// spawns OilDrums on the map, if they're enabled
OilDrum_spawnDrums()
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
void OilDrum_update()
{
	// if any of the active OilDrums have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<8; i++)
	{
		// check if enabled and health is <= 0... then boom
		char enabled = (char)((OilDrum_active & (int)1<<(i)) > 0);
		if(enabled==TRUE && OilDrum_health[i]<=0)
		{
			// boom
			Explosion_spawn(OilDrum_x[i], OilDrum_y[i], 10, 10, TRUE);
			
			// no longer active
			OilDrum_active &= ~((int)1<<(i));
		}
	}// next i
}