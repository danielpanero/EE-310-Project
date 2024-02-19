
#include <nds.h>
#include <stdio.h>
#include <fat.h>

#include "graphic.h"
#include "timer.h"
#include "keyboard.h"
#include "sound.h"
#include "game.h"

int main(void)
{
  fatInitDefault();
  graphicInit();

  timerAnimationInit();
  keyBoardInit();
  soundInit();

  gameInit();
  soundMusicStart();
  while (1)
  {
    gameUpdate();
    mmStreamUpdate();

    swiWaitForVBlank();
    oamUpdate(&oamSub);
  }
}
