// C Source File
// Created 11/12/2017; 8:33:12 PM

#include "Main.h"

/*
	Explosions
	----------
	
	This defines Explosions
	
	About Explosions:
	
	They will have a max-radius size
	They will have a damage amount, more powerful explosions will propell worms further
	They can optionally spawn fire-particles
*/

// x/y positions of our Explosions
short Explosion_x[MAX_EXPLOSIONS] = {0, 0, 0, 0, 0, 0, 0, 0};
short Explosion_y[MAX_EXPLOSIONS] = {0, 0, 0, 0, 0, 0, 0, 0};

// current time of our explosion
// note that we will consider <=0 time an inactive explosion
char Explosion_time[MAX_EXPLOSIONS] = {0, 0, 0, 0, 0, 0, 0, 0};

// the size of the explosion
char Explosion_size[MAX_EXPLOSIONS] = {0, 0, 0, 0, 0, 0, 0, 0};

// the damage power of the explosion
char Explosion_power[MAX_EXPLOSIONS] = {0, 0, 0, 0, 0, 0, 0, 0};

// if any explosion is active
unsigned short Explosion_active = 0;

/*
	this int will store a bitmask for the first-frame of an explosion
	when an explosion is created, it will set it's corresponding bit to 1
	after it's first update the bit will be zero.
	
	since explosions update AFTER other objects, they will have a chance to see the first
	frame of an explosion.
	
	explosions will only cause damage on the first frame
*/
unsigned short Explosion_firstFrame = 0;




// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Updates an individual explosion at the given index.
 *
 * @param index the explosion to update.
*/
void updateExplosion(short index)
{
	// if our time and size are the same, this is the first frame of this explosion
	if(Explosion_time[index]==Explosion_size[index])
		Explosion_firstFrame |= (unsigned short)1<<(index);
	else
		Explosion_firstFrame &= ~((unsigned short)1<<(index));
	
	// decrease it's time
	Explosion_time[index]--;

	// if the explosions time is up, set it inactive
	if(Explosion_time[index]<0)
		Explosion_active &= ~((unsigned short)1<<(index));
}


/**
 * Spawns fire at an explosion point, for explosions that use fire.
 *
 * Explosions, such as those from OilDrums or Molotov Cocktains spawn fire particles
*/
void spawnFire()
{
		
}



// --------------------------------------------------------------------------------------------------------------------------------------



// spawns a new explosion
void Explosion_spawn(short x, short y, char size, char power, char hasFire)
{
	// first we gotta find a free explosion. if none is found,
	// we'll over-write the current closest-to-death explosion
	short expIndex = -1;
	short lowestTimeIndex = -1;
	short i=0;
	for(i=0; i<MAX_EXPLOSIONS; i++)
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
	Explosion_time[expIndex] = size;
	Explosion_power[expIndex] = power;
	
	// explosions wont have their "first frame" until next frame
	Explosion_firstFrame &= ~((unsigned short)1<<(expIndex));
	
	// set this explosion active
	Explosion_active |= (unsigned short)1<<(expIndex);
	
	// if this explosion has fire, let's spawn it now
	if(hasFire)
		spawnFire();
}


// updates active explosions
void Explosion_update()
{
	// update all our explosions, (if active)
	short i=0;
	for(i=0; i<MAX_EXPLOSIONS; i++)
		if(Explosion_active & (unsigned short)1<<i)
			updateExplosion(i);
}


