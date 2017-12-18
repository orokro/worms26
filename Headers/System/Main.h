
#include <tigcclib.h>

// include all our headers in dependency order
#include "extgraph.h"
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

/**
 * Updates our tile map with the positions of objects
*/
extern void Map_updateTiles();


/* ======================================================================================================================================
   PHYSICS/COLLISION +++ PHYSICS/COLLISION +++ PHYSICS/COLLISION +++ PHYSICS/COLLISION +++ PHYSICS/COLLISION +++ PHYSICS/COLLISION +++ PH
   ====================================================================================================================================== */

// collision defines
#define COL_UP 0b00000001
#define COL_DOWN 0b00000010
#define COL_LEFT 0b00000100
#define COL_RIGHT 0b00001000
#define COL_UD 0b00000011
#define COL_LR 0b00001100
#define COL_DLR 0b00001110
#define COL_UDLR 0b00001111

/*
	This one is special
	
	In some cases, both the LEFT and RIGHT collider can hit in a frame.
	
	If this is the case, we will move the object UP until neither left or right are hitting.
	But this means, most likely, the DOWN collider will test nothing, allowing the object
	to fall, which will once again trigger it's LEFT and RIGHT colliders.
	
	When this happens, the object will NEVER come to be settled.
	
	So, whenever we move the object by colliding both LEFT and RIGHT, we will return this special
	code, which the physics system can use to remove its velocity and set it rested. Hackish, but works.
*/
#define COL_BOTHLR 0b10000000

// I can't help but abstract this... we'll see how it goes..
// this is a Collider struct for remembering the specific collider properties of an object
typedef struct{
	
	// the type of collider:
	unsigned char type;
	
	// the list of point distances to check. size varies by type
	char u, d, l, r;
	
	// a bit masked byte with whatever collisions happened on the last frame it was tested
	unsigned char collisions;
	
}Collider;


// I can't help but abstract this... we'll see how it goes..
// this is a physics object, so we can reuse physics code for many types of objects
typedef struct{
	
	// references to the x and y, position and velocity this physics object managers:
	short *x, *y;
	char *xVelo, *yVelo;
	
	// the last X and Y position this object was on the previous frame
	short lastX, lastY;
	
	// the objects bouciness on X and Y:
	// 0.0 = doesn't bounce.
	// 1.0 = bounces back with full velocity
	// -1.0 = special case: object not allowed to move on that axis
	float bouncinessX, bouncinessY;
	
	// the objects smoothness
	// 0 = the object picks a random direction to bounce every time it hits the ground
	// 100 = the object always bounces it it's headed direction
	char smoothness;
	
	// the index this of this object in it's corresponding arrays
	char index;
	
	// the number of frames since this object hasnt moved
	// when this reaches 3, it will be considered "setteled"
	char staticFrames;
	
	// references to the settled bit wise long for this object
	unsigned short *settled;
	
	// the collider for this object
	Collider col;
	
}PhysObj;

// collision function prototypes

/**
 * to be used like a pseudo constructor for a new Collider
 *
 * a collider can test various points around an objects center, and reposition the object appropriately if it colides with the land.
 *
 * @param type the type of collider, which can be COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT, COL_UP, COL_LR, or COL_UDLR
 * @param up the up amount to check, if its COL_UP, COL_UD, or COL_UDLR
 * @param down the down amount to check, if its COL_DOWN, COL_UD, or COL_UDLR
 * @param left the left amount to check, if its COL_LEFT, COL_LR, or COL_UDLR
 * @param right the right amount to check, if its COL_RIGHT, COL_LR, or COL_UDLR
 * @return a new Collider struct preinitialized with these params.
*/
extern Collider new_Collider(unsigned char type, char up, char down, char left, char right);

