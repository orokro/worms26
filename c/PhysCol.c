// C Source File
// Created 11/22/2017; 2:28:07 AM

#include "Main.h"
#include "PhysCol.h"

/*
	PhysCol
	-------
	
	This file provides a bunch of generic physics and collision methods,
	which many objects will be using
	
*/

// makes a new collider
Collider new_Collider(unsigned char type, char up, char down, char left, char right)
{
	Collider new;
	new.type = type;
	new.u = up;
	new.d = down;
	new.l = left;
	new.r = right;
	new.collisions = 0;
	return new;
}

// makes a new PhysObj
PhysObj new_PhysObj(short *x, short *y, char *xVelo, char *yVelo, float bounciness, float smoothness, char objIndex, unsigned short *settled, Collider c)
{
	PhysObj new;
	new.x = x;
	new.y = y;
	new.xVelo = xVelo;
	new.yVelo = yVelo;
	new.bouncinessX = bounciness;
	new.bouncinessY = bounciness;
	new.smoothness = (char)(smoothness*100);
	new.settled = settled;
	new.col = c;
	new.index = objIndex;
	new.lastX = 0;
	new.lastY = 0;
	new.staticFrames = 0;
	return new;
}

/**
 * Applys an objects collider. This can mutate it's X/Y.
 *
 * @param *col pointer the Collider struct to apply
 * @param *x pointer the x position to test relatively
 * @param *y pointer the y position to test relatively
 * @return an unsigned char with bits masked based on which collisions were responded to
*/
unsigned char Collider_apply(Collider *col, short *x, short *y)
{

	// reset the collisions bitmask for this collider
	col->collisions = 0;
	
	// do we need to test up and/or down?
	if(col->type & COL_UD)
	{
		// perform up and down tests if need be:
		char movedUp = (col->type & COL_UP) ? (Collide_test(*x, (*y)-col->u, COL_UP)) : 0;
		char movedDown = (col->type & COL_DOWN) ? (Collide_test(*x, (*y)+col->d, COL_DOWN)) : 0;
		
		// testing down takes precedence, so if we moved down at all, just apply that
		if(movedDown!=0)
		{
			*y = *y + movedDown;
			col->collisions |= COL_DOWN;
		// otherwise, we will apply the up movement
		}else if(movedUp!=0)
		{
			*y = *y + movedUp;
			col->collisions |= COL_UP;
		}		
	}// end if up or down collision
	
	// do we need to test left and/or right?
	if(col->type & COL_LR)
	{
		// perform left and right tests if need be:
		char movedLeft = (col->type & COL_LEFT) ? (Collide_test((*x)-col->l, *y, COL_LEFT)) : 0;
		char movedRight = (col->type & COL_RIGHT) ? (Collide_test((*x)+col->r, *y, COL_RIGHT)) : 0;
		
		/*
			unlike up and down colliders, neither can take prescendence.
			we don't want to push the object off the edges of the map, which can happen with a feedback
			loop of collisions. If the left colider and right collider both hit something, instead
			we want to push the item UP since both it's edges are "on ground"
			
			This might end up in some unrealistic behavoir, but its better than pushing the object to the
			far edges of the map.
			
			in this case, we will push the object up, and test again, until neither collider hits.
			
			also note that this mode only needs to apply if we're testing both COL_LEFT and COL_RIGHT
			such as in COL_LR or COL_UDLR
			
			also note that, the movedLeft or movedRight variables would default to 0 if this wasn't
			the case so this IF will only pass if we're testing both LEFT and RIGHT and they both collided
		*/
		if(movedLeft && movedRight)
		{
			// move until neither is colliding any more
			// note that, the worst case scenario is that the worm is moved to the top of the map,
			// in which case both colliders will definately return false, so this won't be an endless while
			while(movedLeft && movedRight)
			{
				// move the object up:
				*y = *y - 1;
				
				// test again. No need for ternarys since the only way to get to this loop is if both are tested
				movedLeft = Collide_test(*x-col->l, *y, COL_LEFT);
				movedRight = Collide_test(*x+col->r, *y, COL_RIGHT);
			}
			
			// we will consider colliding both LEFT and RIGHT the same as colliding with DOWN (the ground)
			// special case: both left and right hit. We should stop movement on this object and settle it ASAP
			col->collisions |= COL_BOTHLR;
			
		// other wise we can just move the object left or right by the some of movedLeft and movedRight
		}else if(movedLeft || movedRight)
		{
			
			/*
				so, we can end up here in 3 scenarios:
					- we are testing both left and right, but one was 0 (i.e. didn't collide)
					- we are testing left, and right defaulted to 0
					- we are testing right, and left defaulted to 0
					
				thus, no matter how you look at it, one will always be 0.
				
				if we add movedLeft and movedRight it will return the ammount to move the x
				value left or right.
				
				if both movedLeft and movedRight were non zero, then the above while loop would have happened
				instead.
				
				Note that, movedLeft and movedRight can be negative for their corresponding direction.
			*/
			*x = *x + (movedLeft+movedRight);
			
			// save which side we hit
			// if our net total is negative, the right collider hit and pushed the object left:
			col->collisions |= ((movedLeft+movedRight)<0 ? COL_RIGHT : COL_LEFT);
		}// end if both hit
		
	}// end if left or right
	
	// return what we collided with, if anything
	return col->collisions;
}


