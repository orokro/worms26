/*
	Map.c
	-----
	
	Handles all the logic for the Map including drawing it
	
	The map will be made up of a large buffer, four times the screen size
	(double vertical and double horizontal)

	The map will be generated in this buffer, and when explosions erase land,
	the land will be removed from this buffer.

	Pixels in this buffer will be tested for collision.
	
	C Source File
	Created 11/11/2017; 11:34:08 PM
*/

// includes
#include "Main.h"
#include "Map.h"
#include "Draw.h"
#include "Worms.h"
#include "OilDrums.h"
#include "Mines.h"



/*
	Below, these array values will store eligible spawn points on the map.
	
	Everytime an object requests a spawn point we will pick from one of
	these, then deactivate it.
	
	There can be a total of 8 Oil Drums, 10 Mines, and 16 worms
	at the start of a game.
	
	Crates spawn later, so we don't have to worry about them
	
	We need a minimum of 8+10+16 spawn points, or 34 spawn points.
	
	But since the map generates these as it goes, it should generate
	more than necessary so we can chose from them randomly, and not
	have all the worms next to each other.
	
	The map is 320 pixels wide, and has an upper level and a lower level.
	
	Mines always need to be at least 10 pixels away from a worm, since
	mines trigger at 10px.
	
	Thus, if we generate spawn points, every 12 pixels on both levels,
	we would get: (320/12)*2 spawn points, or 53 spawn points.
	
	This will give us 19 extra. Some areas of land wont be spawnable,
	if its an open hole to the level below, or to the water.
	
	We need to make sure that we don't generate more than 19*12 pixels
	of open space.
*/ 
short spawnPoint_x[53];
short spawnPoint_y[53];


/*
	the map will keep an array of 620 tiles, (32 x 20) which represent
	10x10 tiles on the map. When mines or crates spawn or move, they
	will set their id in the tile they're in.
	
	This way, we can only check the crates and mines the tiles are in
	for this worm.
*/
unsigned char mapTiles[640];
	
	
// when a spawn point is requested of the map, it will find one
// and update these global map variables:
short Map_lastRequestedSpawnX=0;
short Map_lastRequestedSpawnY=0;


// the texture pattern for the land
static unsigned long spr_LandTexture[] = { // 32 tall
	0b11000000100000000000001000000000,
	0b01110000000111111110000000010010,
	0b01111111111111111111100001000000,
	0b00011111111111111111111110000000,
	0b10000011111111100000111111100010,
	0b00010000100000000100000111110000,
	0b00000000000000100000001000000000,
	0b01000011111100000000100001000010,
	0b00001111110000010001111100000000,
	0b11111110000010000011111111111111,
	0b11111100100000000010000111111111,
	0b11110000001001001000100000000111,
	0b00000000000000000000000100010000,
	0b10000010111110000000000000000000,
	0b00010001111111111100100001000010,
	0b00000011111111111111111000000000,
	0b00100111100001111111111111111110,
	0b00000000000010001111111111111000,
	0b00000001000000000000000000000001,
	0b00000000000100000010001000000000,
	0b00111111100000010000000001001000,
	0b11111111111000000001000000000011,
	0b11111111111110000000000000100111,
	0b00100011111111100001111100000111,
	0b00000000001111111111111000001000,
	0b01000100000011111110000001000000,
	0b11100000100000000000001000000111,
	0b11111000000000100010000000111111,
	0b01111100001000000001111111111110,
	0b00001110000000111111111111110000,
	0b00100000100100011111111110000100,
	0b00000000000000000000000001000000};
	
	

// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * trace the edges of the map with a black line
 */
void Map_traceEdges()
{
	// iteration vars
	short x, y;
	
	// map buffer reference as unsigned long
	unsigned long *light = (unsigned long*)mapLight;
	unsigned long *dark = (unsigned long*)mapDark;
	
	// make a mask that looks like 10000000000000000000000000000001
	unsigned long changeEdges = 1;
	changeEdges = changeEdges | changeEdges<<(31);
	
	// loop thru every 32bit value in our 32 bit y columns
	for(x=0; x<10; x++)
	{
		// update loading screen
		Draw_cake(320+(x*20), 620);
		
		// at the top of every colum, this is 0 since it's "outside" the top of the buffer
		unsigned long lastValues = 0;
		
		// now we need to loop over the map and calculate dark-pixels for the maps edges
		for(y=0; y<200; y++)
		{			
			// the long index to sample from
			unsigned short pos = (x*200)+y;
			
			// get current 32 pixels
			unsigned long current = light[pos] | dark[pos];
			
			// get the pixels that were previously above these pixels
			// note that we cant use (light[pos-1] | dark[pos-1]) because we might have changed pixels and we'll get feedback
			// we need to use lastValues so we know what the pixels were before they were changed
			unsigned long above = lastValues;
			
			// we can just read the next row of pixels below these... (pos+1)
			unsigned long below = (y<200) ? (light[pos+1] | dark[pos+1]) : 0;
			
			// see which pixels change from the above and below row to the current:
			unsigned long changedAbove = current ^ above;
			unsigned long changedBelow = current ^ below;
			
			// we can see which pixels changed horizontally by comparing with bit-shifts
			// note that, on the edegs of our 32 bit map columns it will fail to draw horizontally changed pixels edge
			// it will still draw vertical changed pixels, however
			unsigned long changedLeft = current ^ current<<1;
			unsigned long changedRight = current ^ current>>1;
			
			// add them to get total pixels changed:
			// note that we turn the edge pixels OFF if they are on in our current segment
			// this is to prevent it from always drawing borders every 32 px
			unsigned long changed = (changedAbove | changedBelow | changedLeft | changedRight) & ~(changeEdges & current);
			
			// subtract pixels that are currently off, because we don't
			// want to darken empty pixels
			changed &= ~current;
			
			// darken the vertically changed pixels in both buffers:
			//light[pos] |= changed;
			dark[pos] |= changed;
			
			// save for next loop, so it can see what was above
			lastValues = current;
		}// next x
	}// next y
}


