/*
	PhysCol.c
	---------
	
	This is part of our new physics / collision system.
	
	At the time of writing, not all objects use this, but we have it available.
	
	This provides Physics Objects structs that have common physics properties
	
	C Source File
	Created 11/22/2017; 2:28:07 AM
*/

// includes
#include "Main.h"
#include "PhysCol.h"
#include "Map.h"
#include "Explosions.h"
#include "Weapons.h"
#include "Worms.h"
#include "Game.h"


/**
 * Makes a new collider object
 *
 * @param type - the type of collider
 * @param up - how far up to test
 * @param down - how far down to test
 * @param left - how far left to test
 * @param right - how far right to test
 * @returns - the new Collider object
 */
void new_Collider(Collider *obj, unsigned char type, char up, char down, char left, char right)
{
	obj->type = type;
	obj->u = up;
	obj->d = down;
	obj->l = left;
	obj->r = right;
	obj->collisions = 0;
}


/**
 * makes a new PhysObj
 *
 * @param *x - the x position
 * @param *y - the y position
 * @param *xVelo - the initial x velocity
 * @param *yVelo - the initial y velocity
 * @param bounciness - bounciness factory
 * @param smoothness - smoothness
 * @param objIndex
 * @param settled
 * @param c - the collider to test for
 * @returns - the new PhysObj
 */	
void new_PhysObj(PhysObj *obj, short *x, short *y, char *xVelo, char *yVelo, char bounciness, char smoothness, char objIndex, unsigned short *settled)
{
	obj->x = x;
	obj->y = y;
	obj->xVelo = xVelo;
	obj->yVelo = yVelo;
	obj->bouncinessX = ((float)bounciness)/100;
	obj->bouncinessY = obj->bouncinessX;
	obj->smoothness = smoothness;
	obj->settled = settled;
	obj->index = objIndex;
	obj->lastX = 0;
	obj->lastY = 0;
	obj->staticFrames = 0;
}


/**
 * tests for a collision and moves the opposite direction until a free pixel is found, if it did collide
 * @param x - test x
 * @param y - test y
 * @param dir - direction to test from
 */
short Collide_test(short x, short y, char dir)
{
	// test for a collision, and if none, just return 0
	if(Map_testPoint(x, y)==FALSE)
		return 0;
		
	// get the direction to move:
	char xDir=0;
	char yDir=0;
	
	switch(dir)
	{
		case COL_UP:    xDir=0; yDir=1;  break;
		case COL_DOWN:  xDir=0; yDir=-1; break;
		case COL_LEFT:  xDir=1; yDir=0;  break;
		case COL_RIGHT: xDir=-1; yDir=0; break;
	}
	
	// make relative pixel coordinates to test with:
	char xMovement = 0;
	char yMovement = 0;
	short safetyCount = 0;
	
	/*
		move until a free pixel.
		SAFETY FIX: Added safetyCount. If we are buried deeper than 25 pixels,
		something is wrong. Stop searching to prevent CPU freeze.
	*/
	while(Map_testPoint(x+xMovement, y+yMovement))
	{
		xMovement += xDir;
		yMovement += yDir;
		
		safetyCount++;
		if(safetyCount > 25) return 0; // Abort: Too thick, don't teleport/freeze
	}
	
	// return the amount that direction needs to move:
	return (dir < 4) ? yMovement : xMovement;
}


