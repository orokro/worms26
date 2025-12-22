// C Source File
// Created 11/12/2017; 8:33:12 PM

#include "Main.h"
#include "Explosions.h"
#include "Map.h"

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
 * Erases the map for a given explosion
 *
 * @param index the index of the explosion to erase from the map
*/
void eraseMap(short index)
{
	/*
		To erase from the map, we will set up a virtual screen and use tigcc's eclispe
		methods to draw the explosions circle.
		
		Then we will do a pass over the virtual screen and map buffers and use bitwise
		operations to remove the circle area from our map buffers (light and dark)
		
		NOTE: we cannot use the DrawClipEllipse method on our map buffers because their
		byte order is different than a screen buffer, for fast drawing routines.
		
		Finally we will draw a couple more outer layers of circles and loop over the
		buffers again to add a burned edge to the map.
	*/
	
	// get explosion details
	short x = Explosion_x[index];
	short y = Explosion_y[index];
	short s = Explosion_size[index];

	// determine the pixel edges of our explosion:
	short t = y-s;
	short b = y+s;
	short l = x-s;
	short r = x+s;
	
	// determine the columns the left and right are in
	short lCol = (l-(l%32))/32;
	short rCol = (r-(r%32))/32;
	
	// make virtual screen the size of our explosion
	void *virtual=malloc(LCD_SIZE);
	
	// set it to be our virtual screen
	PortSet(virtual, 239, 127);
	
	// clear our virtual screen
	ClrScr();
	
	// convert the x and y position of the explosion to be relative to our mini buffer
	x -= (lCol*32);
	y -= t;
	
	// loop to draw a bunch of eclipses
	short e;
	for(e=1; e<s; e++)
	{
		DrawClipEllipse(x, y, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
		DrawClipEllipse(x, y-1, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
	}
	
	/*
		now, our map buffers are arranged such that every 200 unsigned longs are a
		vertical column, with 10 columns for a 320 wide map.
		
		The buffer will be arranged such that every colWidth unsinged longs will
		be a row.
		
		Thus we will loop over the proper unsigned longs in both the map and our
		explosion buffer and copy remove pixels from our map buffer.
	
	*/
	
	// referencs to our map buffers, and explosion buffer
	unsigned long *light = (unsigned long*)mapLight;
	unsigned long *dark = (unsigned long*)mapDark;
	
	// loop over our rows
	short row, col;
	for(row=t; row<=b; row++)
	{
		// loop over the columns in this row:
		for(col=lCol; col<=rCol; col++)
		{
			// read the unsigned long from both our light and dark buffers
			unsigned long ulLight = light[(col*200)+row];
			unsigned long ulDark = dark[(col*200)+row];
			
			// move to the offset in the virtual:
			unsigned short *expShort = (unsigned short*)virtual;
			unsigned long *expLong = (unsigned long*)(expShort+((15*(row-t))+((col-lCol)*2)));
			
			// read the corresponding unsinged long from the explosion buffer,
			// and flip its bits, so that the exposion is 0 and everything else is 1
			unsigned long ulExp = ~(*expLong);
			
			// if we AND the inverted exposlion bits, the map should be erased:
			light[(col*200)+row] = ulLight & ulExp;
			dark[(col*200)+row] = ulDark & ulExp;
			
		}// next col
	}// next row
	
	// loop to draw extra eclipses to make a burned edge of the map
	for(e=s; e<s+2; e++)
	{
		DrawClipEllipse(x, y, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
		DrawClipEllipse(x, y-1, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
	}
	
	// now we do another pass to add the burned edge
	for(row=t; row<=b; row++)
	{
		// loop over the columns in this row:
		for(col=lCol; col<=rCol; col++)
		{
			// read the unsigned long from both our light and dark buffers,
			// and combine them into one that represents the map at that lixe
			unsigned long ulLight = light[(col*200)+row];
			unsigned long ulDark = dark[(col*200)+row];
			unsigned long ulMap = ulLight | ulDark;

			// move to the offset in the virtual:
			unsigned short *expShort = (unsigned short*)virtual;
			unsigned long *expLong = (unsigned long*)(expShort+((15*(row-t))+((col-lCol)*2)));
			
			// read the corresponding unsinged long from the explosion buffer
			unsigned long ulExp = (*expLong);
			
			// if we AND the exposlion bits, the map should add 1s to both layers where
			// it overlays, but it will erase the map elsewhere so we can OR the layer back on
			light[(col*200)+row] = (ulMap & ulExp) | ulLight;
			dark[(col*200)+row] = (ulMap & ulExp) | ulDark;
			
		}// next col
	}// next row
	
	// clean up time
	PortRestore();
	free(virtual);
}



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

	// if the explosions time is up, destory the map and set it inactive
	if(Explosion_time[index]<0)
	{
		eraseMap(index);
		Explosion_active &= ~((unsigned short)1<<(index));
	}
		
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