// does physics for an in game object
char Physics_apply(PhysObj *obj)
{
	// before we apply physics, save the inital state of the X/Y
	short initX = *obj->x;
	short initY = *obj->y;
	
	// if an object is too far below the water, it is perminently settled, and we dont
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
	unsigned char hits = Collider_apply(&obj->col, obj->x, obj->y);
	
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
	}else if(hits!=0)
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
		where they move every frame no matter once, thus never becomming settled.
		
		Because they aren't really in-motion at this point, their velocities should
		be very low, or 0.
		
		So basically, we want to count static frames only when the object was moved
		with a velocity that's big enough.
		
		If the velocity is small, it doesn't matter if it was moved (since it will
		sometimes move on every frame).
		
		If the velocity is small for 6 frames in a row, we will consider the object
		to be settled. If the object truely was in motion, it's velocity would
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

// sets or adds velocity to an object
void Physics_setVelocity(PhysObj *obj, char x, char y, char additive)
{

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
	
	// any object that had velocity added is definately "not" settled
	obj->staticFrames = 0;
	*obj->settled &= ~((unsigned short)1<<(obj->index));
}

// tests for a collision and moves the opposite direction until a free pixel is found, if it did collide
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
		case COL_UP:
			xDir=0;
			yDir=1;
			break;
		case COL_DOWN:
			xDir=0;
			yDir=-1;
			break;
		case COL_LEFT:
			xDir=1;
			yDir=0;
			break;
		case COL_RIGHT:
			xDir=-1;
			yDir=0;
			break;
	}
	
	// make relative pixel coordinates to test with:
	char xMovement = 0;
	char yMovement = 0;
	
	/*
		move until a free pixel.
	  worst case scenario is that the worm is put in an invalide state on the map
	  and this function moves them all the way to the edge of the map.
	  
	  Map_testPoint WILL return false once OOB of the map.. so the worm
	  would fly to the left or right of the map. Lets hope the game never lets worms
	  end up in invalid places...
	*/
	while(Map_testPoint(x+xMovement, y+yMovement))
	{
		xMovement += xDir;
		yMovement += yDir;
	}// wend
	
	// return the amount that direction needs to move:
	// COL_UP and COL_DOWN are 0 and 1 respectively, so they are less than 2
	return (dir < 4) ? yMovement : xMovement;
	
}


// check if an object is hit by active explosions
short Physics_checkExplosions(PhysObj *obj)
{
	// if no explosions are in their first frame, no need to check 'em
	if(Explosion_firstFrame==0)
		return 0;
	
	// calculate total damge from all possible explosions
	short totalDamage = 0;
	
	short i=0; 
	for(i=0; i<MAX_EXPLOSIONS; i++)
	{
		// check if the explosion is in it's first-frame
		unsigned short firstFrame = (Explosion_firstFrame & (unsigned short)1<<i);
		
		// only do shit if first frame, yo
		if(firstFrame)
		{
			// get the explosions distance from us
			short d = dist(*obj->x, *obj->y, Explosion_x[i], Explosion_y[i]);
			
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
				// subract the minimum radius from both:
				short minD = d - Explosion_size[i];
				short minL = largerRadius - Explosion_size[i];
				
				// calculate how far away we are:
				pendingDamageRatio = (1.0f - ((float)minD/(float)minL));
			}// end if within larger radius		
			
			// if damage was done, we should add it to our total, and apply velocity
			if(pendingDamageRatio>0.0f)
			{
				// total damage done so far
				totalDamage += (pendingDamageRatio*Explosion_power[i]);
				
				// get the x and y differences from us to the explosion
				short x = *obj->x - Explosion_x[i];
				short y = *obj->y - Explosion_y[i];
				
				// calcualte the power to add in this direction:
				short power = (pendingDamageRatio*Explosion_size[i]);
				
				// use ratio of x/y to determine how much power to apply to each
				short xPow = abs(((float)x/(float)y)*power*0.1f) * ((x>0) ? 1 : -1);
				short yPow = abs(((float)y/(float)x)*power*0.1f) * ((y>0) ? 1 : -1);
				
				// apply velocity!
				Physics_setVelocity(obj, xPow, yPow, TRUE);
			}
				
		}// end if first frame		
	}// next i
	
	// return the total damage that was done
	return totalDamage;
}


