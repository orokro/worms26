/*
	Explosions.c
	------------
	
	This defines Explosions
	
	About Explosions:
	
	They will have a max-radius size
	They will have a damage amount, more powerful explosions will propel worms further
	They can optionally spawn fire-particles
	
	C Source File
	Created 11/12/2017; 8:33:12 PM
*/


// includes
#include "Main.h"
#include "Explosions.h"
#include "Map.h"
#include "Draw.h"
#include "Camera.h"


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
 * Digs a hole in the map
 * 
 * @param x x position
 * @param y y position
 * @param s radius
 * @param border if true, adds a dark border around the hole
 */
void Explosion_dig(short x, short y, short s, char border)
{
	// determine the pixel edges:
	short t = y-s;
	short b = y+s;
	short l = x-s;
	short r = x+s;
	
	// determine the columns
	short lCol = (l-(l%32))/32;
	short rCol = (r-(r%32))/32;
	
	// make virtual screen
	void *virtual=malloc(LCD_SIZE);
	if(virtual == NULL) return;
	
	PortSet(virtual, 239, 127);
	ClrScr();
	
	short drawX = x - (lCol*32);
	short drawY = y - t;
	
	// Pass 1: Draw inner hole
	short e;
	for(e=1; e<s; e++)
	{
		DrawClipEllipse(drawX, drawY, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
		DrawClipEllipse(drawX, drawY-1, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
	}
	
	unsigned long *light = (unsigned long*)mapLight;
	unsigned long *dark = (unsigned long*)mapDark;
	
	short startRow = (t < 0) ? 0 : t;
	short endRow = (b > 199) ? 199 : b;
	short startCol = (lCol < 0) ? 0 : lCol;
	short endCol = (rCol > 9) ? 9 : rCol;

	short row, col;
	for(row=startRow; row<=endRow; row++)
	{
		for(col=startCol; col<=endCol; col++)
		{
			unsigned long ulLight = light[(col*200)+row];
			unsigned long ulDark = dark[(col*200)+row];
			
			unsigned short *expShort = (unsigned short*)virtual;
			unsigned long *expLong = (unsigned long*)(expShort+((15*(row-t))+((col-lCol)*2)));
			
			unsigned long ulExp = ~(*expLong);

			light[(col*200)+row] = ulLight & ulExp;
			dark[(col*200)+row] = ulDark & ulExp;
		}
	}
	
	if(border)
	{
		// Pass 2: Draw border rings
		for(e=s; e<s+2; e++)
		{
			DrawClipEllipse(drawX, drawY, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
			DrawClipEllipse(drawX, drawY-1, e, e, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
		}

		for(row=startRow; row<=endRow; row++)
		{
			for(col=startCol; col<=endCol; col++)
			{
				unsigned long ulLight = light[(col*200)+row];
				unsigned long ulDark = dark[(col*200)+row];
				unsigned long ulMap = ulLight | ulDark;

				unsigned short *expShort = (unsigned short*)virtual;
				unsigned long *expLong = (unsigned long*)(expShort+((15*(row-t))+((col-lCol)*2)));
				
				// Read the newly expanded explosion mask (not inverted)
				unsigned long ulExp = (*expLong);

				// Add border (set both layers to 1 where mask is 1)
				// Since we already erased the center, this only affects the rim
				light[(col*200)+row] = (ulMap & ulExp) | ulLight;
				dark[(col*200)+row] = (ulMap & ulExp) | ulDark;
			}
		}
	}
	
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
	Explosion_time[index]-=3;

	// if the explosions time is up, destroy the map and set it inactive
	if(Explosion_time[index]<0)
	{
		// erase the map (dig with border)
		Explosion_dig(Explosion_x[index], Explosion_y[index], Explosion_size[index], TRUE);
		Explosion_active &= ~((unsigned short)1<<(index));
		Camera_clearIf(&Explosion_x[index], &Explosion_y[index]);
	}
		
}


/**
 * Spawns fire at an explosion point, for explosions that use fire.
 *
 * Explosions, such as those from OilDrums or Molotov Cocktails spawn fire particles
*/
void spawnFire()
{
		
}



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * spawns a new explosion
 * @param x - x location to spawn at
 * @param y - y location to spawn at
 * @param size - size of explosion to spawn
 * @param hasFire - true if has fire effects
 */
short Explosion_spawn(short x, short y, char size, char power, char hasFire)
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
	  
	 	Essentially, we will be skipping it's in between animation frames, and skipping to the last
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
	
	// focus on explosion
	Camera_focusOn(&Explosion_x[expIndex], &Explosion_y[expIndex]);

	// explosions wont have their "first frame" until next frame
	Explosion_firstFrame &= ~((unsigned short)1<<(expIndex));
	
	// set this explosion active
	Explosion_active |= (unsigned short)1<<(expIndex);
	
	// if this explosion has fire, let's spawn it now
	if(hasFire)
		spawnFire();

	return expIndex;
}


/**
 * updates active explosions
 */
void Explosion_update()
{
	// update all our explosions, (if active)
	short i=0;
	for(i=0; i<MAX_EXPLOSIONS; i++)
		if(Explosion_active & (unsigned short)1<<i)
		{
			Camera_focusOn(&Explosion_x[i], &Explosion_y[i]);
			updateExplosion(i);
		}			
}


/**
 * Draws active explosions, if there are any
*/
void Explosion_drawAll()
{
	// if no explosion is active, just GTFO
	if(Explosion_active==0)
		return;
		
	// loop over explosions..
	short i;
	for(i=0; i<MAX_EXPLOSIONS; i++)
	{
		if(Explosion_active & (unsigned short)1<<i)
		{
			short x=Explosion_x[i];
			short y=Explosion_y[i];
			short r=Explosion_size[i]-Explosion_time[i];
			
			if(worldToScreen(&x, &y))
			{
				short e;
				for(e=1; e<r; e++)
				{
					GrayDBufSetHiddenAMSPlane(DARK_PLANE);
					DrawClipEllipse(x, y, e, e, &(SCR_RECT){{0, 0, 159, 99}}, A_XOR);
					GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
					DrawClipEllipse(x, y, e, e, &(SCR_RECT){{0, 0, 159, 99}}, A_XOR);
				}
			}
			
		}// end if active

	}// next i
}


/**
 * returns the index of the first active explosion, or -1 if none are active
 * @return char index of first active explosion
 */
char Explosion_getFirstActive()
{
	short i;
	for(i=0; i<MAX_EXPLOSIONS; i++)
	{
		if(Explosion_active & (unsigned short)1<<i)
			return (char)i;
	}
	
	return -1;
}