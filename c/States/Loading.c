/*
	Loading.c
	---------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the Loading state machine specific code.
*/



/**
	Called on the first-frame when the Games state machine is set to Loading mode.
*/
static void Loading_enter()
{
	// init state related to starting match
	Game_initRound();
	
	// show the cake loading screen
	Draw_cake(0, 100);
	
	// render the map and spawn items on the map (worms, oil drums, etc)
	Map_makeMap();

	// finished
	Draw_cake(100, 100);

	// switch to game mode worm select after loading
	Game_changeMode(gameMode_WormSelect);
}


/**
	Called every frame that the Games state machine is in Loading mode.
*/
static void Loading_update()
{
	

}


/**
	Called on the first-frame when the Games state machine leaves Loading mode.
*/
static void Loading_exit()
{


}
