#include "mkplus.h"

typedef void (*PlayerStateFunc)(int, u16);

/*
 * Tabela indexada pelo enum Fighters. Goro/Shang ficam NULL até existirem.
 * Inicializadores: se alguém inserir um lutador no meio do enum,
 * o slot certo continua apontando pro handler certo.
 */
static const PlayerStateFunc PLAYER_STATE_FUNCS[FIGHTERS_COUNT] = {
    [JOHNNY_CAGE]   = playerState_Johnny,
    [KANO]          = playerState_Kano,
    [RAIDEN]        = playerState_Raiden,
    [LIU_KANG]      = playerState_LiuKang,
    [SUBZERO]       = playerState_SubZero,
    [SCORPION]      = playerState_Scorpion,
    [SONYA]         = playerState_Sonya,
    [GORO]          = NULL,
    [SHANG_TSUNG]   = NULL,
    [REPTILE]       = playerState_Reptile
};

void playerState(int ind, u16 state)
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

    if (PLAYER_STATE_FUNCS[player[ind].id])
    {
        PLAYER_STATE_FUNCS[player[ind].id](ind, state);
    }

    SPR_setHFlip(player[ind].sprite, (player[ind].direcao == 1) ? FALSE : TRUE);

    SPR_setDepth(player[ind].sprite, SPR_MIN_DEPTH);

    SPR_setAnimAndFrame(player[ind].sprite, 0, player[ind].animFrame - 1);
    player[ind].frameTimeTotal = player[ind].dataAnim[1];
    //SPR_setAnim(player[ind].sprite, 0);
}

void anima()
{
    gASG_system = FALSE;

    // jogadores 1 e 2
    for (int ind = 0; ind < 2; ind++)
    {
        player[ind].frameTimeAtual++;

        if (player[ind].frameTimeAtual > player[ind].frameTimeTotal && gASG_system == FALSE)
        {
            player[ind].animFrame++;

            if (player[ind].animFrame > player[ind].animFrameTotal)
            {
                gASG_system = TRUE;

                if (player[ind].state == PARADO)
                {
                    playerState(ind, PARADO);
                }
            }
            player[ind].frameTimeAtual = 1;
            player[ind].frameTimeTotal = player[ind].dataAnim[player[ind].animFrame];

            if (player[ind].sprite)
            {
                u16 totalReal = player[ind].sprite->animation->numFrame;

                if (player[ind].animFrame - 1 < totalReal)
                {
                    SPR_setAnimAndFrame(player[ind].sprite, 0, player[ind].animFrame - 1);
                }
            }
        }
        else if (player[ind].frameTimeAtual > player[ind].frameTimeTotal && gASG_system == TRUE)
        {
            if (player[ind].frameTimeAtual > 1)
            {
                player[ind].frameTimeAtual--;
            }
        }
    }
}
