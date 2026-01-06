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
#include "Crates.h"
#include "Game.h"
#include "Camera.h"
#include "Match.h"
#include "Keys.h"

	
// when a spawn point is requested of the map, it will find one
// and update these global map variables:
short Map_lastRequestedSpawnX=0;
short Map_lastRequestedSpawnY=0;


// --------------------------------------------------------------------------------------------------------------------------------------


/**
 * @brief Determines if a point is considered out of bounds
 * 
 * @param x - x
 * @param y - y
 * @return char - TRUE if out of bounds, FALSE if in bounds 
 */
char Map_isOOB(short x, short y)
{
	// out of bounds check
	if(x<-1000 || x>=1420 || y<-500 || y>=200)
		return TRUE;
		
	return FALSE;
}


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
		
		// at the top of every Colum, this is 0 since it's "outside" the top of the buffer
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
			// note that, on the edges of our 32 bit map columns it will fail to draw horizontally changed pixels edge
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
 * @brief Makes a type-1 style land generation map
 * 
 * @param light - light map buffer
 * @param dark - dark map buffer
 * @param spr_LandTexture - the land texture sprite to use
 */
void Map_makeType1(unsigned long *light, unsigned long *dark, const unsigned long* spr_LandTexture)
{
	// iteration vars
	short x, y;

	/*
		Generating the map...
		
		The map will consist of drawing a series of vertical lines next to each other.
		Each vertical column of pixels will be slightly higher or lower than the ones next to it.
		
		On the top of the map, the lines will have both a top and bottom height.
		
		On the bottom of the map, the lines will go all the way to the bottom of the buffer, and only require a height.
		
		Sometimes, if the lines goto 0 height, this would be a hole in the land, leading the level below, or to water.
		
		We will loop from the left edge of the buffer, to the right edge of the buffer and set pixels at various heights
		
		Since the buffer will be updated 1 unsigned short at a time, we will generate the pixels for one column at a time
		then copy to the map.
	*/
	
	// keep track of empty land:
	unsigned char emptyLand=0;
	
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
			
			Also note that, the upperLineTop and upperLineBottom can never go below 0
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
		}
		else
		{
			// calculate if either land sections are "0" height:
			if(lowerLineTop>=30)
				emptyLand++;
			if(upperLineTop==0 && upperLineBottom==0)
				emptyLand++;
		}
		
		// now we need to draw the lines on the map...
		for(y=0; y<200; y++)
		{
			// determine if the pixel is on or off:
			char pixelOn = (y>=(50-upperLineTop) && y<(50+upperLineBottom)) || (y>(170+lowerLineTop));

			// if it's on, we should OR it on to the current memory of the map
			if(pixelOn>0)
			{
				// we want to find the bit to draw... starting with the highest bit working right
				// therefore, we want 31-(x%32)
				// then shift that bit to be 1..
				// then OR it on the map
				unsigned long mask = 1;
				mask = mask<<(31-(x%32));
				
				// what column are we in?
				short row = (x-(x%32))/32;
				
				// the texture will just loop its y columns, every 200 pixels
				light[(row*200)+y] |= (mask);
				dark[(row*200)+y] |= (spr_LandTexture[y%32] & mask);
		
			}// pixel on
			
		}// next y

	}// next x
}


/**
 * @brief Makes a type-2 style "Floating Islands" map
 * 
 * @param light - light map buffer
 * @param dark - dark map buffer
 * @param spr_LandTexture - the land texture sprite to use
 */
