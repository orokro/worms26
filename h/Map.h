/*
	Map.h
	-----
	
	Main header file for the Map.c module
*/

#ifndef MAP_H
#define MAP_H


// Map defines
#define MAP_WIDTH 320
#define MAP_HEIGHT 200


// map globals
extern short Map_lastRequestedSpawnX;
extern short Map_lastRequestedSpawnY;
extern void *mapLight, *mapDark;
extern unsigned char mapTiles[640];

// map function prototypes


/**
 * @brief Determines if a point is considered out of bounds
 * 
 * @param x - x
 * @param y - y
 * @return char - TRUE if out of bounds, FALSE if in bounds 
 */
extern char Map_isOOB(short x, short y);


/**
 * Outlines the edges of land in the light and dark map buffers
*/
extern void Map_traceEdges();


// Spawn Types
#define SPAWN_WORM 0
#define SPAWN_MINE 1
#define SPAWN_DRUM 2
#define SPAWN_CRATE 3

/**
 * Finds a valid spawn point for the given object type.
 * Uses raycasting and proximity checks to ensure safety.
 *
 * @param type - The type of object (SPAWN_WORM, SPAWN_MINE, etc)
 * @param outX - Pointer to store the result X
 * @param outY - Pointer to store the result Y
 * @return TRUE if found, FALSE if failed (should rarely fail)
 */
extern char Map_findSpawnPoint(char type, short *outX, short *outY);

/**
 * tests a point on the map
 * 
 * @returns TRUE or FALSE if a point is on land
 */
extern char Map_testPoint(short, short);


/**
 * Renders a map playfield for the worms to play on, as well as spawns in game items.
 *
 * This builds the map as well as spawns Worms, OilDrums (if active), and Mines (if active)
*/
extern void Map_makeMap();


/**
 * Draws the map!
 */
extern void Map_draw();


/**
 * @brief Adds a line of terrain to the map
 * 
 * @param x0 start x
 * @param y0 start y
 * @param x1 end x
 * @param y1 end y
 */
extern void Map_addTerrainLine(short x0, short y0, short x1, short y1);

#endif