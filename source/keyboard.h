#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyBoardInit();
void keyBoardStart();
void keyBoardStop();

struct InputEvent keyBoardPoll();

#endif