#include "mkplus.h"

#include "sp_subzero.h"

/**
 * @brief Define o estado do personagem Sub-Zero.
 *
 * @param numPlayer o player que está sendo atualizado (0 para o player 1 ou 1 para o player 2)
 * @param state novo estado da animação do personagem (ex: PARADO, ANDANDO, PULANDO, etc.)
 */
void playerState_SubZero(u8 numPlayer, u16 state)
{
    switch (state)
    {
    case PARADO:
        player[numPlayer].w = 56;      // 7*8
        player[numPlayer].h = 120;     // 15*8
        player[numPlayer].axisX = 28;  // 56/2
        player[numPlayer].axisY = 120; // mesmo que h
        // player[numPlayer].y = gAlturaDoPiso;
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
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_subzero_parado,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case VITORIA:
        player[numPlayer].w = 64;      // 8*8
        player[numPlayer].h = 144;     // 18*8
        player[numPlayer].axisX = 32;  // 64/2
        player[numPlayer].axisY = 144; // mesmo que h
        player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].animFrameTotal = 4;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_subzero_wins,
                                                       player[numPlayer].x - player[numPlayer].axisX, // 288-32
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        // para pose de vitória não repetir a animação
        SPR_setAnimationLoop(player[numPlayer].sprite, FALSE);
        break;
    case ANDAR_PRA_FRENTE:
    case ANDAR_PRA_TRAS:
        player[numPlayer].w = 72;      // 9*8
        player[numPlayer].h = 120;     // 15*8
        player[numPlayer].axisX = 36;  // 72/2
        player[numPlayer].axisY = 120; // mesmo que h
        // player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].dataAnim[3] = 5;
        player[numPlayer].dataAnim[4] = 5;
        player[numPlayer].dataAnim[5] = 5;
        player[numPlayer].dataAnim[6] = 5;
        player[numPlayer].dataAnim[7] = 5;
        player[numPlayer].dataAnim[8] = 5;
        player[numPlayer].dataAnim[9] = 5;
        player[numPlayer].animFrameTotal = 9;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_subzero_andar,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case ABAIXANDO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 104;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 104;
        // player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].dataAnim[1] = 2;
        player[numPlayer].dataAnim[2] = 2;
        player[numPlayer].animFrameTotal = 2;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_subzero_abaixando,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        // SPR_setAnimationLoop(player[numPlayer].sprite, FALSE);
        break;
    case ABAIXADO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 104;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 104;
        // player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_subzero_abaixado,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case LEVANTANDO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 104;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 104;
        // player[numPlayer].y = gAlturaDoPiso;
        player[numPlayer].dataAnim[1] = 2;
        player[numPlayer].dataAnim[2] = 2;
        player[numPlayer].animFrameTotal = 2;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_subzero_levantando,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        // SPR_setAnimationLoop(player[numPlayer].sprite, FALSE);
        break;
    case VIRAR:
        player[numPlayer].w = 64;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].animFrameTotal = 4;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_virar,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case CHUTE_BAIXO:
        player[numPlayer].w = 120;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].dataAnim[5] = 6;
        player[numPlayer].dataAnim[6] = 6;
        player[numPlayer].dataAnim[7] = 6;
        player[numPlayer].animFrameTotal = 7;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_cb,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case CHUTE_ALTO:
        player[numPlayer].w = 120;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 35;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].dataAnim[5] = 6;
        player[numPlayer].dataAnim[6] = 6;
        player[numPlayer].dataAnim[7] = 6;
        player[numPlayer].dataAnim[8] = 6;
        player[numPlayer].dataAnim[9] = 6;
        player[numPlayer].dataAnim[10] = 6;
        player[numPlayer].animFrameTotal = 10;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_ca,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case CHUTE_PERTO:
        player[numPlayer].w = 88;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 35;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].dataAnim[5] = 6;
        player[numPlayer].animFrameTotal = 5;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_ca_prox,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;

    case SOCO_PERTO:
        player[numPlayer].w = 88;
        player[numPlayer].h = 128;
        player[numPlayer].axisX = 35;
        player[numPlayer].axisY = 128;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].dataAnim[5] = 6;
        player[numPlayer].animFrameTotal = 5;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_sa_prox,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
        break;
    case RASTEIRA:
        player[numPlayer].w = 120;
        player[numPlayer].h = 128;
        player[numPlayer].axisX = 40;
        player[numPlayer].axisY = 108; // 128-20px
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].dataAnim[3] = 5;
        player[numPlayer].dataAnim[4] = 5;
        player[numPlayer].dataAnim[5] = 5;
        player[numPlayer].dataAnim[6] = 5;
        //player[numPlayer].dataAnim[7] = 6;
        player[numPlayer].animFrameTotal = 6;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_rasteira,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case GIRATORIA:
        player[numPlayer].w = 120;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 46;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].dataAnim[3] = 5;
        player[numPlayer].dataAnim[4] = 5;
        player[numPlayer].dataAnim[5] = 5;
        player[numPlayer].dataAnim[6] = 5;
        player[numPlayer].dataAnim[7] = 5;
        player[numPlayer].dataAnim[8] = 5;
        player[numPlayer].animFrameTotal = 8;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_roundhouse,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case CHUTE_ABAIXADO:
        player[numPlayer].w = 96;
        player[numPlayer].h = 88;
        player[numPlayer].axisX = 46;
        player[numPlayer].axisY = 88;
        player[numPlayer].dataAnim[1] = 6;
        player[numPlayer].dataAnim[2] = 6;
        player[numPlayer].dataAnim[3] = 6;
        player[numPlayer].dataAnim[4] = 6;
        player[numPlayer].dataAnim[5] = 6;
        player[numPlayer].animFrameTotal = 5;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_c_abaixado,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case BLOQUEIO_EM_PE_INI:
        player[numPlayer].w = 96;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 45;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].animFrameTotal = 2;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_block_ini,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case BLOQUEIO_EM_PE:
        player[numPlayer].w = 96;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 45;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_block,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case BLOQUEIO_EM_PE_FIM:
        player[numPlayer].w = 96;
        player[numPlayer].h = 120;
        player[numPlayer].axisX = 45;
        player[numPlayer].axisY = 120;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].dataAnim[2] = 5;
        player[numPlayer].animFrameTotal = 2;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_block_fim,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case BLOQUEIO_ABAIXADO_INI:
    case BLOQUEIO_ABAIXADO_FIM:
        player[numPlayer].w = 64;
        player[numPlayer].h = 88;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 88;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_block_baixo_ini,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case BLOQUEIO_ABAIXADO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 88;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 88;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_block_baixo,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case INI_PULO_NEUTRO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 88;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 88;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_pulo_n_ini,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case FIM_PULO_NEUTRO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 128;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 128;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_pulo_n_fim,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    case PULO_NEUTRO_SOLO:
        player[numPlayer].w = 64;
        player[numPlayer].h = 112;
        player[numPlayer].axisX = 32;
        player[numPlayer].axisY = 112;
        player[numPlayer].dataAnim[1] = 5;
        player[numPlayer].animFrameTotal = 1;
        player[numPlayer].sprite = SPR_addSpriteExSafe(&sp_sz_pulo_n_pouso,
                                                       player[numPlayer].x - player[numPlayer].axisX,
                                                       player[numPlayer].y - player[numPlayer].axisY,
                                                       TILE_ATTR(player[numPlayer].paleta, FALSE, FALSE, FALSE),
                                                       SPRITE_FLAGS);
    break;
    default:
        break;
    }

    if (player[numPlayer].direcao == 1)
    {
        PAL_setPalette(PAL2, pal_subzero_p1.data, DMA);
    }
    else
    {
        PAL_setPalette(PAL3, pal_subzero_p1.data, DMA);
    }
    // player[numPlayer].state = state;
}

void subzero_sfx(u8 numPlayer, u16 state)
{
}

void subzero_update_physics(u8 i)
{
}

const FighterOps subzeroOps = {
    .id = SUBZERO,
    .set_state = playerState_SubZero,
    .play_sound = subzero_sfx,
    .update_physics = subzero_update_physics,
};