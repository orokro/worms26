/*
	Environment.c
	-------------

	This file will be in-line included in Draw.c.

	It's purpose is to make Draw.c more readable, as it's pretty long.

	All the environmental drawing code will go here, EXCEPT map, which is in Map_drawMap.

	So we'll include:
	- Leafs
	- Clouds
	- Mountains
	- Water	
*/

// full screen ref
const SCR_RECT fullScreen = {{0, 0, 159, 99}};


/**
 * draw falling leave wind and cloud speed indicators
*/
void drawLeavesAndClouds()
{
	short i;
	
	// use smaller wind for smoother sprites
	short wind = Game_wind/4;
	
	// no need for external data, only on leaf at a time so we can keep it static
	static short Leaf_x[5] = {10, 30, 60, 90, 120};
	static short Leaf_y[5] = {10, 30, 60, 90, 120};
	
	// only 3 clouds at a time, keep it static:
	static short Cloud_x[] = {0, 53, 106};
	static short Cloud_y[] = {-50, -70, -90};
	
	for(i=0; i<5; i++)
	{
		// leaves always move down at a constant rate, and sideways by the wind rate
		Leaf_y[i]++;
		Leaf_x[i]+=wind;
		
		// get leafs onscreen pos
		short xPos = Leaf_x[i];
		short yPos = Leaf_y[i];
		
		// draw if if on screen
		if(worldToScreen(&xPos, &yPos))
			ClipSprite16_OR_R(xPos, yPos, 8, spr_Leaf, lightPlane);
			
		// or respawn if off screen and far enough below
		else if(yPos>80)
		{
			if(abs(Game_wind)<5 || random(2)==0)
			{
				Leaf_x[i] = camX+random(160)-80;
				Leaf_y[i] = camY-58;
			}else
			{
				Leaf_x[i] = camX + ((Game_wind<0) ? 90 : -90);
				Leaf_y[i] = camY-61 + random(70);
			}
		}		
	} // next l
	
	// loop thru clouds and draw or respawn
	for(i=0; i<3; i++)
	{
		// clouds only move horizontally
		Cloud_x[i] += wind;
		
		// get positions to mutate
		short xPosC = Cloud_x[i];
		short yPosC = Cloud_y[i];
		
		char onScreen = worldToScreen(&xPosC, &yPosC);
		
		// check screen pos:
		if(onScreen)
		{
			// draw the cloud:
			ClipSprite32_MASK_R(xPosC, yPosC, 14, spr_Cloud_Light, spr_Cloud_Mask, lightPlane);
			ClipSprite32_MASK_R(xPosC, yPosC, 14, spr_Cloud_Dark, spr_Cloud_Mask, darkPlane);
		}else if(xPosC < -32 && wind<0)
		{
			Cloud_x[i] = camX+80+31;
			Cloud_y[i] = -50-random(50);
		}else if(xPosC > 160+32 && wind>0)
		{
			Cloud_x[i] = camX-80-31;
			Cloud_y[i] = -50-random(50);
		}
	}// next i
}


/**
 * Draws a grayscale parallax mountain range in the background.
 */
void drawMountains()
{
	short i;
	short offsetX = (camX-160)/5;
	short offsetY = camY/5;
	for(i=0; i<5; i++)
		ClipSprite32_OR_R(i*32-offsetX, 60-offsetY, 38, spr_Mountain[i], lightPlane);
}


/**
 * draws the water infront of the map / objects
*/
void drawWater()
{
	// frame and frame timer
	static char frame=0;
	static char frameTimer=0;
	if(++frameTimer>=2)
	{
		frameTimer=0;
		if(++frame>7)
			frame=0;
	}		
	
	static char frames[] = {0, 1, 2, 3, 4, 3, 2, 1};
	short f = frames[(short)frame];
	
	short waterY = 241-camY - Game_waterLevel;
	short i;
	for(i=0; i<7; i++)
	{
		short waterX = i*32 - (camX+328)%32;
	
		// draw just the and dark light planes:
		ClipSprite32_MASK_R(waterX, waterY, 9, &spr_Water_Dark[0+(f*9)], &spr_Water_Mask[0+(f*9)], darkPlane);
		ClipSprite32_AND_R(waterX, waterY, 9, &spr_Water_Mask[0+(f*9)], lightPlane);
		ClipSprite32_OR_R(waterX, waterY, 9, &spr_Water_Light[0+(f*23)], lightPlane);
	}

	// draw the current worms y position on screen
	//const short currentWormY = Worm_y[(short)Worm_currentWorm];
	//static char buffer[32];
	//sprintf(buffer, "Worm Y: %d", currentWormY);
	//GrayDrawStr2B(50, 1, buffer, A_XOR, lightPlane, darkPlane);

	// if the top of the under-area is offscreen, no need to draw the rest
	if((waterY + 9) >= 99)
		return;

	// draw the water rectangle to fill in below the waterline
	SCR_RECT waterRect = {{0, waterY + 9, 159, 99}};

	// Draw Light Gray: LightPlane = 1 (Black), DarkPlane = 0 (White)
	PortSet(lightPlane, 239, 127);
	ScrRectFill(&waterRect, &fullScreen, A_NORMAL);  // Force 1s
	PortRestore();
	PortSet(darkPlane, 239, 127);
	ScrRectFill(&waterRect, &fullScreen, A_REVERSE);  // Force 0s
	PortRestore();
}
