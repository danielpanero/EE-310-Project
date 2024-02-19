#include "graphic-private.h"

static struct Sprite disk1 = {.index = 0};
static struct Sprite disk2 = {.index = 1};
static struct Sprite puck = {.index = 2};

static struct Sprite digits1[10];
static struct Sprite digits2[10];

static struct Sprite cow[2] = {{.index = 3}, {.index = 4}};

// External miscellaneous functions
// =================================================
void graphicInit()
{
    initMain();
    initSub();
}

void graphicDrawDisk1(unsigned int x, unsigned int y, int angle)
{
    oamSetXY(&oamSub, disk1.index, convertFieldCoordinateX(x), convertFieldCoordinateY(y));
    oamRotateScale(&oamSub, disk1.index, degreesToAngle(angle), intToFixed(1, 8), intToFixed(1, 8));
}

void graphicDrawDisk2(unsigned int x, unsigned int y, int angle)
{
    oamSetXY(&oamSub, disk2.index, convertFieldCoordinateX(x), convertFieldCoordinateY(y));
    oamRotateScale(&oamSub, disk2.index, degreesToAngle(angle), intToFixed(1, 8), intToFixed(1, 8));
}

void graphicDrawPuck(unsigned int x, unsigned int y, int angle)
{
    oamSetXY(&oamSub, puck.index, convertFieldCoordinateX(x), convertFieldCoordinateY(y));
    oamRotateScale(&oamSub, puck.index, degreesToAngle(angle), intToFixed(1, 8), intToFixed(1, 8));
}

void graphicDrawHighScore(unsigned int highScore1, unsigned int highScore2)
{
    for (int i = 0; i <= 9; i++)
    {
        if (i == highScore1)
        {
            oamSetHidden(&oamSub, digits1[i].index, false);
        }
        else
        {
            oamSetHidden(&oamSub, digits1[i].index, true);
        }

        if (i == highScore2)
        {
            oamSetHidden(&oamSub, digits2[i].index, false);
        }
        else
        {
            oamSetHidden(&oamSub, digits2[i].index, true);
        }
    }
}

void graphicCleared()
{
    oamSetPriority(&oamSub, disk1.index, 0);
    oamSetPriority(&oamSub, disk2.index, 0);
    oamSetPriority(&oamSub, puck.index, 0);

    oamSetPriority(&oamSub, cow[0].index, 3);
    oamSetPriority(&oamSub, cow[1].index, 3);

    BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_0;
    BGCTRL_SUB[1] = BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_1;

    timerAnimationStop();
}

void graphicShowLost()
{
    oamSetPriority(&oamSub, disk1.index, 3);
    oamSetPriority(&oamSub, disk2.index, 3);
    oamSetPriority(&oamSub, puck.index, 3);

    oamSetPriority(&oamSub, cow[0].index, 0);
    oamSetPriority(&oamSub, cow[1].index, 0);

    BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_1;
    BGCTRL_SUB[1] = BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_0;

    timerAnimationStart();
}

void graphicShowWin()
{
    oamSetPriority(&oamSub, disk1.index, 3);
    oamSetPriority(&oamSub, disk2.index, 3);
    oamSetPriority(&oamSub, puck.index, 3);

    oamSetPriority(&oamSub, cow[0].index, 3);
    oamSetPriority(&oamSub, cow[1].index, 3);

    BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_1;
    BGCTRL_SUB[1] = BG_COLOR_256 | BG_MAP_BASE(2) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_0;

    timerAnimationStart();
}

void graphicAnimate()
{
    // Background animation
    static int bgShift = -5;
    static int bgDirection = -1;

    // Moving text up and down
    bgShift += bgDirection;
    if (abs(bgShift) > ANIMATION_MAX_BG_SHIFT)
    {
        bgDirection *= -1;
    }
    REG_BG1VOFS_SUB = bgShift;
    REG_BG2VOFS_SUB = bgShift;

    // Cow animation
    static int x = 0;
    static int cowSprite = -1;
    if (x <= SCREEN_WIDTH - 16)
    {
        x += 1;
    }
    else
    {
        x = 0;
    }

    oamSetXY(&oamSub, cow[0].index, x, ANIMATION_COW_VERTICAL_POS);
    oamSetXY(&oamSub, cow[1].index, x, ANIMATION_COW_VERTICAL_POS);

    if (bgDirection == -1)
    {
        oamSetPriority(&oamSub, cow[0].index, 0);
        oamSetPriority(&oamSub, cow[1].index, 3);
    }
    else
    {
        oamSetPriority(&oamSub, cow[0].index, 3);
        oamSetPriority(&oamSub, cow[1].index, 0);
    }
}

// Internal miscellaneous functions
// =================================================

/* Setting up the mainscreen:
    - VRAM_A_CR for the main background in rotoscale mode
 */
