/*
	PhysCol.h
	---------
	
	Main header file for the PhysCol.c module
*/

#ifndef PHYSCOL_H
#define PHYSCOL_H


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
	
	// the objects bounciness on X and Y:
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
	
	// the number of frames since this object hasn't moved
	// when this reaches 3, it will be considered "settled"
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
 * a collider can test various points around an objects center, and reposition the object appropriately if it collides with the land.
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
 * @param bounciness the percent amount (0.0 - 1.0) the objects velocity should be after colliding. Note: this sets both bouncinessX and bouncinessY. They can manually be set later if they need to be different.
 * @param smoothness the percent amount (0.0 - 1.0) the object should randomly change x directly when bouncing with the ground. 1.0 = always the same x direction. 0.0 = random direction.
 * @param settled the value that should be bit-masked if the object is considered settled (e.g. hasn't moved in multiple frames)
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
 * @param impact if set to TRUE, the velocity will be applied immediately, if not, it might skip frames to simulate smoother movement.
*/
extern void Physics_setVelocity(PhysObj *obj, char x, char y, char additive, char impact);


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
 * If the point collides with the map, it will calculate the first available point in the opposite direction.
 * This method returns 0 if there is no collision, or the amount of pixels in the opposite direction for the
 * first non map pixel. This return value can be added to the items X/Y so it's not colliding.
 * The valid directions are COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT
 *
 * @param x the x position in world space to test
 * @param y the y position in world space to test
 * @param dir the direction of the test. Can be: COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT
*/
extern short Collide_test(short, short, char);


#endif