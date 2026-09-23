#include "mkplus.h"

#include "sp_kano.h"

/**
 * @brief Define o estado do personagem Kano.
 *
 * @param numPlayer o player que está sendo atualizado (0 para o player 1 ou 1 para o player 2)
 * @param State novo estado da animação do personagem (ex: PARADO, ANDANDO, PULANDO, etc.)
 */
void playerState_Kano(int numPlayer, u16 State)
{
    switch (State)
    {
    case PARADO:
        player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].w = 64;      // 8*8
        player[numPlayer].h = 120;     // 15 * 8
        player[numPlayer].axisX = 32;  // 64/2
        player[numPlayer].axisY = 120; // mesmo que h
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].dataAnim[3] = 5;
        player[numPlayer].dataAnim[4] = 5;
        player[numPlayer].dataAnim[5] = 5;
        player[numPlayer].dataAnim[6] = 5;
        player[numPlayer].dataAnim[7] = 5;
        player[numPlayer].animFrameTotal = 7;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_kano_parado, player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case VITORIA:
        player[numPlayer].w = 96;      // 12*8
        player[numPlayer].h = 144;     // 18*8
        player[numPlayer].axisX = 48;  // 96/2
        player[numPlayer].axisY = 144; // mesmo que h
        player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].dataAnim[5] = 6;
        player[numPlayer].animFrameTotal = 5;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_kano_wins,
                                                       player[numPlayer].x - player[numPlayer].axisX, // 288-48
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        // para pose de vitória não repetir a animação
        SPR_setAnimationLoop(player[numPlayer].sprite, FALSE);
        break;
    default:
        break;
    }

    if (player[numPlayer].direcao == 1)
    {
        PAL_setPalette(PAL2, pal_kano_p1.data, DMA);
    }
    else
    {
        PAL_setPalette(PAL3, pal_kano_p1.data, DMA);
    }
}
