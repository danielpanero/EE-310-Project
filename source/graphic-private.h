#pragma once

#ifndef GRAPHIC_PRVATE_H
#define GRAPHIC_PRIVATE_H

#include <nds.h>
#include <stdio.h>

#include "constants.h"
#include "graphic.h"
#include "timer.h"

#include "subBackgrounds.h"
#include "mainBackground.h"
#include "sprites.h"

struct Sprite
{
    u16 *gfx;
    unsigned int index;
};

void initMain();
void initSub();

unsigned int convertFieldCoordinateX(unsigned int x);
unsigned int convertFieldCoordinateY(unsigned int y);

#endif