/**
 * Applies an object's collider. This can mutate its X/Y.
 *
 * @param *obj pointer to the Physics Object
 * @return an unsigned char with bits masked based on which collisions were responded to
*/
unsigned char Collider_apply(PhysObj *obj)
{
	Collider *col = &obj->col;
	short *x = obj->x;
	short *y = obj->y;
	char yVelo = *obj->yVelo; 

	// Safety Flag: Did we just hit a ceiling?
	char hitCeiling = 0;

	col->collisions = 0;
	
	// --- UP / DOWN ---
	if(col->type & COL_UD)
	{
		char movedUp = (col->type & COL_UP) ? (Collide_test(*x, (*y)-col->u, COL_UP)) : 0;
		char movedDown = (col->type & COL_DOWN) ? (Collide_test(*x, (*y)+col->d, COL_DOWN)) : 0;
		
		// INTELLIGENT FIX: STRICT VELOCITY GATING
		if(yVelo < 0)
		{
			// CASE 1: Moving UP (Jumping/Jetpack)
			// We ONLY care about the ceiling. We MUST ignore the floor (movedDown).
			// Even if movedUp is 0 (head in air) and movedDown is -20 (feet in rock),
			// we ignore the feet. Gravity will pull us down later.
			
			if(movedUp != 0)
			{
				*y = *y + movedUp;
				col->collisions |= COL_UP;
				hitCeiling = 1;
			}
		}
		else
		{
			// CASE 2: Moving DOWN or STATIC (Falling/Walking)
			// Standard behavior with MTV conflict resolution
			if(movedDown != 0 && movedUp != 0)
			{
				// Both hit. Choose the smaller correction.
				if(abs(movedUp) < abs(movedDown))
				{
					*y = *y + movedUp;
					col->collisions |= COL_UP;
					hitCeiling = 1;
				}
				else
				{
					*y = *y + movedDown;
					col->collisions |= COL_DOWN;
				}
			}
			else if(movedDown != 0)
			{
				*y = *y + movedDown;
				col->collisions |= COL_DOWN;
			}
			else if(movedUp != 0)
			{
				*y = *y + movedUp;
				col->collisions |= COL_UP;
				hitCeiling = 1;
			}
		}
	}
	
	// --- LEFT / RIGHT ---
	if(col->type & COL_LR)
	{
		char movedLeft = (col->type & COL_LEFT) ? (Collide_test((*x)-col->l, *y, COL_LEFT)) : 0;
		char movedRight = (col->type & COL_RIGHT) ? (Collide_test((*x)+col->r, *y, COL_RIGHT)) : 0;
		
		if(movedLeft && movedRight)
		{
			// CRASH FIX:
			// The original code pushes the worm UP (*y = *y - 1) to escape a squeeze.
			// If we are moving UP (yVelo < 0), we are jamming ourselves into the ceiling.
			// We should ONLY climb if we are falling/stable AND definitely didn't just bonk a ceiling.
			
			if (yVelo >= 0 && !hitCeiling)
			{
				short safety = 0;
				// Safety: Prevent infinite loops if geometry is weird
				while(movedLeft && movedRight && safety < 10)
				{
					*y = *y - 1;
					movedLeft = Collide_test(*x-col->l, *y, COL_LEFT);
					movedRight = Collide_test(*x+col->r, *y, COL_RIGHT);
					safety++;
				}
			}
			
			col->collisions |= COL_BOTHLR;
		}
		else if(movedLeft || movedRight)
		{
			*x = *x + (movedLeft+movedRight);
			col->collisions |= ((movedLeft+movedRight)<0 ? COL_RIGHT : COL_LEFT);
		}
	}
	
	return col->collisions;
}


/**
 * does physics for an in game object
 * @param *obj - the object to compute physics for
 */