/**
 * builds a random map for the worms to play on
 */
void Map_makeMap()
{
	// gotta be random!
	randomize();
	
	// iteration vars
	short x, y;
	
	// map buffer reference as unsigned long
	unsigned long *light = (unsigned long*)mapLight;
	unsigned long *dark = (unsigned long*)mapDark;
	
	// before we generate the map, lets clear the memory entirely
	// clear the buffer entirely:
	for(x=0; x<2000; x++)
	{
		light[x]=0;
		dark[x]=0;
	}
		
	
	/*
		Generating the map...
		
		The map will consist of drawing a series of vertical lines next to each other.
		Each vertical column of pixels will be slightly higher or lower than the ones next to it.
		
		On the top of the map, the lines will have both a top and bottom height.
		
		On the bottom of the map, the lines will go all the way to the bottom of the buffer, and only require a height.
		
		Sometimes, if the lines goto 0 height, this would be a hole in the land, leading the level below, or to water.
		
		We will loop from the left edge of the buffer, to the right edge of the buffer and set pixels at various heights
		
		Since the buffer will be updated 1 unsigned short at a time, we will generate the pixels for one colum at a time
		then copy to the map.
		
		As we go, every 12 pixels we will add a spawn-point for the upper and lower levels.
		
		We need a minum of 34 spawn-points, so we can spawn everything (16 worms, 8 oil drums, 10 mines)
		
		If we generate land that is 0 height, we cant spawn on there. We, therefore, can't spawn more than 19*12 0 height
		land points. We will keep track of 0 height land and if we exceed 19*12 pixels worth, stop allowing 0 height land.
	*/
	
	// initialize our spawn point buffers:
	for(x=0; x<53; x++)
	{
		spawnPoint_x[x]=-1;
		spawnPoint_y[x]=-1;
	}
	
	// keep track of empty land:
	unsigned char emptyLand=0;
	
	// keep track of current spawn-point index:
	char spawnIndex=0;
	
	// the positions of the lines to draw:
	char upperLineTop=0;
	char upperLineBottom=0;
	char lowerLineTop=30;
	
	// the map hills will either go up hill or down hill, or flat
	// for a set number of pixels. This will avoid super bumpy terrain
	char upperTopDir = 1;
	char upperBotDir = 1;
	char lowerTopDir = -1;
	
	char upperTopLength = random(7);
	char upperBotLength = random(7);
	char lowerTopLength = random(7);
	
	
	// loop from left of buffer to right of buffer
	for(x=0; x<320; x++)
	{
		
		// update loading screen
		if(x%20==0)
			Draw_cake(x, 620);
		
		/*
		  move the lines randomly.
			note that these are relative positions - that is, the upper areas
			will always be +/- 50, and the lower area always +/-150
			
			Also note that, the upperLineTop and upperLineBottm can never go below 0
			as they are subtracted from their relative position.
		*/
		upperLineTop += upperTopDir * random(3);
		upperLineBottom += upperBotDir * random(3);
		lowerLineTop += lowerTopDir * random(3);
		
		// decrement their length times, and change direction if need be
		if(upperTopLength--<=0)
		{
			upperTopLength = random(5)+5;
			upperTopDir = random(2) ? 1 : -1; // -1 to 1
			// if its at its limit, make it go down
			if(upperLineTop>14)
				upperTopDir=-1;
			// if less than 50, always go up:
			if(x<50)
				upperTopDir=1;
			// if greater than 320-50, always go down
			else if(x>320-50)
				upperTopDir=-1;
		}
		if(upperBotLength--<=0)
		{
			upperBotLength = random(5)+5;
			upperBotDir = random(2) ? 1 : -1; // -1 to 1
			// if its at its limit, make it go down
			if(upperLineBottom>24)
				upperBotDir=-1;
			// if less than 50, always go up:
			if(x<50)
				upperBotDir=1;
			// if greater than 320-50, always go down
			else if(x>320-50)
				upperBotDir=-1;
		}
		if(lowerTopLength--<=0)
		{
			lowerTopLength = random(5)+5;
			lowerTopDir = random(2) ? 1 : -1; // -1 to 1
			// if its at its limit, make it go down
			if(lowerLineTop<-14)
				lowerTopDir=1;
			// if less than 50, always go up:
			if(x<50)
				lowerTopDir=-1;
			// if greater than 320-50, always go down
			else if(x>320-50)
				lowerTopDir=1;
		}
		
		// make sure the generated sections are inbounds
		if(upperLineTop<0) upperLineTop=0;
		else if(upperLineTop>20) upperLineTop = 20;
		if(upperLineBottom<0) upperLineBottom=0;
		else if(upperLineBottom>30) upperLineBottom = 30;
		if(lowerLineTop>30) lowerLineTop=30;
		else if(lowerLineTop<-20) lowerLineTop=-20;
		
		// if we are no longer allowed to generate empty land,
		// set new minimums
		if(emptyLand>228)
		{
			if(upperLineTop<5) upperLineTop=5;
			if(lowerLineTop>20) lowerLineTop=20;
		}else
		{
			// calculate if either land sections are "0" height:
			if(lowerLineTop>=30)
				emptyLand++;
			if(upperLineTop==0 && upperLineBottom==0)
				emptyLand++;
		}
		
		// if its time to make spawn points..
		// just generate them at our current x, and the tops of the current land
		if(x%12==0 && spawnIndex<53)
		{
			// upper spawn point, if land isn't 0:
			if(!(upperLineTop==0 && upperLineBottom==0))
			{
				spawnPoint_x[(short)spawnIndex]=x;
				spawnPoint_y[(short)spawnIndex]=50-upperLineTop;
				spawnIndex++;
			}
			
			
			// lower spawn point, if land isn't 0:
			if(lowerLineTop<30)
			{
				spawnPoint_x[(short)spawnIndex]=x;
				spawnPoint_y[(short)spawnIndex]=170+lowerLineTop;
				spawnIndex++;
			}
		}
		
		// now we need to draw the lines on the map...
		for(y=0; y<200; y++)
		{
			// determine if the pixel is on or off:
			char pixelOn = (y>=(50-upperLineTop) && y<(50+upperLineBottom)) || (y>(170+lowerLineTop));

			// if it's on, we should OR it on to the current memory of the map
			if(pixelOn>0)
			{
				// we want to find the bit to draw... starting with the highest bit workign right
				// therefore, we want 31-(x%32)
				// then shift that bit to be 1..
				// then OR it on the map
				unsigned long mask = 1;
				mask = mask<<(31-(x%32));
				
				// what colum are we in?
				short row = (x-(x%32))/32;
				
				// the texture will just loop its y colums, every 200 pixels
				light[(row*200)+y] |= (mask);
				dark[(row*200)+y] |= (spr_LandTexture[y%32] & mask);
		
			}// pixel on
			
		}// next y
	}// next x
	
	// trace edges in the buffers
	Map_traceEdges();

	// part of generating the map will be generating the objects on it..
	Mines_spawnMines();
	OilDrums_spawnDrums();
	Worm_spawnWorms();
	
}


