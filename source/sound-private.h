#pragma once

#ifndef SOUND_PRVATE_H
#define SOUND_PRIVATE_H

#include <nds.h>
#include <maxmod9.h>
#include <stdio.h>
#include <fat.h>

#include "sound.h"
enum MusicStates
{
    STOPPED,
    MUSIC,
    EFFECT
};

enum Effects
{
    EFFECT_WIN,
    EFFECT_LOST
};

struct Sound
{
    FILE *file;
    mm_word samplingRate;
    mm_stream_formats format;
};

mm_word fillMusicStream(mm_word length, mm_addr dest, mm_stream_formats format);

#endif