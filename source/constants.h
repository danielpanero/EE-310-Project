#pragma once

#ifndef CONSTANT_H
#define CONSTANT_H

#include <stdbool.h>

extern const unsigned int FIELD_WIDTH;
extern const unsigned int FIELD_HEIGHT;

extern const unsigned int DISK_WIDTH;
extern const unsigned int DISK_HEIGHT;
extern const unsigned int DISK_RADIUS;

extern const unsigned int PUCK_WIDTH;
extern const unsigned int PUCK_HEIGHT;
extern const unsigned int PUCK_RADIUS;

extern const unsigned int SLOT_WIDTH;
extern const unsigned int SLOT_HEIGHT;

extern const unsigned int DISK_STEP;
extern const unsigned int DISK_VELOCITY_USER;
extern const unsigned int DISK_VELOCITY_BOT;
extern const unsigned int DISK_PRECISION;

extern const unsigned int PUCK_STEP;
extern const double COLLISION_FACTOR;
extern const double PUCK_FRICTION;

extern const double ANIMATION_FREQ;
extern const double ANIMATION_MAX_BG_SHIFT;
extern const unsigned int HIGHSCORE_MARGIN;

extern const unsigned int ANIMATION_COW_VERTICAL_POS;
extern const unsigned int ANIMATION_COW_SPEED;

enum States
{
    STOPPED,
    PLAYING,
    WIN,
    LOST
};

struct InputEvent
{
    bool keyPressed;
    unsigned int key;

    bool touchPressed;
    unsigned int x;
    unsigned int y;
};

#endif