/**
 * tests a point on the map
 * 
 * @returns TRUE or FALSE if a point is on land
 */
char Map_testPoint(short x, short y)
{
	// our X/Y position will be passed in world space.
	// luckily, our map buffer is also world space.

	// if the point is out of bounds, it's automatially a non-hit:
	if((x<0) || (x>=320) || (y<0) || (y>=200))
		return FALSE;
		
	// map buffer pointer for memory testing
	unsigned long *light = (unsigned long*)mapLight;
	unsigned long *dark = (unsigned long*)mapDark;
		
	// since we now know it's in bounds, we need to translate
	// the X/Y to a long / bit address in the map buffer
	// bufferX = the row of 200 unsigned longs in the buffer
	// bufferY = the unsigned long in that row
	short bufferX = ((x-(x%32))/32)*200;
	short bufferY = y;

	// we need to check the exact pixel. lets get the short (32 bits) at that location
	unsigned long mapData = light[bufferY+bufferX] | dark[bufferY+bufferX];

	// imagine x is pixel 14. That's technically this pixel: 0000000000000010
	// since the lowest bit is index 0, there's 31 total indexable bits (32 bits total)
	// thus, the pixel we want is: 31-pixelX, where pixelX is x%32 (which will only ever be 0-31)
	short pixelBitIndex = (31-(x%32));
	
	// test the exact pixel.
	unsigned long mask = 1;
	mask = (mask<<(pixelBitIndex));
	char pixelOn = (char)((mapData & mask)>0);

	// comment out and it will crash
	// return FALSE;
	
	// return the status of this map pixel
	return pixelOn;
}



/**
 * find a free point to spawn something, that doesn't overlap with something else existing
 */
void Map_getSpawnPoint()
{
	// loop over our available spawn points till we find one:
	while(TRUE)
	{
			char index = random(53);
			if(spawnPoint_x[(short)index]!=-1)
			{
				// save the spawn point
				Map_lastRequestedSpawnX=spawnPoint_x[(short)index];
				Map_lastRequestedSpawnY=spawnPoint_y[(short)index];
				
				// deactivate it
				spawnPoint_x[(short)index]=-1;
				spawnPoint_y[(short)index]=-1;
				
				// gtfo
				return;
			}
	}	
}