char Physics_apply(PhysObj *obj)
{
	// before we apply physics, save the inital state of the X/Y
	short initX = *obj->x;
	short initY = *obj->y;

	// Check global flag AND ensure we aren't on the ground
    if((Game_stateFlags & gs_lowGravity) && !(*obj->settled) && *obj->yVelo>0)
    {
        // Apply gravity only 1 out of every 3 frames (33% Gravity)
        // This means on frames 0 and 1, we UNDO the gravity the caller added.
        if(Game_timer % 2 != 0)
        {
            // Subtract 1 to cancel out the caller's "yVelo++" or "yVelo += 1"
            *obj->yVelo = 1;
        }
    }

	// if an object is too far below the water, it is permanently settled, and we dont
	// need to do anything with it.
	if(initY>250)
	{
		*obj->settled |= ((unsigned short)1<<(obj->index));
		return 0;
	}
	
	// apply the velocities (which may be 0), if their bounciness is less than zero
	// we don't ever change that axis
	if(obj->bouncinessX>=0.0f)
		*obj->x += (*obj->xVelo);
	if(obj->bouncinessY>=0.0f)
		*obj->y += (*obj->yVelo);
	
	// apply the collider, which might move our object
	unsigned char hits = Collider_apply(obj);
	
	// calculate if the object moved at all, either from velo or the collider:
	char moved = (*obj->x!=initX || *obj->y!=initY);
	
	// if both our left and right colliders hit, lets just stop the object in its tracks
	if(hits & COL_BOTHLR)
	{
		*obj->xVelo=0;
		*obj->yVelo=0;
		*obj->settled |= ((unsigned short)1<<(obj->index));
		
		// if an axis is locked, prevent it's movement
		if(obj->bouncinessX<0.0f)
			*obj->x = initX;
		if(obj->bouncinessY<0.0f)
		 	*obj->y = initY;
		
		return 0;
		
	// if hit nothing, nothing to do here:
	}
	else if(hits!=0)
	{
		// for left and right hits
		if(hits & COL_LR)
		{
			// shrink xVelo based on bounciness
			// and make sure its positive or negative based on which side hit
			*obj->xVelo = (abs(*obj->xVelo) * ((hits & COL_LEFT) ? 1 : -1))  * obj->bouncinessX;
		}
		
		// for up and down hits
		if(hits & COL_UD)
		{
			// shrink yVelo based on bounciness
			// and make sure its positive or negative based on which side hit
			*obj->yVelo = (abs(*obj->yVelo) * ((hits & COL_UP) ? 1 : -1))  * obj->bouncinessY;

			/*
				smoothness only applies to ground objects.
				based on it's smoothness pick a random number and decided to flip the xVelocity or not
				note that, the smoothness constructor takes a float from 0.0 to 1.0, but internally
				converts it to 0-100
			*/
			if(random(100) > obj->smoothness)
				*obj->xVelo *= -1;
		}
		
		// if it only hit LR and not UD, we should shrink the y bounciness due to friction
		if((hits & COL_LR) && !(hits & COL_UD))
			*obj->yVelo = (*obj->yVelo * obj->bouncinessY);
			
		// like wise for UD and not LR
		if((hits & COL_UD) && !(hits & COL_LR))
			*obj->xVelo = (*obj->xVelo * obj->bouncinessX);
		
		
		// if either of the velocities are 1, we will make them 0, since rounding errors can sometimes
		// force objects to never "settle"
		if(abs(*obj->xVelo) <= 1)
			*obj->xVelo = 0;
		if(abs(*obj->yVelo) <= 1)
			*obj->yVelo = 0;
		
		
	}// end if hit something
	
	/*
		So, when objects move, sometimes their colliders can put them in a state
		where they move every frame no matter once, thus never becoming settled.
		
		Because they aren't really in-motion at this point, their velocities should
		be very low, or 0.
		
		So basically, we want to count static frames only when the object was moved
		with a velocity that's big enough.
		
		If the velocity is small, it doesn't matter if it was moved (since it will
		sometimes move on every frame).
		
		If the velocity is small for 6 frames in a row, we will consider the object
		to be settled. If the object truly was in motion, it's velocity would
		never stay less than 2 for more than 6 frames, just due to gravity alone.
	*/
	if((abs(*obj->xVelo)>2 || abs(*obj->yVelo)>2) && moved)
	{
		// reset counter and set object settled
		obj->staticFrames = 0;
		*obj->settled &= ~((unsigned short)1<<(obj->index));
	
	// otherwise, increase our frame count of static frames, and if its over 6, consider it settled
	}else
	{
		obj->staticFrames++;
		if(obj->staticFrames>6)
		{
			// reset counter and set object settled
			obj->staticFrames = 0;
			*obj->settled |= (unsigned short)1<<(obj->index);
		}
	}
	
	// if an axis is locked, prevent it's movement
	if(obj->bouncinessX<0.0f)
		*obj->x = initX;
	if(obj->bouncinessY<0.0f)
		*obj->y = initY;
		
	
	// return weather or not the object was moved in this frame
	return moved;
}