/**
 * to be used like a pseudo constructor for a new PhysObj
 * 
 * @param *x pointer to the objects x value
 * @param *y pointer to the objects y value
 * @param *xVelo pointer to the objects x velocity
 * @param *yVelo pointer to the objects y velocity
 * @param bounciness the percent amount (0.0 - 1.0) the objets velocity should be after colliding. Note: this sets both bouncinessX and bouncinessY. They can manually be set later if they need to be different.
 * @param smoothness the percent amount (0.0 - 1.0) the object should randomly change x directly when bouncing with the ground. 1.0 = always the same x direction. 0.0 = random direction.
 * @param settled the value that should be bit-masked if the object is considered settelted (e.g. hasn't moved in multiple frames)
 * @param c the objects Collider struct to test collisions with
*/
extern PhysObj new_PhysObj(short *x, short *y, char *xVelo, char *yVelo, float bounciness, float smoothness, char objIndex, unsigned short *settled, Collider c);

/**
 * updates a physics object, including map collision
 *
 * @param obj the PhysObj struct to apply
 * @return TRUE or FALSE weather or not the object moved on this frame
*/
extern char Physics_apply(PhysObj*);

/**
 * Sets a physics objects velocity
 *
 * @param *obj the physics object to set velocity
 * @param x the new x velocity
 * @param y the new y velocity
 * @param additive if set to TRUE the X/Y are added to the current velocity instead of replacing it.
*/
extern void Physics_setVelocity(PhysObj *obj, char x, char y, char additive);

/**
 * Tests an object against nearby explosions, if any.
 *
 * Will automatically apply velocities if hit by any number of explosions.
 *
 * @param *obj the physics object to test with
 * @return the total damage taken by the object, if hit by one or more explosions
*/
extern short Physics_checkExplosions(PhysObj *obj);

/**
 * This method takes a point in world space, and a direction and tests for collision.
 *
 * If the point collides with the map, it will calculate the first availble point in the opposite direction.
 * This method returns 0 if there is no collision, or the amount of pixels in the opposite direction for the
 * first non map pixel. This return value can be added to the items X/Y so it's not colliding.
 * The valid directions are COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT
 *
 * @param x the x position in world space to test
 * @param y the y position in world space to test
 * @param dir the direction of the test. Can be: COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT
*/
extern short Collide_test(short, short, char);
   

   
   
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



/* ======================================================================================================================================
   WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORMS +++ WORM
   ====================================================================================================================================== */

// worm defines
#define MAX_WORMS 16

// worm enums
enum WormModes {wormMode_idle, wormMode_walking, wormMode_jumping, wormMode_backFliping, wormMode_falling, wormMode_knockBack, wormMode_parachute, wormMode_rope, wormMode_bungie};

// worm globals
extern short Worm_x[MAX_WORMS];
extern short Worm_y[MAX_WORMS];
extern char Worm_xVelo[MAX_WORMS];
extern char Worm_yVelo[MAX_WORMS];
extern PhysObj Worm_physObj[MAX_WORMS];
extern unsigned short Worm_dir;
extern short Worm_health[MAX_WORMS];
extern unsigned short Worm_isDead;
extern unsigned short Worm_active;
extern char Worm_mode[MAX_WORMS];
extern char Worm_currentWorm;
extern unsigned short Worm_settled;
extern unsigned short Worm_onGround;

// worm function prototypes

/**
 * At the beginning of the game, this places all the worms on the map and sets them active.
 *
 * This also gives them a random initial direction to face: left or right.
*/
extern void Worm_spawnWorms();

/**
 * This handles updates, (i.e. phsysics, gravity, and map collisions) for worms, as well as damage checking from explosions.
*/
extern void Worm_update();


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


/* ======================================================================================================================================
   CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CR
   ====================================================================================================================================== */

// crate defines
#define MAX_CRATES 8
#define crateHealth 0
#define crateWeapon 1
#define crateTool 2

// crate globals
extern short Crate_x[MAX_CRATES];
extern short Crate_y[MAX_CRATES];
extern char Crate_health[MAX_CRATES];
extern char Crate_type[MAX_CRATES];
extern unsigned short Crate_active;
extern unsigned short Crate_settled;
extern char parachuteCrate;

// crate function prototypes

/**
 * This spawns a single Crate in the game, of the given type.
 * 
 * @param type the type of crate to spawn, as one of our defines: crateHealth, crateWeapon, crateTool
 * @return TRUE or FALSE if a crate was successfully spawned
*/
extern char Crates_spawnCrate();

/**
 * Updates the Crates currently active, should be called once per frame.
*/
extern void Crates_update();

