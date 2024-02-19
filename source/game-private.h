#pragma once

#ifndef GAME_PRVATE_H
#define GAME_PRIVATE_H

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "game.h"
#include "graphic.h"
#include "keyboard.h"
#include "sound.h"

struct Disk
{
    unsigned int x;
    unsigned int y;

    unsigned int goalX;
    unsigned int goalY;

    double vx;
    double vy;

    int angle;
};

struct Puck
{
    unsigned int x;
    unsigned int y;

    double vx;
    double vy;

    int angle;
};

enum States updateStopped();
enum States updateWin();
enum States updateLost();
enum States updatePlaying(struct InputEvent *inputEvent);
void interactionPuckDisk(struct Puck *puck, struct Disk *disk);
void movePuck(struct Puck *puck);
void changeGoalDiskUser(unsigned int pos_x, unsigned pos_y);
void stepDiskUser(int step_x, int step_y);
void moveDisk(struct Disk *disk, const unsigned velocity);
void makeDecision();
void resetGame();
void drawGame();

int calculateAngle(double vx, double vy);

#endif