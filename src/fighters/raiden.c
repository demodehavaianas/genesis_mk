#include "fighters.h"
#include "sprites.h"
#include "estruturas.h"
#include "game_vars.h"

/**
 * @brief Define o estado do personagem Raiden.
 * 
 * @param numPlayer o player que está sendo atualizado (0 para o player 1 ou 1 para o player 2)
 * @param State novo estado da animação do personagem (ex: PARADO, ANDANDO, PULANDO, etc.)
 */
void playerState_Raiden(int numPlayer, u16 State)
{
     switch (State)
    {
    case PARADO:
        player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].w = 16 * 8;
        player[numPlayer].h = 15 * 8;
        player[numPlayer].dataAnim[1] =  6;
        player[numPlayer].dataAnim[2] =  6;
        player[numPlayer].dataAnim[3] =  6;
        player[numPlayer].dataAnim[4] =  6;
        player[numPlayer].dataAnim[5] =  6;
        player[numPlayer].dataAnim[6] =  6;
        player[numPlayer].dataAnim[7] =  6;
        player[numPlayer].dataAnim[8] =  6;
        player[numPlayer].dataAnim[9] =  6;
        player[numPlayer].dataAnim[10] =  6;
        player[numPlayer].animFrameTotal = 10;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&spr_raiden, player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD);
        break;

    default:
        break;
    }

    if(player[numPlayer].direcao == 1)
    {
        PAL_setPalette(PAL2, spr_raiden.palette->data, DMA);
    } else {
        PAL_setPalette(PAL3, spr_raiden.palette->data, DMA);
    }
}