void initMain()
{
    VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;
    REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;

    BGCTRL[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

    REG_BG2PA = 256;
    REG_BG2PC = 0;
    REG_BG2PB = 0;
    REG_BG2PD = 256;

    dmaCopy(mainBackgroundPal, BG_PALETTE, mainBackgroundPalLen);
    dmaCopy(mainBackgroundBitmap, BG_GFX, mainBackgroundBitmapLen);
}

/* Setting up the subscreen:
    - VRAM_C_CR for the main background in tiled mode
    - VRAM_D_DR for the sprites
 */
void initSub()
{
    REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG1_ACTIVE | DISPLAY_BG0_ACTIVE;

    // Setting up the main background
    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;

    BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_0;
    BGCTRL_SUB[1] = BG_COLOR_256 | BG_MAP_BASE(1) | BG_TILE_BASE(1) | BG_32x32 | BG_PRIORITY_1;

    dmaCopy(subBackgroundsTiles, BG_TILE_RAM_SUB(1), subBackgroundsTilesLen);
    dmaCopy(subBackgroundsPal, BG_PALETTE_SUB, subBackgroundsPalLen);

    dmaCopy(&subBackgroundsMap[0], BG_MAP_RAM_SUB(0), 32 * 48);
    dmaCopy(&subBackgroundsMap[32 * 24], BG_MAP_RAM_SUB(1), 32 * 48);
    dmaCopy(&subBackgroundsMap[32 * 48], BG_MAP_RAM_SUB(2), 32 * 48);

    // Setting up the sprites
    VRAM_D_CR = VRAM_ENABLE | VRAM_D_SUB_SPRITE;
    oamInit(&oamSub, SpriteMapping_1D_32, false);

    dmaCopy(spritesSharedPal, SPRITE_PALETTE_SUB, spritesSharedPalLen);

    for (int i = 0; i <= 9; i++)
    {
        u16 *gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

        digits1[i].gfx = gfx;
        digits2[i].gfx = gfx;

        digits1[i].index = 10 + i;
        digits2[i].index = 20 + i;

        oamSet(&oamSub, digits1[i].index, SCREEN_WIDTH - HIGHSCORE_MARGIN - 16, HIGHSCORE_MARGIN, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, digits1[i].gfx, -1, false, false, false, false, false);
        oamSet(&oamSub, digits2[i].index, SCREEN_WIDTH - HIGHSCORE_MARGIN - 2 * 16, HIGHSCORE_MARGIN, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, digits2[i].gfx, -1, false, false, false, false, false);
    }

    disk1.gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
    disk2.gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
    puck.gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

    cow[0].gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
    cow[1].gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

    dmaCopy(disk1Tiles, disk1.gfx, disk1TilesLen);
    dmaCopy(disk2Tiles, disk2.gfx, disk2TilesLen);
    dmaCopy(puckTiles, puck.gfx, puckTilesLen);

    dmaCopy(cow1Tiles, cow[0].gfx, cow1TilesLen);
    dmaCopy(cow2Tiles, cow[1].gfx, cow2TilesLen);

    dmaCopy(n0Tiles, digits1[0].gfx, n0TilesLen);
    dmaCopy(n1Tiles, digits1[1].gfx, n1TilesLen);
    dmaCopy(n2Tiles, digits1[2].gfx, n2TilesLen);
    dmaCopy(n3Tiles, digits1[3].gfx, n3TilesLen);
    dmaCopy(n4Tiles, digits1[4].gfx, n4TilesLen);
    dmaCopy(n5Tiles, digits1[5].gfx, n5TilesLen);
    dmaCopy(n6Tiles, digits1[6].gfx, n6TilesLen);
    dmaCopy(n7Tiles, digits1[7].gfx, n7TilesLen);
    dmaCopy(n8Tiles, digits1[8].gfx, n8TilesLen);
    dmaCopy(n9Tiles, digits1[9].gfx, n9TilesLen);

    oamSet(&oamSub, disk1.index, 0, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, disk1.gfx, disk1.index, true, false, false, false, false);
    oamSet(&oamSub, disk2.index, 0, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, disk2.gfx, disk2.index, true, false, false, false, false);
    oamSet(&oamSub, puck.index, 0, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, puck.gfx, puck.index, true, false, false, false, false);

    oamSet(&oamSub, cow[0].index, 0, 0, 3, 0, SpriteSize_16x16, SpriteColorFormat_256Color, cow[0].gfx, cow[0].index, true, false, false, false, false);
    oamSet(&oamSub, cow[1].index, 0, 0, 3, 0, SpriteSize_16x16, SpriteColorFormat_256Color, cow[1].gfx, cow[1].index, true, false, false, false, false);
}

unsigned int convertFieldCoordinateX(unsigned int x)
{
    return x + (SCREEN_WIDTH - FIELD_WIDTH) / 2 - 8; // -8 in order to account for the doubling of the size of the sprite for rotation
}
unsigned int convertFieldCoordinateY(unsigned int y)
{
    return y + (SCREEN_HEIGHT - FIELD_HEIGHT) / 2 - 8; // -8 in order to account for the doubling of the size of the sprite for rotation
}