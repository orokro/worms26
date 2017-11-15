// Header File
// Created 11/11/2017; 11:31:16 PM

/*
	Crates
	------
	
	This defines globals for Crates on the map.
	
	About Crates:
	
	There are 3 types of crates: weapon, health, tool.
	Each type can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	Crates have no velocity - they can fall down, but not moved otherwise.
	
	Crates have health, and will explode if drained of health.
*/

#define crateHealth 0
#define crateWeapon 1
#define crateTool 2

// x/y positions of our Crates
extern short Crate_x[8];
extern short Crate_y[8];

// health of our crates
extern char Crate_health[8];

// type of each Crate
extern char Crate_type[8];

// is the crate active?
extern int Crate_active;

// function prototypes
void Crates_spawnCrate(char);
void Crates_update();