/**
 * sets or adds velocity to an object
 * @param *obj - physics object we're applying velocity to
 * @param x - x velocity to add
 * @param y - y velocity to add
 * @param additives - adds to existing velocity instead of setting it
 * @param impact - if TRUE, applies the velocity immediately, if FALSE, may skip frames for smoother movement
 */
void Physics_setVelocity(PhysObj *obj, char x, char y, char additive, char impact)
{
	static char skipFrame = 0;

	// if not impact, we may skip frames for smoother movement
	if(!impact)
	{
		skipFrame = (skipFrame+1)%2;
		if(skipFrame%2==0)
			return;
	}

	// add or set the velocity
	if(additive)
	{
		*obj->xVelo += x;
		*obj->yVelo += y;
	}else
	{
		*obj->xVelo = x;
		*obj->yVelo = y;
	}
	
	// any object that had velocity added is definitely "not" settled
	obj->staticFrames = 0;
	*obj->settled &= ~((unsigned short)1<<(obj->index));
}


/**
 * check if an object is hit by active explosions
 */
short Physics_checkExplosions(PhysObj *obj)
{
	// if no explosions are in their first frame, no need to check 'em
	if(Explosion_firstFrame==0)
		return 0;
		
	// calculate total damage from all possible explosions
	short totalDamage = 0;
	short i=0;
	for(i=0; i<MAX_EXPLOSIONS; i++)
	{
		// check if the explosion is in it's first-frame
		unsigned short firstFrame = (Explosion_firstFrame & (unsigned short)1<<i);
		
		// only do shit if first frame, yo
		if(firstFrame)
		{
			// FIX 1: Use longs for delta calculation to prevent overflow when squaring
			long dx = (long)(*obj->x) - (long)Explosion_x[i];
			long dy = (long)(*obj->y) - (long)Explosion_y[i];
			
			// Calculate distance using floats/longs to avoid 16-bit overflow
			float d = sqrt((float)(dx*dx + dy*dy));
			
			// get the explosions size, and large size
			short radius = Explosion_size[i];
			short largerRadius = (short)(1.5f * Explosion_size[i]);
			
			// pending damage...
			float pendingDamageRatio = -1.0f;
			
			// if we're withing the blast radius, take full damage:
			if(d<radius)
				pendingDamageRatio = 1.0f;
			else if(d<largerRadius)
			{
				// subtract the minimum radius from both:
				float minD = d - radius;
				float minL = largerRadius - radius;
				
				// calculate how far away we are:
				pendingDamageRatio = (1.0f - (minD/minL));
			}// end if within larger radius
			
			// if damage was done, we should add it to our total, and apply velocity
			if(pendingDamageRatio>0.0f)
			{
				// total damage done so far
				totalDamage += (short)(pendingDamageRatio*Explosion_power[i]);
				
				// calculate the power to add in this direction:
				// using size for "physical force" magnitude
				float power = (pendingDamageRatio*Explosion_size[i])*3; 
				
				// FIX 2: Correct Vector Projection
				// Previously (x/y) would cause division by zero or infinite force 
				// if the worm was aligned horizontally/vertically.
				// We use (dx/d) to get the normalized direction vector.
				
				// Multiplier to tune the "kick" strength. 
				// 0.3f provides a similar feel to your original 0.1f without the infinite spikes.
				float kickScale = 0.3f; 
				
				if(d > 0.1f) // Prevent div by zero
				{
					short xPow = (short)((dx / d) * power * kickScale);
					short yPow = (short)((dy / d) * power * kickScale);
					
					// apply velocity!
					Physics_setVelocity(obj, xPow, yPow, TRUE, TRUE);
				}
			}
		}// end if first frame
	}// next i
	
	// return the total damage that was done
	return totalDamage;
}


