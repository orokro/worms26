#ifndef MAP_H
#define MAP_H

/* ======================================================================================================================================
   MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP ++
   ====================================================================================================================================== */

// map globals
extern short Map_lastRequestedSpawnX;
extern short Map_lastRequestedSpawnY;
extern void *mapLight, *mapDark;
extern unsigned char mapTiles[640];

// map function prototypes

/**
 * Outlines the edges of land in the light and dark map buffers
*/
extern void Map_traceEdges();

/**
 * Checks if a point on the map is land or not.
 *
 * The map is a large buffer, and this method will take an X/Y position,
 * with X==0/Y==0 being the top left.
 * It will return a char boolean TRUE/FALSE if the pixel is LAND/NOT LAND.
 *
 * @param x the x position to test
 * @param y the y position to test
 * @return a char boolean if the point is land or not.
*/
extern char Map_testPoint(short, short);

/**
 * Sets globals, Map_lastRequestedSpawnX and Map_lastRequestedSpawnY to a valid spawn point for an item on the map.
 *
 * When the map is being generated a number of valid spawn-points are also generated.
 * This doesn't exactly "return" a point, but it selects from one of the generated ones and makes
 * the aforementioned globals equal to the valid spawn point.
 *
 * This method will also nullify the spawn point from the pool so no other items can spawn there.
*/
extern void Map_getSpawnPoint();

/**
 * Renders a map playfield for the worms to play on, as well as spawns in game items.
 *
 * This builds the map as well as spawns Worms, OilDrums (if active), and Mines (if active)
*/
extern void Map_makeMap();

#endif