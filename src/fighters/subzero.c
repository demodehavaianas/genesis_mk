#include "fighters.h"
#include "sprites.h"
#include "estruturas.h"
#include "game_vars.h"

/**
 * @brief Define o estado do personagem Sub-Zero.
 * 
 * @param numPlayer o player que está sendo atualizado (0 para o player 1 ou 1 para o player 2)
 * @param State novo estado da animação do personagem (ex: PARADO, ANDANDO, PULANDO, etc.)
 */
void playerState_SubZero(int numPlayer, u16 State)
{
    switch (State)
    {
    case PARADO:
        player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].w = 16 * 8;
        player[numPlayer].h = 15 * 8;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].dataAnim[3] = 5;
        player[numPlayer].dataAnim[4] = 5;
        player[numPlayer].dataAnim[5] = 5;
        player[numPlayer].dataAnim[6] = 5;
        player[numPlayer].dataAnim[7] = 5;
        player[numPlayer].dataAnim[8] = 5;
        player[numPlayer].dataAnim[9] = 5;
        player[numPlayer].dataAnim[10] = 5;
        player[numPlayer].dataAnim[11] = 5;
        player[numPlayer].dataAnim[12] = 5;
        player[numPlayer].animFrameTotal = 12;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&spr_subzero, player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
        break;

    default:
        break;
    }

    if(player[numPlayer].direcao == 1)
    {
        PAL_setPalette(PAL2, spr_subzero.palette->data, DMA);
    } else {
        PAL_setPalette(PAL3, spr_subzero.palette->data, DMA);
    }
}