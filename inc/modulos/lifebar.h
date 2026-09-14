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

/**
 * @brief 
 * 
 * @param plan Plano onde vai ser desenhada a barra de vida
 * @param numPal Paleta de Cores a ser usada
 * @param x posição X (em tiles) da barra de vida
 * @param y posição Y (em tiles) da barra de vida
 * @param currentLife vida atual
 * @param maxLife máximo de vida
 * @param length tamanho da barra de vida (em tiles)
 */
void drawLifeBar(u16 plan, u16 numPal, u16 x, u16 y, u16 currentLife, u16 maxLife, u8 length);

/**
 * @brief 
 * 
 * @param plan Plano onde vai ser desenhada a barra de vida
 * @param numPal Paleta de Cores a ser usada
 * @param x posição X (em tiles) da barra de vida
 * @param y posição Y (em tiles) da barra de vida
 * @param length tamanho da barra de vida (em tiles)
 */
void hideLifeBar(u16 plan, u16 numPal, u16 x, u16 y, u8 length);

#endif