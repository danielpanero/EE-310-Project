#include "keyboard-private.h"

// External miscellaneous functions
// =================================================
void keyBoardInit()
{
    REG_KEYCNT = (1 << 14) | KEY_START;

    irqSet(IRQ_KEYS, &keyBoardISR);
}

void keyBoardStart()
{
    irqEnable(IRQ_KEYS);
}

void keyBoardStop()
{
    irqDisable(IRQ_KEYS);
    scanKeys();
}

struct InputEvent keyBoardPoll()
{
    scanKeys();
    unsigned int keys = keysHeld();

    struct InputEvent inputEvent = {.touchPressed = false, .keyPressed = false, .x = 0, .y = 0, .key = 0};
    if (keys & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);

        inputEvent.touchPressed = true;
        inputEvent.x = touch.px;
        inputEvent.y = touch.py;
    }

    if (keys & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT))
    {
        inputEvent.keyPressed = true;
        inputEvent.key = keys;
    }

    return inputEvent;
}

// Internal miscellaneous functions
// =================================================
void keyBoardISR()
{
    u16 keys = ~(REG_KEYINPUT);
    if (keys & KEY_START)
    {
        gameStart();
    }
};
