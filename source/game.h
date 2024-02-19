#pragma once

#ifndef GAME_H
#define GAME_H

#include "constants.h"

void gameInit();
void gameStart();
void gameStop();

enum States gameState();

void gameUpdate();

#endif