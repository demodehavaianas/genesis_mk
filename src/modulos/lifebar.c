#include "lifebar.h"

#include <genesis.h>
#include "gfx.h"

void initLifebar()
{
    VDP_loadTileSet(&lifebar, TILE_LIFEBAR_BASE, DMA);
}

// Desenha uma barra de vida horizontal com tamanho variável + terminal
void drawLifeBar(u16 plan, u16 x, u16 y, u16 currentLife, u16 maxLife, u8 length)
{
    if (maxLife == 0 || length == 0)
        return;

    VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, TILE_LIFEBAR_BASE), x-1, y);

    u16 percent = (currentLife * 100) / maxLife;
    u8 filled = (percent * length) / 100;
    u8 remainder = (percent * length) % 100;

    for (u8 i = 0; i < length; i++)
    {
        u16 tileIndex;

        if (i < filled)
        {
            tileIndex = LIFEBAR_FULL;
        }
        else if (i == filled)
        {
            if (remainder >= 75)
                tileIndex = LIFEBAR_75;
            else if (remainder >= 50)
                tileIndex = LIFEBAR_50;
            else if (remainder >= 25)
                tileIndex = LIFEBAR_25;
            else
                tileIndex = LIFEBAR_EMPTY;
        }
        else
        {
            tileIndex = LIFEBAR_EMPTY;
        }

        VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, TILE_LIFEBAR_BASE + tileIndex), x + i, y);
    }

    // Terminal no final da barra
    VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL2, TRUE, FALSE, TRUE, TILE_LIFEBAR_BASE + LIFEBAR_END), x + length, y);
}

// Esconde a barra de vida sobrescrevendo com tiles vazios
void hideLifeBar(u16 plan, u16 x, u16 y, u8 length)
{
    for (u8 i = 0; i < length + 1; i++) // +1 para o terminal
    {
        VDP_setTileMapXY(plan, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, TILE_LIFEBAR_BASE + LIFEBAR_CLEAR), x + i, y);
    }
}