/**
 * Allows a worm to pick up the crate
 *
 * @param index the crate index to pickup
 * @param worm the index of the worm who picked up the crate
*/
extern void Crates_pickUp(short, short);


/* ======================================================================================================================================
   WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WE
   ====================================================================================================================================== */

// Weapons defines
#define MAX_WEAPONS 10

// Define bitmask flags for the types of properties a weapon can have:
// when spawning a weapon, these can be ORed together to create it's logic
#define usesAim 							0b0000000000000001
#define usesCharge 						0b0000000000000010
#define usesCursor 						0b0000000000000100
#define usesPhysics 					0b0000000000001000
#define usesWind 							0b0000000000010000
#define usesHoming 						0b0000000000100000
#define usesFuse 							0b0000000001000000
#define usesDetonation 				0b0000000010000000
#define usesController 				0b0000000100000000
#define usesDetonateOnImpact 	0b0000001000000000
#define isAnimal 							0b0000010000000000
#define isCluster 						0b0000100000000000
#define isParticle 						0b0001000000000000				
#define isMele 								0b0010000000000000
#define spawnsSelf 						0b0100000000000000
#define multiUse 							0b1000000000000000

/*
  enumerate our list of weapons, with matching index positions as described in the array above
  note: the last row is for secondary weapons: they don't use the same game logic as the primaries
  
  also note: these enumerations are also used for the current selected weapon, which doesn't
  necessarily have an in-game object, or might be different.
  
  For isntance, a bow looks like a bow in the hand of the worm, but bow game objet is drawn as an arrow
  Where as a cluster bomb is drawn as a grenade in game, but spawns WFragments, which aren't a selectable
  weapon, only a weapon that exists as a side effect of a cluster or mortar, or etc. exploding.
  
*/
enum Weapons {
	WJetPack,				WBazooka, 				WGrenade, 	WShotGun,		WFirePunch, 	WDynamite, 		WAirStrike, 		WBlowTorch, 	WNinjaRope, 			WSuperBanana, 		WPetrolBomb, 	WMadCows, 				WSkipGo,
	WLowGravity, 		WHomingMissle, 		WCluster,		WHandGUn,		WDragonBall,	WMine, 				WNapalmStrike, 	WDrill, 			WBungeeCord,			WHolyHandGrenade,	WSkunk, 			WOldLady, 				WSurrender,
	WFastWalk,			WMortar, 					WBanana, 		WUzi,				WKakamaze, 		WSheep, 			WMailStrike, 		WGirder, 			WParachute,				WFlameThrower, 		WMingVase, 		WConcreteDonkey, 	WSelectWorm,
	WLaserSight, 		WHomingPigeon, 		WAxe,				WMiniGun,		WSuicideBomb, WSuperSheep, 	WMineStrike, 		WBaseballBat, WTeleport, 				WSalvationArmy, 	WSheepStrike, WNuclearTest, 		WFreeze,
	WInvisibility,	WSheepLauncher,		WQuake, 		WLongbow,  	WProd, 				WMole, 				WMoleSquadron, 	WGirderPack, 	WScalesOfJustice,	WMBBomb, 					WCarpetBomb, 	WArmageddon, 			WMagicBullet,
	WFragment, 			WFire,						WSkunkGas,	WComet
};

// weapons globals
extern char Weapon_type[MAX_WEAPONS];
extern short Weapon_x[MAX_WEAPONS];
extern short Weapon_y[MAX_WEAPONS];
extern PhysObj Weapon_physObj[MAX_WEAPONS];
extern char Weapon_xVelo[MAX_WEAPONS];
extern char Weapon_yVelo[MAX_WEAPONS];
extern unsigned short Weapon_time[MAX_WEAPONS];
extern unsigned short Weapon_active;
extern unsigned short Weapon_settled;
extern short Weapon_targetX;
extern short Weapon_targetY;
extern char Weapon_aimPosList[10][2];

