
#include <tigcclib.h>

// include all our headers in dependency order
#include "extgraph.h"
#include "MainGeneral.h"


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
extern short Match_defaultWeapons[5][14];



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
extern int camX;
extern int camY;

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

// draw function prototypes

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
extern void *mapBuffer;

// map function prototypes

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
   COLLISION +++ COLLISION +++ COLLISION +++ COLLISION +++ COLLISION +++ COLLISION +++ COLLISION +++ COLLISION +++ COLLISION +++ COLLISIO
   ====================================================================================================================================== */
   
// collision defines
#define COL_UP 0
#define COL_DOWN 1
#define COL_LEFT 2
#define COL_RIGHT 3

// collision function prototypes

/*
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

// explosion globals
extern short Explosion_x[8];
extern short Explosion_y[8];
extern char Explosion_time[8];
extern char Explosion_size[8];
extern char Explosion_power[8];
extern int Explosion_firstFrame;

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

// worm enums
enum WormModes {wormMode_idle, wormMode_walking, wormMode_jumping, wormMode_backFliping, wormMode_falling, wormMode_knockBack, wormMode_parachute, wormMode_rope, wormMode_bungie};

// worm globals
extern short Worm_x[16];
extern short Worm_y[16];
extern char Worm_xVelo[16];
extern char Worm_yVelo[16];
extern unsigned long Worm_dir;
extern char Worm_health[16];
extern unsigned long Worm_isDead;
extern unsigned long Worm_active;
extern char Worm_mode[16];
extern char Worm_currentWorm;
extern unsigned long Worm_unstable;
extern unsigned long Worm_onGround;

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

// OilDrum globals
extern short OilDrum_x[8];
extern short OilDrum_y[8];
extern char OilDrum_health[8];
extern int OilDrum_active;

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

// mine globals
extern short Mine_x[10];
extern short Mine_y[10];
extern char Mine_xVelo[10];
extern char Mine_yVelo[10];
extern char Mine_fuse[10];
extern int Mine_active;

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



/* ======================================================================================================================================
   CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CRATES +++ CR
   ====================================================================================================================================== */

// crate defines
#define crateHealth 0
#define crateWeapon 1
#define crateTool 2

// crate globals
extern short Crate_x[8];
extern short Crate_y[8];
extern char Crate_health[8];
extern char Crate_type[8];
extern int Crate_active;

// crate function prototypes

/**
 * This spawns a single Crate in the game, of the given type.
 * 
 * @param type the type of crate to spawn, as one of our defines: crateHealth, crateWeapon, crateTool
*/
extern void Crates_spawnCrate(char);

/**
 * Updates the Crates currently active, should be called once per frame.
*/
extern void Crates_update();



/* ======================================================================================================================================
   WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WE
   ====================================================================================================================================== */

// Define bitmask flags for the types of properties a weapon can have:
// when spawning a weapon, these can be ORed together to create it's logic
#define usesVelocity 		1 	// 00000001
#define usesGravity	 		2 	// 00000010
#define usesTimer	   		4 	// 00000100
#define usesHoming 	 		8		// 00001000
#define usesMovement 		16 	// 00010000
#define usesController 	32 	// 00100000

/*
  enumerate our list of weapons, with matching index positions as described in the array above
  note: the last row is for secondary weapons: they don't use the same game logic as the primaries
  
  also note: these enumerations are also used for the current selected weapon, which doesn't
  necessarily have an in-game object, or might be different.
  
  For isntance, a bow looks like a bow in the hand of the worm, but bow game objet is drawn as an arrow
  Where as a cluster bomb is drawn as a grenade in game, but spawns WClusters, which aren't a selectable
  weapon, only a weapon that exists as a side effect of a cluster or mortar exploding.
  
  Mean while, sheep launcher doesnt have an in-game equivalent, but rather, spawns a sheep with initial
  velocity
*/
enum Weapons {
	WJetPack, 		WLowG, 				WFastWalk, 		WLaser,				WInvis,
	WBazooka,			WHoming,			WMorter,			WHomingP,			WSheepLaunch,
	WGrenade,			WCluster,			WBanana,			WAxe, 				WQuake,
	WShotG, 			WHandG, 			WUzi, 				WMiniG, 			WBow, 
	WPunch, 			WDragonBall, 	WDeath, 			WSBomb, 			WProd,
	WDyna, 				WMine, 				WSheep, 			WSSheep, 			WMole,
	WAirStrike, 	WNapStrike, 	WMailStrike,	WMineStrike, 	WMoleStrike,
	WBlow, 				WDrill, 			WGirder, 			WBaseball, 		WGirderPak,
	WNinja, 			WBungee, 			WParachute, 	WTeleport, 		WScales,
	WSBanana, 		WHolyGrenade, WFlame, 			WSalArmy, 		WMB,
	WMolotov, 		WSkunk, 			WMingVase, 		WSheepStrike, WCarpet,
	WCows, 				WOldLady, 		WDonkey, 			WNuke, 				WGeddon,
	WSkip, 				WSurrender, 	WSwitch, 			WIce, 				WMagicB,
	WClusterFrag, WBananaFrag, 	WComet,				WSuperSheep };

// weapons globals
extern char Weapon_type[10];
extern short Weapon_x[10];
extern short Weapon_y[10];
extern char Weapon_xVelo[10];
extern char Weapon_yVelo[10];
extern char Weapon_time[10];
extern char Weapon_uses[10];
extern unsigned short Weapon_active;
extern short Weapon_targetX;
extern short Weapon_targetY;

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
extern void Weapons_spawn(char, short, short, char, char, char, char);

/**
 * Updates all the currently active weapons, should be called once per frame.
*/
extern void Weapons_update();

/**
 * Returns TRUE or FALSE if a any weapon at all is active.
 *
 * @return a char boolean if any weapon at all is active.
*/
extern char Weapons_weaponsActive();

/**
 * When a weapon is a targeted weapon, such as Air Strike or Homing Missle, this sets the current X/Y target.
 *
 * @param x the x position in world space for the weapon target.
 * @param y the y posiiion in world space for the weapon target.
*/
extern void Weapons_setTarget(short, short);



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
