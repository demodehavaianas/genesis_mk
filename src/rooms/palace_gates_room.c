#include <genesis.h>
#include "palace_gates_room.h"
#include "stages.h"
#include "gfx.h"
#include "sprites.h"
#include "game_vars.h"
#include "lifebar.h"
#include "input_system.h"

#define PALACE_GATES_WIDTH 928
#define PALACE_GATES_HEIGHT 232

void initPalaceGatesRoom(void)
{
    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_COLUMN);
    // VDP_setWindowFullScreen();
    
    gInd_tileset = 0;
    u16 life = 100;

    while (TRUE)
    {
        inputSystem();
        gFrames++;

        // carrego as informações do cenário.
        if (gFrames == 1)
        {
            // defino o tamanho do meu cenário para rolagem
            gBG_Width = PALACE_GATES_WIDTH;
            gBG_Height = PALACE_GATES_HEIGHT;

            // 304
            gScrollValue = (gBG_Width - VDP_getScreenWidth()) / 2;

            s16 startX = (VDP_getScreenWidth() - gBG_Width) / 2;
            s16 startY = -(gBG_Height - VDP_getScreenHeight());

            // ------ BACKGROUND A ------
            VDP_loadTileSet(pg_bga.tileset, gInd_tileset, DMA);
            // VDP_setTileMapEx(BG_A, pg_bga.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset),
            //  0, 0, 0, 0, gBG_Width / 8, 29, DMA_QUEUE);
            VDP_setTileMapEx(BG_A, pg_bga.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset),
                             startX / 8, startY / 8, 0, 0, gBG_Width / 8, 29, DMA);
            PAL_setPalette(PAL0, pg_bga.palette->data, DMA);
            gInd_tileset += pg_bga.tileset->numTile;

            // ------ BACKGROUND B ------
            VDP_loadTileSet(pg_bgb.tileset, gInd_tileset, DMA);
            VDP_setTileMapEx(BG_B, pg_bgb.tilemap, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset),
                             0, startY / 8, 0, 0, 64, 29, DMA);
            PAL_setPalette(PAL1, pg_bgb.palette->data, DMA);
            gInd_tileset += pg_bgb.tileset->numTile;

            // ------ LIFEBAR ------
            initLifebar();
            drawLifeBar(BG_A, PAL1, 1, 1, life, 100, 16);
            drawLifeBar(BG_A, PAL1, 23, 1, life, 100, 16);

            // ------ PLAYERS ------
            player[0].sprite = SPR_addSprite(&spr_subzero, 24, 96, TILE_ATTR(PAL2, 0, FALSE, FALSE));
            PAL_setPalette(PAL2, spr_subzero.palette->data, DMA);

            player[1].sprite = SPR_addSprite(&spr_reptile, 168, 96, TILE_ATTR(PAL3, 0, FALSE, TRUE));
            PAL_setPalette(PAL3, spr_reptile.palette->data, DMA);

            player[0].id = SUBZERO;
            player[1].id = REPTILE;
            player[0].state = PARADO;
            player[1].state = PARADO;
            player[0].paleta = PAL2;
            player[1].paleta = PAL3;
        }

        // ---- faz as nuvens rolarem -----
        scrollOffset += 1;

        for (int i = 0; i < 48; i++)
        {
            scrollValues[i] = scrollOffset;
        }

        VDP_setHorizontalScrollLine(BG_B, 0, scrollValues, 48, CPU);
        // -----

        SPR_update();
        SYS_doVBlankProcess();
    }
}
