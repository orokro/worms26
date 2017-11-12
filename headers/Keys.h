// Header File
// Created 11/11/2017; 11:34:22 PM

/*
	Keys
	----
	
	In the previous version of the Worms source code, keys were tested directly
	and there were some ugly hacks associated with doing so.
	
	For instance, if I wanted to make sure a key wasn't tested every frame,
	I would write something ugly like this:
	
	if(_keytest(RR_2ND))
	{
		// wait until key is released
		while(_keytest(RR_2ND));
		
		// do a thing
	}
	
	The while loop would essentially crash/lock the program until it's released.
	
	This way, when it detects a key down, it wouldn't resume the code until the
	key is up, ensuring it was only pressed once.
	
	This has a few downsides
		1) it can be abused. If you want to stop the clock, you can hold a button
		2) it will also pause physics
		3) screen goes black and white because flipping the gray buffer is paused
		4) it has those ugly while loops peppered throughout the keytest code
		
	This new key system works a bit differently.
	
	The game only cares about the following keys:
	
	2nd - fire, place target, select weapon from weapons menu
	Enter - same as 2nd
	diamond - jump
	alpha - backflip
	shift - enable manual camera scrolling
	escape - pause game, or exit weapons menu
	catalog - open weapon menu
	apps - select worm in worm-select mode	
	F1 - open weapon menu
	up/down/left/right - move worm, aim, pan camera, move arrow, select weapon
	plus - rotate girder clockwise
	minus - rotate girder counter-clockwise
	1/2/3/4/5 - set fuse length or cluster size on timed / cluster weapons
	
	So we have a total of 20 keys we care about:
	2nd, Enter, diamond, alpha, shift, escape, catalog, apps, F1, up, down, left, right, plus, minus, 1, 2, 3, 4, 5

	Instead of having inline key tests and waiting for the key to release, we can do something better.
	
	This key system defines three unsgined, and some bitmasks. On every frame we will check all keys and update
	our ints appropriately.
	
	In the game logic, we can test these shorts with their bitmasks.
	
	One int bit will only be 1 when keys are first down, one int bit will always reflect the state of the key
	and one int bit will only be true on the frame the key is released.
	
	This way, the key can test the keyDown int for the desired key, and it will only be true
	on the frame that the key was down.
	
	Other things, like waking or moving the cursor only care if the key is currently down,
	and instead can test the keyState int.
	
	This header will define the public keytest functions as well as the bitmasks for the keys we care about,
	and a key-update method that we will call at the beginning of every frame.
	
	This way, by the time the gamelogic is ready to check keys, the latest-states will be available.
	
	We will define three unsigned ints, but they only need to be in the Keys.c file:
	keyDown
	keyState
	keyUp
	
	and provide public methods for testing keys with our custom defines, instead of the TI-89 keycodes
*/

// our bitmasks for the keys we care about. we will use names that either reflect the physical button
// or the buttons game-logic name
// note the powers of 2! moving the bit one place increased by powers of 2. Yay computer science
#define keyAction (long)1					//00000000000000000000000000000001
#define keyJump (long)2						//00000000000000000000000000000010
#define keyBackflip (long)4				//00000000000000000000000000000100
#define keyCameraControl (long)8	//00000000000000000000000000001000
#define keyEscape (long)16				//00000000000000000000000000010000
#define keyWeaponsMenu (long)32		//00000000000000000000000000100000
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

// useful macro to check a single bit
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

// function declarations
void Keys_update();
char Keys_keyDown();
char Keys_keyState();
char Keys_keyUp();

