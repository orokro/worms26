// C Source File
// Created 11/11/2017; 11:34:08 PM

#include "../Headers/System/Main.h"

// declare our external/global variables
short Map_lastRequestedSpawnX=0;
short Map_lastRequestedSpawnY=0;

//void *mapBuffer;

void Map_makeMap(void *mapBuffer)
{
	short x, y;
	
	void *segment = malloc(LCD_SIZE);
	PortSet(segment, 239, 127);
	//DrawClipEllipse((short)20, (short)20, (short)15, (short)15, &(SCR_RECT){{0, 0, 239, 127}}, A_NORMAL);
	for(x=10; x<229; x+=30)
	{
		for(y=10; y<117; y+=30)
		{
			DrawClipEllipse((short)x, (short)y, (short)5, (short)5, &(SCR_RECT){{(unsigned short)0, (unsigned short)0, (unsigned short)239, (unsigned short)127}}, A_NORMAL);
		}
	}
	
	char *seg = segment;
	char *map = mapBuffer;
	for(x=0; x<60; x++)
	{
		for(y=0; y<254; y++)
			map[y*60+x] = seg[((y%127)*30)+(x%30)];
	}
	
	return;
	
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