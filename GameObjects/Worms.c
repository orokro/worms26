// C Source File
// Created 11/11/2017; 11:34:05 PM

#include "../Headers/System/Main.h"

// worm variable defaults
short Worm_x[16] = {15, 40, 20, 65, 20, 35, 15, 40, 15, 25, 30, 50, 25, 40, 15, 10};
short Worm_y[16] = {10, 25, 55, 40, 15, 40, 20, 35, 30, 20, 15, 45, 20, 15, 50, 5};
char Worm_xVelo[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Worm_yVelo[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long Worm_dir = 0;
char Worm_health[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
long Worm_isDead = 0;
long Worm_active = 1;
char Worm_mode[16] = {wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };
char Worm_currentWorm = 0;