/**
 * check if a worm collides with a weapon
 * 
 * @param wormIndex the index of the worm to test
 * @param wormMask the bitmask of the worm to test
 * @param *obj the physics object to test with
 * @return the total damage taken by the worm
 */
short Physics_checkWeapons(short wormIndex, unsigned short wormMask, PhysObj *obj)
{
	// gtfo early if no active weapons or if the worm is dead/inactive
	if(Weapon_active==0 || (Worm_isDead & wormMask) || !(Worm_active & wormMask))
		return 0;

	short i;
	short totalDamage = 0;

	// worm hit box around physobj
	short wormMinX = (short)(*obj->x - 3);
	short wormMaxX = (short)(*obj->x + 3);
	short wormMinY = (short)(*obj->y - 5);
	short wormMaxY = (short)(*obj->y + 4);

	// weapon half-size: 3 => 6x6, 4 => 8x8
	// tune this
	const short weaponHalf = 2;

	for(i=0; i<MAX_WEAPONS; i++)
	{
		// skip inactive weapons
		if((Weapon_active & (unsigned short)(1<<(i)) )==0)
			continue;

		// early-out X (cheapest + most likely reject)
		{
			short wx = Weapon_x[i];

			if(wx < (short)(wormMinX - weaponHalf))
				continue;
			if(wx > (short)(wormMaxX + weaponHalf))
				continue;
		}

		// early-out Y
		{
			short wy = Weapon_y[i];

			if(wy < (short)(wormMinY - weaponHalf))
				continue;
			if(wy > (short)(wormMaxY + weaponHalf))
				continue;
		}

		// collision!

		// only fire-type weapons can hurt the current worm, so we'll test that first
		if(Weapon_type[i] == WFire || Weapon_type[i] == WFakeFire)
		{
			totalDamage = 3;
			const short randomXVelocity = (random(5) - 2); // -3 to +3
			Physics_setVelocity(&Worm_physObj[wormIndex], randomXVelocity, -2, TRUE, TRUE);
		}

		// if we're the current worm, skip the rest of the checks
		if(wormIndex == Worm_currentWorm)
			continue;

		// handle different kinds of weapon collisions
		if(Weapon_type[i] == WDragonBall || Weapon_type[i] == WKamikaze)
		{
			totalDamage += 12;
		}
		else if(Weapon_type[i] == WLongbow)
		{
			totalDamage += 15;
			Physics_setVelocity(&Worm_physObj[wormIndex], Weapon_xVelo[i], -2, TRUE, TRUE);
		}
		else if(Weapon_type[i] == WSkunkGas)
		{
			totalDamage += 1;
			Worm_poisoned |= wormMask;
			// WSkunkGas lingers, so it is not detonated on impact.
		}
		else if(Weapon_type[i] == WDrill)
		{
			totalDamage += 5;
			Physics_setVelocity(&Worm_physObj[wormIndex], random(9) - 4, -4, TRUE, TRUE);
		}
		else if(Weapon_type[i] == WBlowTorch)
		{
			// default damage for other weapons
			totalDamage += 8;
			Physics_setVelocity(&Worm_physObj[wormIndex], Worm_xVelo[wormIndex], 4, TRUE, TRUE);
		}
		
		if((Weapon_props[(short)Weapon_type[i]] & usesDetonateOnImpact) != 0)
			Weapons_detonateWeapon(i);
	}

	return totalDamage;
}
