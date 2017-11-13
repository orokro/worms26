// C Source File
// Created 11/12/2017; 8:33:12 PM

#include "../MainSubFolder.h"

// function prototypes
void updateExplosion(short);
void spawnFire();

// spawns a new explosion
void Explosion_spawn(short x, short y, char size, char power, char hasFire)
{
	// first we gotta find a free explosion. if none is found,
	// we'll over-write the current closest-to-death explosion
	short expIndex = -1;
	short lowestTimeIndex = -1;
	short i=0;
	for(i=0; i<8; i++)
	{
		// if it's active, we can just used that!
		if(Explosion_time[i]<=0)
		{
			expIndex = i;
			break;
		}else
		{
		
			// if its not yet set, or its a new lowest time, save this index
			if(lowestTimeIndex==-1)
				lowestTimeIndex=i;
			else if(Explosion_time[i]<Explosion_time[lowestTimeIndex])
				lowestTimeIndex=i;
		}		
	}// next i
	
	/*
		If the explosion index was not set (still -1) then all explosions must be in use
	  we will just use the one that was closest to being over.
	  further-more we will just set it's frame to 1 (last frame) and call it's update manually
	  this will have the affect of erasing the map the full-size and damaging worms or items
	  
	 	Essentially, we will be skipping it's inbetween animation frames, and skipping to the last
	 	frame of our explosion before we replace it!
	*/
	if(expIndex==-1)
	{
		// we'll use the lowest time index
		expIndex = lowestTimeIndex;
		
		// let's set that explosion to it's last frame and call it's update right now
		Explosion_time[expIndex] = 1;
		updateExplosion(expIndex);
	}
	
	// set new parameters for explosion:
	Explosion_x[expIndex] = x;
	Explosion_y[expIndex] = y;
	Explosion_size[expIndex] = size;
	Explosion_power[expIndex] = power;
	
	// set first-frame bit for this explosion to 1 (TRUE)
	Explosion_firstFrame |= (int)1<<(expIndex);
	
	// if this explosion has fire, let's spawn it now
	if(hasFire==TRUE)
		spawnFire();
}

// spawns fire
void spawnFire()
{
		
}


// updates active explosions
void Explosion_update()
{
	// update all our explosions, (if active)
	short i=0;
	for(i=0; i<8; i++)
		if(Explosion_time[i]>0)
			updateExplosion(i);
}

// updates a single explosion
void updateExplosion(short index)
{
	// decrease it's time
	Explosion_time[index]--;
	
	// no matter what, it's not a first-frame any more
	Explosion_firstFrame &= ~((int)1<<(index));
	
	// calculate it's current radius
	short radius = Explosion_size[index] - Explosion_time[index];
	
	// draw the explosion on our MapBuffers to erase map
	// TO-DO: implement explosion drawing

}