// Header File

/*
	Explosions
	----------
	
	This defines globals for Explosions
	
	About Explosions:
	
	They will have a max-radius size
	They will have a damage amount, more powerful explosions will propell worms further
	They can optionally spawn fire-particles
*/

// x/y positions of our Explosions
extern short Explosion_x[8];
extern short Explosion_y[8];

// current time of our explosion
// note that we will consider <=0 time an inactive explosion
extern char Explosion_time[8];

// the size of the explosion
extern char Explosion_size[8];

// the damage power of the explosion
extern char Explosion_power[8];

/*
	this int will store a bitmask for the first-frame of an explosion
  when an explosion is created, it will set it's corresponding bit to 1
  after it's first update the bit will be zero.
	
	since explosions update AFTER other objects, they will have a chance to see the first
	frame of an explosion.
	
	explosions will only cause damage on the first frame
*/
extern int Explosion_firstFrame;

// function prototypes
void Explosion_spawn(short, short, char, char, char);
void Explosion_update();