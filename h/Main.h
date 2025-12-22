
#include <tigcclib.h>

// include all our headers in dependency order
#include "Lib\extgraph.h"
#include "MainGeneral.h"


#define MAP_WIDTH 320
#define MAP_HEIGHT 200

/*
	Below the externs for all our global variables and global functions are defined.

	They are segmented into sections with large comment headers.

	For reference, this is the order of dependency:
		- Keys
		- Camera
		- Draw
		- Map
		- Exposions
		- Worms
		- Oil Drums
		- Mines
		- Crates
		- Weapons
		- Game

	The varibles and functions are defined in the above order,
	so latter items cannot reference things defined after them.
*/



/* ======================================================================================================================================
   MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++
   ====================================================================================================================================== */
  
// main defines
  
// how many frames consist of one second?
#define TIME_MULTIPLIER 15
	       
// main globals
extern void *GblDBuffer;
extern char GameRunning;
extern void *mapBuffer;
extern void *lightPlane;
extern void *darkPlane;
	
// main prototypes

/**
 * Calculates the distance between two 2D points.
 	 * 
 	 * @param x1 the x value of the first coordinate
 	 * @param y1 the y value of the first coordinate
 	 * @param x2 the x value of the second coordinate
 	 * @param y2 the y value of the second coordinate
 	 * @return the distance between 2 points, via Pythags thereom
*/
short dist(short, short, short, short);
	
	
   
/* ======================================================================================================================================
   MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATCH +++ MATC
   ====================================================================================================================================== */

// match globals
extern char Match_wormCount[2];
extern char Match_gravestones[2];
extern char Match_wormStartHealth;
extern char Match_allowWormSelection;
extern char Match_artilleryMode;
extern char Match_turnTime;
extern char Match_minesEnabled;
extern char Match_mineFuseLength;
extern char Match_dudMines;
extern char Match_oilDrumsEnabled;
extern char Match_toolCratesEnabled;
extern char Match_healthCratesEnabled;
extern char Match_weaponCratesEnabled;
extern short Match_teamWeapons[2][65];
extern const char *Match_teamNames[2];
extern const char *Match_wormNames[16];


/* ======================================================================================================================================
   KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++ KEYS +++
   ====================================================================================================================================== */

// key defines

// our bitmasks for the keys we care about. we will use names that either reflect the physical button
// or the buttons game-logic name
// note the powers of 2! moving the bit one place increased by powers of 2. Yay computer science
#define keyAction (long)1					//00000000000000000000000000000001
#define keyJump (long)2						//00000000000000000000000000000010
#define keyBackflip (long)4				//00000000000000000000000000000100
#define keyCameraControl (long)8	//00000000000000000000000000001000
#define keyEscape (long)16				//00000000000000000000000000010000
#define keyWeaponsSelect (long)32	//00000000000000000000000000100000
#define keyWormSelect (long)64		//00000000000000000000000001000000
#define keyLeft (long)128					//00000000000000000000000010000000
#define keyRight (long)256				//00000000000000000000000100000000
#define keyUp (long)512						//00000000000000000000001000000000
#define keyDown (long)1024				//00000000000000000000010000000000
#define keyGirderRight (long)2048	//00000000000000000000100000000000
#define keyGirderLeft (long)4096	//00000000000000000001000000000000
#define key1 (long)8192						//00000000000000000010000000000000
#define key2 (long)16384					//00000000000000000100000000000000
#define key3 (long)32768					//00000000000000001000000000000000
#define key4 (long)65536					//00000000000000010000000000000000
#define key5 (long)131072					//00000000000000100000000000000000
#define keyAction2 (long)262144		//00000000000001000000000000000000

// note this key: because it's all 1's, it will always return true when & on any other int,
// so long as at least ONE bit is set. This way, we can test for any-key
#define keyAny (long)262143				//00000000000001111111111111111111

// this works similar to keyAny, except just for the direction keys
#define keyCursors (long)1920			//00000000000000000000011110000000

// keys function prototypes

/**
 * Reads the current state of the key unput, and updates our bitwise buffers.
 * 
 * Three longs are defined: keysDown, keysState, keysUp.
 * These bits in these longs mirror the key states for each key matching our #defined bitmasks.
 * keysDown only has it's bits true on the single-frame a key is down on.
 * keysUp only has it's bits true for the single-frame a key is up on.
 * keysState has it's bits true on every frame the key is pressed.
*/
extern void Keys_update();

/**
 * Checks if a key is FIRST down ON THIS FRAME using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is down
*/
extern char Keys_keyDown(long);

/**
 * Checks if a key is pressed on this frame using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is pressed
*/
extern char Keys_keyState(long);

/**
 * Checks if a key is LET up ON THIS FRAME using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is let up
*/
extern char Keys_keyUp(long);



/* ======================================================================================================================================
   CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CA
   ====================================================================================================================================== */

// camera defines
#define camSpeed 6

// camera globals
extern short camX;
extern short camY;

// camera function prototypes

/**
 * Updates the current state of the Camera, should be called once per frame.
*/
extern void Camera_update();

/**
 * Sets the Camera to focus on a pair of short points.
 *
 * The Camera will follow the pointers set each frame.
 * If the variavles that were passed in as pointers are updated,
 * the Camera will have new coordinates to follow each frame.
 *
 * @param targetX a pointer to a short x variable for the Camera to track
 * @param targetY a pointer to a short y variable for the Camera to track
*/
extern void Camera_focusOn(short*, short*);

/**
 * Removes the current focal reference pointers from the camera.
 *
 * After calling, the Camera will be static until moved by the user,
 * or a new target is set with Camera_focusOn(*x, *y)
*/
extern void Camera_clearFocus();



/* ======================================================================================================================================
   DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++
   ====================================================================================================================================== */

