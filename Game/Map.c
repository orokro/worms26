// C Source File
// Created 11/11/2017; 11:34:08 PM

#include "../Headers/System/Main.h"

// declare our external/global variables
short Map_lastRequestedSpawnX=0;
short Map_lastRequestedSpawnY=0;

//void *mapBuffer;

#define screenWidth 159
#define screenHeight 99


// function prototypes
char Map_testPoint(short, short);
void Map_getSpawnPoint();
void Map_makeMap(void*);


// builds a random map for the worms to play on
void Map_makeMap(void *mapBuffer)
{
	// iteration vars
	short x, y;
	
	/*
		NOTE:
		Since I can't get port set working on a double size map yet, for debugging I will draw a regular screen size
		
		Then I will copy this screen size four times to the map buffer, so each quadrant (top left, top right, bottom left, bottom right)
		will have an identical copy of data.
		
		For now, the map will have a border around it's edges in each quadrant, as well as some circles placed evenly.
		
		When I figure out how to draw with PortSet on the full map size, this code will be updated.
	*/
	
	// memory for a regular screen size worth od data
	void *segment = malloc(LCD_SIZE);
	
	// draw to this memory
	PortSet(segment, 239, 127);
	
	// loop to draw a bunch of circles on the map for Camera-scroll debugging
	for(x=10; x<screenWidth; x+=30)
		for(y=10; y<90; y+=30)
			DrawClipEllipse((short)x, (short)y, (short)5, (short)5, &(SCR_RECT){{(unsigned short)0, (unsigned short)0, (unsigned short)screenWidth, (unsigned short)screenHeight}}, A_NORMAL);
	
	// draw borders on the map
	DrawLine(0, 0, screenWidth, 0, A_NORMAL);
	DrawLine(0, screenHeight, screenWidth, screenHeight, A_NORMAL);
	DrawLine(0, 0, 0, screenHeight, A_NORMAL);
	DrawLine(screenWidth, 0, screenWidth, screenHeight, A_NORMAL);
	
	// now we need to copy the map to the four quadrants of the mapbuffer
	// so, each quadrant will be identical.
	// we will use modulus to have one set of loops copy it to all four places at once
	short *seg = segment;
	short *map = mapBuffer;
	for(x=0; x<30; x++)
		for(y=0; y<200; y++)
			map[y*30+x] = seg[((y%100)*15)+(x%15)];
	
	// free our temporary map segment buffer
	free(segment);
	
	// the code below was my initial attempt to user PortSet with a double-width and double-height map.
	// it doesnt work for some reason...
	/*
	
	PortSet(mapBuffer, 477, 254);
	//PortSet (mapBuffer, 239, 127);
	SetCurClip (&(SCR_RECT){{0, 0, 477, 254}});
	DrawClipEllipse((short)20, (short)20, (short)15, (short)15, &(SCR_RECT){{0, 0, 477, 254}}, A_NORMAL);
	SetCurClip (&(SCR_RECT){{0, 0, 159, 99}});
	return;
	
	
	for(x=10; x<467; x+=30)
	{
		for(y=10; y<251; y+=30)
		{
			DrawClipEllipse((short)x, (short)y, (short)15, (short)15, &(SCR_RECT){{(unsigned short)0, (unsigned short)0, (unsigned short)477, (unsigned short)261}}, A_NORMAL);
		}
	}
	*/
}

// tests a point on the map
char Map_testPoint(short x, short y)
{
	// prevent warnings for now
	x=x;
	y=y;
	
	// TO-DO: implement
	return FALSE;
}

// find a free point to spawn something, that doesn't overlap with something else existing
void Map_getSpawnPoint()
{
	// TO-DO: implement
	Map_lastRequestedSpawnX=0;
	Map_lastRequestedSpawnY=0;
	
}