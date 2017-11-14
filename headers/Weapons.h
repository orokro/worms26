// Header File
// Created 11/13/2017; 10:32:04 PM

/*
	Weapons
	-------
	
	This defines globals for Weapons on the map.
	
	About Weapons:
	
	The weapons system defines a number of arrays for on-screen weapon objects.
	Each weapon object has or optionally can have:
	- an X/Y position
	- an X/Y velocity
	- a timer
	- a target
	- gravity
	- .. and more
	
	Each weapon has it's on unique behavoir, however, some behavoirs can be
	shared between many weapons. For example, hande greands and cluster bombs
	both are fired and have physics. When the cluser explodes it will create
	more bomb particles, where as the grenade just explodes.
	
	Physics for grenades, therefore, is shared between the two.
	
	Logic will be in Weapons.C, and this file outlines the public arrays, variables
	and methods.
	
	A note about how Weapons are indexed:
	
	For the weapons menu, and inventory, the weapons are stored in a 5x14 array, like so:
	[5][14] = [5][14] = { 
												{0,9,9,9,9,1,1,3,5,0,2,0,9,1},
												{0,1,3,9,9,2,1,3,2,0,1,0,9,1},
												{0,5,0,9,1,1,0,3,2,0,0,0,3,1},
												{0,0,2,0,0,0,0,1,2,0,0,0,0,1},
												{0,0,0,2,9,0,0,0,0,0,0,0,0,1}
											};
											
	This pattern reflects the same position they are in the WWP menu on the PC.
	However, it can also be indexed directly, such that [0][0]=0 and [0][1]=1,
	where as [1][0] would actually be weapon 15.
	
	Regarding the total number of weapons on screen at once:
	Fire particles, either for napalm or fire, will be handled with a fire system separately,
	since fire doesn't need to take up weapon slots, and lots of weapons may need to spawn fire
	
	Thus, if a cluster bomb clusters into 5 small cluser fragments, there needs to be a total of 6 weapon
	slots availalbe:
	1 for the initial bomb, room to spawn 5 clusters on the same frame the bomb is deactivated
	
	For the uzi and gattling gun, there may need to be more than 6 bullets on screen tho, which would
	by far be the most in-game weapons at once.
	
	Let's say for now, 10 weapons, but we will only loop to 6 unless the "bullets" flag is set																		
*/

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

// the type of the weapon!
char Weapon_type[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// x/y positions of our weapons
char Weapon_x[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Weapon_y[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// velocities of weapons
char Weapon_xVelo[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Weapon_yVelo[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// timer: various weapons can make use of a fuse timer, or timers for other reasons
char Weapon_time[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// each weapon has varius properties it can have: uses velocity, uses gravity, etc.
// this is an array of bit masks for the current weapons, and what features they have
char Weapon_uses[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// bit mask if the weapon is active in this slot
short Weapon_active = 0;

// because Weapons are defined BEFORE the Game header file, we cant access the cursor target
// thus, whever a target is set, it will have to update our weapon target
short Weapon_targetX;
short Weapon_targetY;

// function prototypes
void Weapons_spawn(char, char, char, char, char, char, char);
void Weapons_update();
char Weapons_weaponsActive();
void Weapons_setTarget(short, short);
