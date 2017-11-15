// Header File

/*
	OilDrums
	--------
	
	This defines globals for OilDrums on the map.
	
	About OilDrums:
	
	They can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	OilDrums have no velocity - they can fall down, but not moved otherwise.
	
	OilDrums have health, and will explode if drained of health.
*/

// x/y positions of our oil drums
extern short OilDrum_x[8];
extern short OilDrum_y[8];

// health of our oil drums
extern char OilDrum_health[8];

// this int will be a bit-wise mask for the drums
// the first 8 bits will represent if the drums are active or not
extern int OilDrum_active;

// function prototypes
void OilDrums_spawnDrums();
void OilDrums_update();