// Draw defines

// Draw globals
extern unsigned long windSprites[3][3];
extern unsigned long healthSprites[16][18];
extern unsigned long healthMasks[16][18];

// draw function prototypes

/**
 * Draws the cake loading screen
 *
 * @param amount the current progress out of the current total
 * @param total amount of progress to be completed
*/
extern void Draw_cake(short amount, short total);

/**
 * Renders the map, items on it, and all game elements in general.
 *
 * Renders what is essential for the Game play, including:
 * 	- Map
 * 	- Worms
 * 	- Crates
 * 	- Mines
 * 	- Oil Drums
 * 	- Active Weapons
 * 	- Active particles
 *	- Game timer
 * 	- Wind
 * 	- etc
*/
extern void Draw_renderGame();

/**
 * Draws the pause menu, with current menu item selected.
 * 
 * The pause menu has three options: continue, quit, show controls.
 * This draws the pause menu instead of the current Game.
 * The pause menu will highlight whichever option is selected.
 *
 * @param menuItem the index of the menu item that is currently selected (continue, quit, show controls)
*/
extern void Draw_renderPauseMenu(char);

/**
 * Draws the weapons menu for when a player is selecting a weapon.
 * 
 * @param wx a char that contatins the x-position of the current weapon selected in the matrix of weapons
 * @param wy a char that contatins the y-position of the current weapon selected in the matrix of weapons
*/
extern void Draw_renderWeaponsMenu(char, char);

/**
 * renders our tiny text font to a serrirs of unsigned longs
 *
 * @param *buffer a pointer to at least 3 unsigned longs to render the text to
 * @param size the number of unsigned longs this buffer is wide. There should be 3*size unsigned longs in the buffer.
 * @param txt the test to write
 * @param color TRUE for white on black, FALSE for black on white
 * @return the length of the text in pixels
*/
extern short Draw_renderText(unsigned long *buffer, char size, char *txt, char color);

/**
 * renders all the worms name sprites once at the beginning of the game
 *
 * @param the index of the worm to draw the health sprite for
*/
extern void Draw_healthSprite(short index);

/**
 * Expirimental, now depricated method, to set a local pointer to the maps memory.
 *
 * @param ptr a pointer to the memory of the map buffer
*/
extern void setMapPtr(void *ptr);



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




   

   
   
/* ======================================================================================================================================
   EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++ EXPLOSIONS +++
   ====================================================================================================================================== */

// explosion defined
#define MAX_EXPLOSIONS 8

// explosion globals
extern short Explosion_x[MAX_EXPLOSIONS];
extern short Explosion_y[MAX_EXPLOSIONS];
extern char Explosion_time[MAX_EXPLOSIONS];
extern char Explosion_size[MAX_EXPLOSIONS];
extern char Explosion_power[MAX_EXPLOSIONS];
extern unsigned short Explosion_active;
extern unsigned short Explosion_firstFrame;


// explosion function prototypes

/**
 * Spawns an explosion at the area, with the max radius size, power and if it spawns fire particles or not.
 *
 * @param x the x position in world space to spawn the explosion
 * @param y the y position in world space to spawn the explosion
 * @param size the max radious the explosion should achieve
 * @param power the max health damagine, and velocity causing power of the explosion
 * @param hasFire a char boolean if the explosion should generate fire particles or not
*/
extern void Explosion_spawn(short, short, char, char, char);

/**
 * Updates all the explisions currently active, should be called once per frame.
*/
extern void Explosion_update();


#include "PhysCol.h"





/* ======================================================================================================================================
   OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUM
   ====================================================================================================================================== */

// OilDrum defines
#define MAX_OILDRUMS 6

// OilDrum globals
extern short OilDrum_x[MAX_OILDRUMS];
extern short OilDrum_y[MAX_OILDRUMS];
extern char OilDrum_health[MAX_OILDRUMS];
extern unsigned short OilDrum_active;
extern unsigned short OilDrum_settled;

// OilDrum function prototypes

/**
 * This spawns all the Oil Drums on the map.
 *
 * This method is always called in the Map_build method, and will check if OilDrums are enabled on it's own.
 * This sets up, and makes active all the OilDrums.
*/
extern void OilDrums_spawnDrums();

/**
 * Updates the Oil Drums currently active, should be called once per frame.
*/
extern void OilDrums_update();



/* ======================================================================================================================================
   MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINES +++ MINE
   ====================================================================================================================================== */

// mine defines
#define mineTriggerDistance 10
#define MAX_MINES 10

// mine globals
extern short Mine_x[MAX_MINES];
extern short Mine_y[MAX_MINES];
extern char Mine_xVelo[MAX_MINES];
extern char Mine_yVelo[MAX_MINES];
extern char Mine_fuse[MAX_MINES];
extern unsigned short Mine_active;
extern unsigned short Mine_triggered;
extern unsigned short Mine_settled;
extern char OilDrum_xVelo[MAX_OILDRUMS];
extern char OilDrum_yVelo[MAX_OILDRUMS];

// mine function prototypes

/**
 * This spawns all the Mines on the map.
 *
 * This method is always called in the Map_build method, and will check if Mines are enabled on it's own.
 * This sets up, and makes active all the Mines.
*/
extern void Mines_spawnMines();

/**
 * Updates the Mines currently active, should be called once per frame.
*/
extern void Mines_update();

/**
 * Allows a worm to trigger a mine
 *
 * @param index the mine index to trigger
*/
extern void Mines_trigger(short);

/**
 * Changes the heath of a worm. Updates worms health sprite.
 *
 * @param index the worm to change
 * @param health the amount to set it to, or add/subtract from
 * @param additive TRUE or FALSE if using additive mode
*/
extern void Worm_setHealth(short, short, char);












