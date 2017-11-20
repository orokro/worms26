/*
	Mines
	-----
	
	This defines globals for Mines on the map.
	
	About Mines:
	
	Mines can either be enabled or disabled.
	If disabled, they will not spawn on the map.
	
	Mines can have pre-set fuse lenghs, or random fuse lengths.
	
	Mines can optionally have duds, which fizzle out and do not explode
	when their fuse runs out.
	
	Note about dudes: since 10 mines will spawn per round, randomly all over
	the map, we can say: the mine at index 0 will be a dud, if dudes are enabled.
	
	There will never be more than one dud, and if duds are enabled we can
	just use index 0, which will randomly be placed - no need to choose
	a random index.
	
*/

// define the trigger distance in pixels for a worm near a mine
#define mineTriggerDistance 10

// x/y positions of our Mines
extern short Mine_x[10];
extern short Mine_y[10];

// velocity of mines
extern char Mine_xVelo[10];
extern char Mine_yVelo[10];

// fuse of mines
extern char Mine_fuse[10];

// is the mine active?
extern int Mine_active;

// function prototypes
void Mines_spawnMines();
void Mines_update();