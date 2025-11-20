#ifndef LIFEBAR_SYSTEM_H
#define LIFEBAR_SYSTEM_H

#include <genesis.h>

#define TILE_LIFEBAR_BASE 1984 // define a posição na vram

enum {
    LIFEBAR_END,
    LIFEBAR_EMPTY,
    LIFEBAR_FULL,
    LIFEBAR_25,
    LIFEBAR_50,
    LIFEBAR_75,
    LIFEBAR_CLEAR,
};

void initLifebar();
void drawLifeBar(u16 plan, u16 x, u16 y, u16 currentLife, u16 maxLife, u8 length);
void hideLifeBar(u16 plan, u16 x, u16 y, u8 length);

#endif