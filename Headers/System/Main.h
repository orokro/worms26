
#include <tigcclib.h>

// include all our headers in dependency order
#include "../Headers/System/MainGeneral.h"
#include "../Headers/extgraph.h"


/* ======================================================================================================================================
   MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++ MAIN +++
   ====================================================================================================================================== */
  
  // main defines
  
	// how many frames consist of one second?
	#define TIME_MULTIPLIER 15
	       
	// main globals
	extern void *GblDBuffer;
	char GameRunning;
	
	// main rototypes
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
extern void Keys_update();
extern char Keys_keyDown();
extern char Keys_keyState();
extern char Keys_keyUp();



/* ======================================================================================================================================
   CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CA
   ====================================================================================================================================== */

// camera defines
#define camSpeed 6

// camera globals
extern int camX;
extern int camY;

// camera function prototypes
extern void Camera_update();
extern void Camera_focusOn();
extern void Camera_clearFocus();



/* ======================================================================================================================================
   DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++ DRAW +++
   ====================================================================================================================================== */

// draw function prototypes
extern void Draw_renderGame();
extern void Draw_renderPauseMenu(char);
extern void Draw_renderWeaponsMenu(char, char);
extern void setMapPtr(void *ptr);



/* ======================================================================================================================================
   MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP +++ MAP ++
   ====================================================================================================================================== */

// map globals
extern short Map_lastRequestedSpawnX;
extern short Map_lastRequestedSpawnY;
extern void *mapBuffer;

// map function prototypes
extern char Map_testPoint(short, short);
extern void Map_getSpawnPoint();
extern void Map_makeMap(void*);



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

// explosion unction prototypes
extern void Explosion_spawn(short, short, char, char, char);
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
extern long Worm_isDead;
extern long Worm_active;
extern char Worm_mode[16];
extern char Worm_currentWorm;

// worm function prototypes
extern void Worm_spawnWorms();



/* ======================================================================================================================================
   OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUMS +++ OIL DRUM
   ====================================================================================================================================== */

// OilDrum globals
extern short OilDrum_x[8];
extern short OilDrum_y[8];
extern char OilDrum_health[8];
extern int OilDrum_active;

// OilDrum function prototypes
extern void OilDrums_spawnDrums();
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
extern void Mines_spawnMines();
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
extern void Crates_spawnCrate(char);
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
extern void Weapons_spawn(char, short, short, char, char, char, char);
extern void Weapons_update();
extern char Weapons_weaponsActive();
extern void Weapons_setTarget(short, short);



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
extern void Game_changeMode(char);
extern void Game_update();