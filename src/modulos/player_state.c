#include "mkplus.h"

void playerState(u8 ind, u16 state)
{
    if (player[ind].sprite)
    {
        SPR_releaseSprite(player[ind].sprite);
        player[ind].sprite = NULL;
    }

    player[ind].animFrame = 1;
    player[ind].frameTimeAtual = 1;
    player[ind].dataAnim[1] = 1;
    player[ind].animFrameTotal = 1;
    player[ind].state = state;
/*
    if(ind == 0 && player[0].direcao == 1 && player[1].x < player[0].x && state == PARADO)
    {
        state = VIRAR;
    }
    if(ind == 0 && player[0].direcao ==-1 && player[0].x < player[1].x && state == PARADO)
    {
        state = VIRAR;
    }
    if(ind == 1 && player[1].direcao == 1 && player[0].x < player[1].x && state == PARADO)
    {
        state = VIRAR;
    }
    if(ind == 1 && player[1].direcao ==-1 && player[1].x < player[0].x && state == PARADO)
    {
        state = VIRAR;
    }
*/
    player[ind].ops->set_state(ind, state);
    player[ind].ops->play_sound(ind, state);

    SPR_setHFlip(player[ind].sprite, (player[ind].direcao == 1) ? FALSE : TRUE);

    SPR_setAnimAndFrame(player[ind].sprite, 0, player[ind].animFrame - 1);
    player[ind].frameTimeTotal = player[ind].dataAnim[1];

    SPR_setDepth(player[ind].sprite, SPR_MIN_DEPTH);
    //SPR_setAnim(player[ind].sprite, 0);
}