void Map_makeType2(unsigned long *light, unsigned long *dark, const unsigned long* spr_LandTexture)
{
	short x, y, i;
	
	// 3 Layers: Top(0), Mid(1), Bot(2)
	// State: 0=Inactive, 1=Active
	char active[3] = {0, 0, 0};
	
	// Current center Y position for each layer
	short centerY[3] = {40, 100, 160};
	
	// Current half-thickness for each layer
	short thick[3] = {0, 0, 0};
	
	// Loop columns
	for(x=0; x<320; x++)
	{
		// Update loading
		if(x%20==0) Draw_cake(x, 620);
		
		for(i=0; i<3; i++)
		{
			// Logic to start/stop islands
			if(active[i])
			{
				// 5% chance to end island if it's been running (thick > 5)
				// Or force end if out of bounds
				if((random(20)==0 && thick[i]>5) || centerY[i] < 10 || centerY[i] > 190) {
					active[i] = 0;
					thick[i] = 0; // Reset thickness
				} else {
					// Grow/Shrink thickness
					thick[i] += (random(3) - 1); // -1, 0, +1
					if(thick[i] < 3) thick[i] = 3;
					if(thick[i] > 20) thick[i] = 20;
					
					// Move Y center
					centerY[i] += (random(3) - 1);
				}
			}
			else
			{
				// 2% chance to start new island
				if(random(50)==0) {
					active[i] = 1;
					thick[i] = 2; // Start thin
					// Reset Y to default lane center +/- random variation
					centerY[i] = (i==0?40:(i==1?100:160)) + (random(20)-10);
				}
			}
			
			// Draw pixels if active
			if(active[i])
			{
				short startY = centerY[i] - thick[i];
				short endY = centerY[i] + thick[i];
				
				// Clamp
				if(startY < 0) startY = 0;
				if(endY > 199) endY = 199;
				
				// Draw Vertical Strip
				for(y=startY; y<endY; y++)
				{
					// Standard drawing logic
					unsigned long mask = 1UL << (31-(x%32));
					short row = (x-(x%32))/32;
					
					light[(row*200)+y] |= mask;
					dark[(row*200)+y] |= (spr_LandTexture[y%32] & mask);
				}
			}
		}
	}
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
	
	// pick the correct texture from settings
	const unsigned long* spr_LandTexture = tex_Ground[(short)Match_mapType];
	
	// make type one map (default)
	if(Match_mapType == 1)
		Map_makeType2(light, dark, spr_LandTexture);
	else
		Map_makeType1(light, dark, spr_LandTexture);
	
	// trace edges in the buffers
	Map_traceEdges();

	// part of generating the map will be generating the objects on it..
	// Prioritize WORMS first, so they get the best spots
	// don't spawn worms if we're in strategic mode
	if(Match_strategicPlacement==FALSE)
		Worm_spawnWorms();

	if(Match_minesEnabled)
		Mines_spawnMines();
	
	if(Match_oilDrumsEnabled)
		OilDrums_spawnDrums();
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

	// if the point is out of bounds, it's automatically a non-hit:
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
 * Helper to check distance between two points squared
 */
static long distSq(short x1, short y1, short x2, short y2) {
	long dx = (long)(x1 - x2);
	long dy = (long)(y1 - y2);
	return dx*dx + dy*dy;
}


/**
 * Finds a valid spawn point for the given object type.
 * Uses simplified vertical scanning and proximity checks.
 *
 * @param type - The type of object (SPAWN_WORM, SPAWN_MINE, etc)
 * @param outX - Pointer to store the result X
 * @param outY - Pointer to store the result Y
 * @return TRUE if found, FALSE if failed
 */
char Map_findSpawnPoint(char type, short *outX, short *outY)
{
	int i, j;
	short startX, startY;
	
	// Configuration based on type
	short radiusWorm = 0;
	short radiusMine = 0;
	short radiusDrum = 0;
	short headroom = 0;
	char checkWater = TRUE;
	char checkHeadroom = FALSE;
	char fromSky = FALSE;
	
	switch(type) {
		case SPAWN_WORM:
			radiusWorm = 15;
			radiusMine = 25;
			radiusDrum = 15;
			headroom = 20;
			checkHeadroom = TRUE;
			break;
		case SPAWN_MINE:
			radiusWorm = 25;
			radiusMine = 10;
			radiusDrum = 10;
			headroom = 5;
			checkHeadroom = TRUE;
			break;
		case SPAWN_DRUM:
			radiusWorm = 15;
			radiusMine = 10;
			radiusDrum = 15;
			headroom = 15;
			checkHeadroom = TRUE;
			break;
		case SPAWN_CRATE:
			radiusWorm = 15;
			radiusMine = 0;
			radiusDrum = 0;
			fromSky = TRUE;
			break;
	}
	
	short maxAttempts = 500;
	
	for(i=0; i<maxAttempts; i++) {
		
		// 1. Pick Random X
		startX = random(310) + 5;
		
		// 2. Determine Y scan start
		// Alternating scan: Top (0) vs Mid (90)
		if(fromSky) {
			startY = 0; 
		} else {
			// Randomly pick Upper (0) or Lower (90) layer to start scan
			startY = (random(2) == 0) ? 0 : 90;
		}
		
		// 3. Find Land using Raycast
		// Use Game_raycast to reduce code size
		// startY is always >= 0, so no out-of-bounds bug
		RaycastHit hit;
		Game_raycast(startX, startY, 0, 1, FALSE, &hit);
		
		short hitY = -1;
		
		if(fromSky) {
			// For crates, if we hit nothing (water) or land, spawn in sky
			*outX = startX;
			*outY = -10;
			
			// But check eventual landing spot for validity
			if(hit.hitType == RAY_HIT_LAND) hitY = hit.y;
			else hitY = 200; // Hole
			
		} else {
			// For ground objects, must hit land
			if(hit.hitType != RAY_HIT_LAND) continue;
			
			hitY = hit.y;
			*outX = hit.x;
			*outY = hit.y;
		}
		
		// 4. Validity Checks
		
		// Water Check
		if(checkWater) {
			if(hitY > (196 - Game_waterLevel))
				continue;
		}
		
		// Headroom Check
		if(checkHeadroom) {
			// Scan UP from ground
			RaycastHit headHit;
			Game_raycast(hit.x, hit.y - 4, 0, -1, FALSE, &headHit);
			if(headHit.hitType == RAY_HIT_LAND) {
				short gap = hit.y - headHit.y;
				if(gap < headroom) continue;
			}
		}
		
		// Proximity Checks (Combined Loop)
		char valid = TRUE;
		long radSqWorm = (long)radiusWorm * radiusWorm;
		long radSqMine = (long)radiusMine * radiusMine;
		long radSqDrum = (long)radiusDrum * radiusDrum;
		long radSqCrate = 400; // Fixed 20*20

		for(j=0; j<MAX_WORMS; j++) {
			// Worms (16)
			if(radiusWorm > 0 && (Worm_active & ((unsigned short)1<<j))) {
				if(distSq(*outX, hitY, Worm_x[j], Worm_y[j]) < radSqWorm) { valid = FALSE; break; }
			}
			// Mines (10)
			if(j < MAX_MINES && radiusMine > 0 && (Mine_active & ((unsigned short)1<<j))) {
				if(distSq(*outX, hitY, Mine_x[j], Mine_y[j]) < radSqMine) { valid = FALSE; break; }
			}
			// Drums (6)
			if(j < MAX_OILDRUMS && radiusDrum > 0 && (OilDrum_active & ((unsigned short)1<<j))) {
				if(distSq(*outX, hitY, OilDrum_x[j], OilDrum_y[j]) < radSqDrum) { valid = FALSE; break; }
			}
			// Crates (8)
			if(j < MAX_CRATES && Crate_active && (Crate_active & ((unsigned short)1<<j))) {
				if(distSq(*outX, hitY, Crate_x[j], Crate_y[j]) < radSqCrate) { valid = FALSE; break; }
			}
		}
		if(!valid) continue;
		
		return TRUE;
	}
	
	return FALSE;
}


// draw the map.. but I can't call this, crashes
void Map_draw()
{
	/*
		How this works:
		
		Our mapBuffer pointer points a buffer that's four times the normal LCD_SIZE.
		
		We only want to draw a sub-section of that buffer on the screen.
		
		For vertical:
		
		The calculators screen height is 100 pixels, so we should draw 50 pixels above and below
		the camera's "center"
		
		So: CameraTop = CameraY-50, and CameraBottom = CameraY+50
		
		The rows we draw on the screen will always be between 0 and 99 inclusive.
		
		So we need to determine the following:
			- The top most row on the screen to draw
			- The bottom most row on the screen to draw
			- The top row from the buffer we should start copying.
			
		If the camera goes out of bounds of the map buffer, we can still draw part of the map
		In this case, there are a few conditions:
			- If the Camera goes above the map (CameraTop is less than 0 in the map-buffer)
				then we can still draw the map, but ScreenTop will be lower on the screen
			- If the Camera goes below the map (CameraBottom is more than 200 in the map-buffer)
				then we can still draw the map. ScreenTop will be 0, but we'll only draw till the end of the buffer
			- If either the CameraBottom is less than 0 or CameraTop is greater than 200 then the map is complete
				off screen, and we don't need to draw it at all.
				
		This way, the camera can still pan outside the map, into nothingness.
		This is useful for following weapons or worms that fly far off the map, or projectiles that fly high
		into the sky.
	*/
	
	// camera top position, in world-space
	short camTop = camY-50;
	
	// camera bottom position, in world-space
	short camBottom = camY+50;
	
	// camera left position, in world-space
	short camLeft = camX-80;
	
	// camera right position, in world-space
	short camRight = camX+80;
	
	// we shouldn't continue if either is out of bounds
	if((camBottom>0) && (camTop<200) && (camLeft<320) && (camRight>0))
	{ 
		
		// the of the screen we should start copying the buffer to
		short screenTop=0;
		
		// the bottom of the screen we should stop copying the buffer to
		short screenBottom=99;
		
		// the top row from the buffer we should start copying from
		// note: we don't need a buffer bottom, since the loop is controlled from screenTop to screenBottom
		// so we only really need bufferTop in the loop
		short bufferTop=camTop;

		// if both the top and the bottom of the buffer points are inbounds
		// we can simply draw the whole screen worth!
		// we don't need to change any variables
		
		// if, however, the top of the camera is is above the map
		// we need to change our render bounds..
		if(camTop<0)
		{
			// whatever we draw, will be from the top row of the buffer
			bufferTop = 0;
			
			// we want to draw lower on the screen... by how far the camera is beyond
			screenTop = (camTop*-1);
			
			// [FIX] SAFETY CHECK: 
            // If screenTop is > 99, the map starts below the screen.
            // We must abort, otherwise (screenBottom - screenTop) will underflow 
            // and crash the calculator with a massive height value.
            if(screenTop > 99)
                return;
                
            // [FIX] Abort if the map is completely below the screen (Height <= 0)
	        // This handles camY <= -49 where screenTop >= 99
			if(screenTop >= screenBottom) 
				return;
            
			// we want to only copy pixels for the rest of the screen
			screenBottom = 99;

		// also check if the bottom of the camera is beyond the map buffer
		}else if(camBottom>199)
		{
			// always draw on the top of the screen:
			screenTop = 0;
			
			// always draw from the camera-top in the buffer:
			bufferTop = camTop;

			// only draw that many rows:
			screenBottom = (200-bufferTop);
		}
		
		// the left side of the screen, we should start drawing to
		short screenLeft = 0;
		
		// the right side of the screen, we should stop drawing at
		short screenRight = 160;
		
		// the position in the buffer we should start copying left-to-right
		short bufferLeft = camLeft;
		
		// if the screen's left is out of bounds of our map buffer
		// then we should start drawing in the middle of the screen...
		if(camLeft<0)
		{
			// the screen left should be the same as how far OB the camera is.
			// if the camera is 10 pixels to the left of the map, the map
			// should draw at 10px right on the screen...
			screenLeft = (camLeft*-1);
			
			// since we're starting at the left of the map, we will draw
			// all the way to the right edge of the screen:
			screenRight = 160;
			
			// since we're starting at the left of the screen, we will
			// start at the left of the buffer:
			bufferLeft=0;
			
		// if the right of the camera is beyond the right edge of the buffer...
		}else if(camRight>319)
		{
			// we will definitely start drawing on the left side of the screen
			screenLeft = 0;
			
			// we will start the buffer at the left-camera edge:
			bufferLeft = camLeft;
			
			// we only need to draw until the buffer runs out..
			screenRight = 160; //320-bufferLeft;
		}
		
		// because we are going to be copying bits in blocks of 32 at a time below
		// we need to convert our screenLeft, screenRight and bufferLeft into columns
		short colLeft = (screenLeft-(screenLeft%32))/32;
		short colRight = (screenRight-(screenRight%32))/32+1;
		short colBuff = (bufferLeft-(bufferLeft%32))/32;

		// loop to manually copy memory from a sub-section of our map, via ClipSprite32
		unsigned long *light = (unsigned long*)mapLight;
		unsigned long *dark = (unsigned long*)mapDark;
		short x;
		short bufferCol = 0;
		
		// loop over the visible columns of the map on screen, or till our buffer runs out
		for(x=colLeft; (x<colRight && (colBuff+bufferCol)<10); x++)
		{
			short offset = screenLeft==0 ? camLeft%32 : 0;

			// take advantage of extgrah's sprite method to handle bit siting and mem copying in one swoop!
			ClipSprite32_OR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &light[((colBuff+bufferCol)*200)+bufferTop], lightPlane);
			ClipSprite32_XOR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &dark[((colBuff+bufferCol)*200)+bufferTop], lightPlane);
			ClipSprite32_OR_R(screenLeft+(bufferCol*32)-offset, screenTop, screenBottom-screenTop, &dark[((colBuff+bufferCol)*200)+bufferTop], darkPlane);

			// on the next iteration we will be on the next buffer 32 bit column
			bufferCol++;
		
		}// next x
		
	}// end if draw map
}


/**
 * Adds a line of terrain to the map
 * 
 * @param x0 start x
 * @param y0 start y
 * @param x1 end x
 * @param y1 end y
 */
void Map_addTerrainLine(short x0, short y0, short x1, short y1)
{
    unsigned long *light = (unsigned long*)mapLight;
	unsigned long *dark = (unsigned long*)mapDark;

    short dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    short dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    short err = dx + dy, e2;

    while (1) {
        if(x0 >= 0 && x0 < 320 && y0 >= 0 && y0 < 200) {
            short row = x0 >> 5; 
            short bit = 31 - (x0 & 31);
            unsigned long mask = 1UL << bit;
            int idx = (row * 200) + y0;
            light[idx] |= mask;
            dark[idx] |= mask;
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
