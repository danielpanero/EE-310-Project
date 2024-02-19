#include "timer-private.h"

// External miscellaneous functions
// =================================================
void timerAnimationInit()
{
    TIMER_DATA(0) = TIMER_FREQ_64(ANIMATION_FREQ);
    TIMER_CR(0) = TIMER_ENABLE | TIMER_DIV_64 | TIMER_IRQ_REQ;

    irqSet(IRQ_TIMER0, &timerISR);
};

void timerAnimationStart()
{
    irqEnable(IRQ_TIMER0);
};

void timerAnimationStop()
{
    irqDisable(IRQ_TIMER0);
};

// Internal miscellaneous functions
// =================================================
void timerISR()
{
    graphicAnimate();
};