#pragma once

#ifndef GRAPHIC_H
#define GRAPHIC_H

void graphicInit();

void graphicDrawDisk1(unsigned int x, unsigned int y, int angle);
void graphicDrawDisk2(unsigned int x, unsigned int y, int angle);
void graphicDrawPuck(unsigned int x, unsigned int y, int angle);
void graphicDrawHighScore(unsigned int highScore1, unsigned int highScore2);

void graphicCleared();
void graphicShowWin();
void graphicShowLost();

void graphicAnimate();

#endif