/*
	there are currently 69 different types of weapon objects that can be on screen
	and behave differently.
	
	The game has 65 weapons / tools the user can choose from in the menu
	
	And there are four extra weapons that can spawn as a result of another weapon:
		- fragments (from cluster bombs, mortars, etc)
		- Fire / Napalm
		- Skunk Gas / Poison Gas
		- Coment (from Armegeddon)
	
	Below is an array of 16 bit shorts we will use to bitwise store the properties of
	each weapon type, so we can optimize the weapon routines to reuse code
*/
extern unsigned short Weapon_props[69];

// weapons function prototypes

/**
 * This spawns a Weapon item in the game.
 *
 * Weapons have a type, position, initial x/y velocity, time and various properties.
 * The properties are bitmasked onto a char.
 * Valid Properties:
   - usesVelocity
 * - usesGravity
 * - usesTimer
 * - usesHoming
 * - usesMovement
 * - usesController
 *
 * @param type a char storing the type of weapon item this is, as defined by the enumeration Weapons.
 * @param x the starting x position of the weapon
 * @param y the starting y position of the weapon
 * @param xVelocity the starting x velocity of the weapon
 * @param yVelocity the staarting y velocity of the weapon
 * @param time weapons use time for different purposes, (e.g. fuse length)
 * @param properties a char that is used as a bitmasked variable for the types of properties this weapon requires
*/
extern void Weapons_spawn(char, short, short, char, char, unsigned short);

/**
 * Updates all the currently active weapons, should be called once per frame.
*/
extern void Weapons_update();

/**
 * When a weapon is a targeted weapon, such as Air Strike or Homing Missle, this sets the current X/Y target.
 *
 * @param x the x position in world space for the weapon target.
 * @param y the y posiiion in world space for the weapon target.
*/
extern void Weapons_setTarget(short, short);

/**
 * When the user fires a weapon
*/
extern void Weapons_fire(short charge);


/* ======================================================================================================================================
   CHARACTER CONTROLLER +++ CHARACTER CONTROLLER +++ CHARACTER CONTROLLER +++ CHARACTER CONTROLLER +++ CHARACTER CONTROLLER +++ CHARACTER
   ====================================================================================================================================== */

// character controller function prototypes

/**
 * Updates the character based on user input for a frame turning the turn game mode.
*/
extern void CharacterController_update();




/* ======================================================================================================================================
   GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++ GAME +++
   ====================================================================================================================================== */

// game enums
enum GameModes {gameMode_WormSelect, gameMode_Turn, gameMode_WeaponSelect, gameMode_Pause, gameMode_Cursor, gameMode_TurnEnd, gameMode_Death, gameMode_AfterTurn, gameMode_GameOver};

// game globals
extern char Game_mode;
extern char Game_previousMode;
extern short Game_timer;
extern short Game_retreatTimer;
extern short Game_graceTimer;
extern long Game_suddenDeathTimer;
extern char Game_wind;
extern char Game_waterLevel;
extern char Game_currentTeam;
extern char Game_currentWormUp[2];
extern short Game_cursorX;
extern short Game_cursorY;
extern char Game_cursirDir;
extern short Game_xMarkSpotX;
extern short Game_xMarkSpotY;
extern char Game_xMarkPlaced;
extern char Game_xMarkAllowedOverLand;
extern char Game_cursorEndTurn;
extern char Game_turn;
extern short Game_weapInventory[5][13];
extern char Game_currentWeaponSelected;
extern unsigned short Game_currentWeaponProperties;
extern unsigned short Game_currentWeaponCharge;
extern char Game_aimAngle;

// game function prototypes

/**
 * Changes the Games primary state machine to a new mode.
 *
 * @param newMode a char representing a game mode as defined in the GameModes enum.
*/
extern void Game_changeMode(char);

/**
 * The main update method for the entire game, should be called once per frame.
 *
 * The Game_update() method handles general every-frame logic,
 * as well as calling the current game states update method as well.
 * This is called every frame in the main-loop.
*/
extern void Game_update();

/**
	* Returns TRUE or FALSE if all he game items in the game are settled and not doing anything.
	* 
	* Crates, Mines, Drums, and Worms must be settled on the ground.
	* No active mine timers allowed.
	* No active explosions.
	* No active weapons
	*
	* @return TRUE or FALSE if everything is settled or not
*/
extern char Game_allSettled();
