#include "sound-private.h"

static enum MusicStates state = STOPPED;
static enum Effects currentEffect;

static mm_stream *musicStream;

static struct Sound soundMusic = {.samplingRate = 44100, .format = MM_STREAM_16BIT_MONO};
static struct Sound soundEffectWin = {.samplingRate = 44100, .format = MM_STREAM_16BIT_MONO};
static struct Sound soundEffectLost = {.samplingRate = 44100, .format = MM_STREAM_16BIT_MONO};

// External miscellaneous functions
// =================================================
void soundInit()
{
    soundMusic.file = fopen("./sound/music.wav", "rb");
    soundEffectWin.file = fopen("./sound/win.wav", "rb");
    soundEffectLost.file = fopen("./sound/lost.wav", "rb");

    sassert(soundMusic.file, "File music not found!");
    sassert(soundEffectLost.file, "File lost not found!");
    sassert(soundEffectWin.file, "File win not found!");

    mm_ds_system sys;
    sys.mod_count = 0;
    sys.samp_count = 0;
    sys.mem_bank = 0;
    sys.fifo_channel = FIFO_MAXMOD;
    mmInit(&sys);

    musicStream->sampling_rate = 44100;
    musicStream->buffer_length = 20000;
    musicStream->callback = fillMusicStream;
    musicStream->format = MM_STREAM_16BIT_MONO;
    musicStream->timer = MM_TIMER2;
    musicStream->manual = 1;

    mmSetModuleVolume(512);

    mmStreamOpen(musicStream);
}

void soundMusicStop()
{
    state = STOPPED;
}

void soundMusicStart()
{
    state = MUSIC;

    rewind(soundMusic.file);

    musicStream->sampling_rate = soundMusic.samplingRate;
    musicStream->format = soundMusic.format;
}

void soundWin()
{
    state = EFFECT;
    currentEffect = EFFECT_WIN;

    rewind(soundEffectWin.file);

    musicStream->sampling_rate = soundEffectWin.samplingRate;
    musicStream->format = soundEffectWin.format;
}

void soundLost()
{
    state = EFFECT;
    currentEffect = EFFECT_LOST;

    rewind(soundEffectLost.file);

    musicStream->sampling_rate = soundEffectLost.samplingRate;
    musicStream->format = soundEffectLost.format;
}

// Internal miscellaneous functions
// =================================================
mm_word fillMusicStream(mm_word length, mm_addr dest, mm_stream_formats format)
{
    int res;

    switch (state)
    {
    case MUSIC:
        res = fread(dest, 4, length, soundMusic.file);
        if (res != length)
        {
            rewind(soundMusic.file);
            length = res - (res % 4);
        }
        else
        {
            length = res;
        }
        break;

    case EFFECT:
    {
        FILE *fileEffect;
        switch (currentEffect)
        {
        case EFFECT_WIN:
            fileEffect = soundEffectWin.file;
            break;

        case EFFECT_LOST:
            fileEffect = soundEffectLost.file;
            break;
        }

        res = fread(dest, 4, length, fileEffect);
        if (res != length)
        {
            state = MUSIC;
            length = res - (res % 4);
        }
        else
        {
            length = res;
        }
        break;
    }
    break;
    }
    return length;
}