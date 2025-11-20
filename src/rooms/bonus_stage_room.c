#include <genesis.h>
#include "bonus_stage_room.h"
#include "stages.h"
#include "gfx.h"
#include "sprites.h"
#include "game_vars.h"
#include "typewriter_printer.h"
#include "bio_textlines.h"
#include "lifebar.h"
#include "input_system.h"

#define BLOCO_P1_POS_X 16
#define BLOCO_P2_POS_X 168
#define BLOCO_POS_Y 128
#define BLOCO_ANIM_INTEGRO 0
#define BLOCO_ANIM_QUEBRADO 1
#define POS_X_P1 24
#define POS_X_P2 176
#define POS_Y_P 56//64 //56

void processBonusStage()
{
    VDP_setWindowFullScreen();
    u16 life = 100;

    for (int i = 0; i < 16; i++)
    {
        GE[i].sprite = NULL;
    }
    while (TRUE)
    {
        inputSystem();
        gFrames++;

        if (gFrames == 1)
        {
            // BG_A
            VDP_loadTileSet(tym_bga.tileset, gInd_tileset, DMA);
            VDP_setTileMapEx(WINDOW, tym_bga.tilemap,
                             TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA);
            PAL_setPalette(PAL0, tym_bga.palette->data, DMA);
            gInd_tileset += tym_bga.tileset->numTile;

            // BG_B com tiles com prioridades setados no editor
            VDP_loadTileSet(tym_bgb.tileset, gInd_tileset, DMA);
            VDP_setTileMapEx(BG_B, tym_bgb.tilemap,
                             TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA);
            PAL_setPalette(PAL1, tym_bgb.palette->data, DMA);
            gInd_tileset += tym_bgb.tileset->numTile;

            // Sprite do P1
            // GE[0].sprite = SPR_addSpriteSafe(&spKanoBonus, POS_X_P1, POS_Y_P, (TILE_ATTR(PAL2, FALSE, FALSE, FALSE)));
            // PAL_setPalette(PAL2, spKanoBonus.palette->data, DMA);
            // SPR_setAnim(GE[0].sprite, 0);

            // Sprite do P2
            // GE[1].sprite = SPR_addSpriteSafe(&spKanoBonus, POS_X_P2, POS_Y_P, (TILE_ATTR(PAL2, FALSE, FALSE, FALSE)));
            // PAL_setPalette(PAL3, spKanoBonus.palette->data, DMA);
            // SPR_setAnim(GE[1].sprite, 0);

            // Add Sprite do bloco do P1
            GE[2].sprite = SPR_addSpriteSafe(&spWood, BLOCO_P1_POS_X, BLOCO_POS_Y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
            SPR_setDepth(GE[2].sprite, SPR_MIN_DEPTH);

            // Add Sprite do bloco do P2
            GE[3].sprite = SPR_addSpriteSafe(&spWood, BLOCO_P2_POS_X, BLOCO_POS_Y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
            SPR_setDepth(GE[3].sprite, SPR_MIN_DEPTH);

            // Teste da animação do Frame do Bloco destruído
            SPR_setFrame(GE[3].sprite, BLOCO_ANIM_QUEBRADO);

            GE[4].sprite = SPR_addSpriteSafe(&spMessage1, 56, 24, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            SPR_setDepth(GE[4].sprite, SPR_MIN_DEPTH);

            PAL_setPalette(PAL2, lifebar_pal.data, DMA);
            initLifebar();
            drawLifeBar(WINDOW, 1, 1, life, 100, 16);
        }

      
        if(gFrames == 60)
        {
            if(GE[4].sprite)
            {
                SPR_releaseSprite(GE[4].sprite);
                GE[4].sprite = NULL;
            }
        }

        SPR_update();
        SYS_doVBlankProcess();
    }
}