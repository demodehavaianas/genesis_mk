#include "mkplus.h"

#define SOCO_TIMER 1

/* Passos 1..5 do soco curto desenham 0,1,2,1,0. O sprite só tem esses 3 quadros. */
static u8 soco_baixo_frame(u16 step)
{
    static const u8 seq[5] = { 0, 1, 2, 1, 0 };

    if (step < 1 || step > 5)
        return 0;
    return seq[step - 1];
}

void animate()
{
    //if(player[0].hitPause == 0 && player[1].hitPause == 0){
    gASG_system = FALSE;

    // jogadores 1 e 2
    for (u8 i = 0; i < MAX_PLAYERS; i++)
    {
        player[i].frameTimeAtual++;

        // hora de trocar o frame
        if (player[i].frameTimeAtual > player[i].frameTimeTotal && gASG_system == FALSE)
        {
            player[i].animFrame++;

            //hora de trocar ou recarregar a animacao
            if (player[i].animFrame > player[i].animFrameTotal)
            {
                if (player[i].state == SOCO_BAIXO_CONTINUO)
                {
                    /* 0..7 acabou. Sem A novo, para. Com A, volta no quadro 1 (não no 0). */
                    if (player[i].key_JOY_countdown[SOCO_TIMER] == 0)
                    {
                        gASG_system = TRUE;
                        playerState(i, PARADO);
                    }
                    else
                        player[i].animFrame = 2;
                }
                else
                //--------------------- refactory
                if (player[i].state == BLOQUEIO_EM_PE     ||
                    player[i].state == BLOQUEIO_ABAIXADO  ||
                    player[i].state == INI_PULO_NEUTRO    
                   // || player[i].state == FIM_PULO_NEUTRO
                )
                {
                    //player[i].animFrame = 1;
                    player[i].animFrame = player[i].animFrameTotal;
                }
                else
                {
                    bool bSegura = (player[i].key_JOY_B_status == BUTTON_PRESSED ||
                                    player[i].key_JOY_B_status == BUTTON_HELD);
                    bool downSegura = (player[i].key_JOY_DOWN_status == BUTTON_PRESSED ||
                                    player[i].key_JOY_DOWN_status == BUTTON_HELD);

                    gASG_system = TRUE;

                    switch (player[i].state)
                    {
                        case PARADO:
                        case LEVANTANDO:
                        case VIRAR:
                        case SOCO_ALTO:
                        case SOCO_BAIXO:
                        case CHUTE_ALTO:
                        case CHUTE_BAIXO:
                        case SOCO_PERTO:
                        case CHUTE_PERTO:
                        case GIRATORIA:
                        case BLOQUEIO_EM_PE_FIM:
                            playerState(i, PARADO);
                            break;

                        case ANDAR_PRA_FRENTE:
                            playerState(i, ANDAR_PRA_FRENTE);
                            break;

                        case ANDAR_PRA_TRAS:
                            playerState(i, ANDAR_PRA_TRAS);
                            break;

                        case ABAIXANDO:
                        case ABAIXADO:
                        case CHUTE_ABAIXADO:
                            playerState(i, ABAIXADO);
                            break;

                        case BLOQUEIO_EM_PE_INI:
                            playerState(i, bSegura ? BLOQUEIO_EM_PE : BLOQUEIO_EM_PE_FIM);
                            break;

                        case BLOQUEIO_ABAIXADO_INI:
                            playerState(i, bSegura ? BLOQUEIO_ABAIXADO : BLOQUEIO_ABAIXADO_FIM);
                            break;

                        case BLOQUEIO_ABAIXADO_FIM:
                            playerState(i, downSegura ? ABAIXADO : LEVANTANDO);
                            break;
                        
                        case RASTEIRA:
                            if(player[i].key_JOY_DOWN_status == BUTTON_RELEASED) 
                                playerState(i, PARADO);
                            else if (player[i].key_JOY_DOWN_status == BUTTON_HELD) 
                                playerState(i, ABAIXADO);
                            break;
                        
                        case PULO_NEUTRO_SOLO:
                            player[i].vSpeed = 0;
                            playerState(i, PARADO);
                            break;
                    }
                }
                //---------------------
                /*gASG_system = TRUE;

                if(player[i].state == PARADO)             playerState(i, PARADO);//
                if(player[i].state == ANDAR_PRA_FRENTE)   playerState(i, ANDAR_PRA_FRENTE);//
                if(player[i].state == ANDAR_PRA_TRAS)     playerState(i, ANDAR_PRA_TRAS);//
                if(player[i].state == ABAIXANDO)          playerState(i, ABAIXADO);//
                if(player[i].state == ABAIXADO)           playerState(i, ABAIXADO);//
                if(player[i].state == LEVANTANDO)         playerState(i, PARADO);//
                if(player[i].state == VIRAR)              playerState(i, PARADO);//
                if(player[i].state == CHUTE_BAIXO)        playerState(i, PARADO);//
                if(player[i].state == CHUTE_ALTO)         playerState(i, PARADO);//
                if(player[i].state == SOCO_PERTO)         playerState(i, PARADO);//
                if(player[i].state == CHUTE_PERTO)        playerState(i, PARADO);//
                if(player[i].state == RASTEIRA && 
                   player[i].key_JOY_DOWN_status == BUTTON_RELEASED) playerState(i, PARADO); //
                if(player[i].state == RASTEIRA && 
                   player[i].key_JOY_DOWN_status == BUTTON_HELD)     playerState(i, ABAIXADO); //
                if(player[i].state == GIRATORIA)          playerState(i, PARADO);//
                if(player[i].state == CHUTE_ABAIXADO)     playerState(i, ABAIXADO);//

                if (player[i].state == BLOQUEIO_EM_PE_INI)
                {
                    if (player[i].key_JOY_B_status == BUTTON_PRESSED ||
                        player[i].key_JOY_B_status == BUTTON_HELD)
                        playerState(i, BLOQUEIO_EM_PE); // ainda segurando 
                    else
                        playerState(i, BLOQUEIO_EM_PE_FIM); // soltou durante os 2 quadros 
                }
                else if (player[i].state == BLOQUEIO_EM_PE)
                {
                    player[i].animFrame = 1; // 1 quadro só: segura, não recria o sprite 
                }
                else if (player[i].state == BLOQUEIO_EM_PE_FIM)
                    playerState(i, PARADO);
                //--
                if (player[i].state == BLOQUEIO_ABAIXADO_INI)
                {
                    if (player[i].key_JOY_B_status == BUTTON_PRESSED ||
                        player[i].key_JOY_B_status == BUTTON_HELD)
                        playerState(i, BLOQUEIO_ABAIXADO);
                    else
                        playerState(i, BLOQUEIO_ABAIXADO_FIM);
                }
                else if (player[i].state == BLOQUEIO_ABAIXADO)
                {
                    player[i].animFrame = 1; // pose de 1 quadro: não recria o sprite
                }
                else if (player[i].state == BLOQUEIO_ABAIXADO_FIM)
                {
                    if (player[i].key_JOY_DOWN_status == BUTTON_PRESSED ||
                        player[i].key_JOY_DOWN_status == BUTTON_HELD)
                        playerState(i, ABAIXADO);
                    else
                        playerState(i, LEVANTANDO);
                }*/
            }

            player[i].frameTimeAtual = 1;
            player[i].frameTimeTotal = player[i].dataAnim[player[i].animFrame];

            {
                u16 frame = (u16)(player[i].animFrame - 1);
                if (player[i].state == SOCO_BAIXO)
                    frame = soco_baixo_frame(player[i].animFrame);
                SPR_setAnimAndFrame(player[i].sprite, 0, frame);
            }
            
            //SPR_setAnimAndFrame(player[i].sprite, 0, player[i].animFrame-1);

            //FUNCAO_FSM_HITBOXES(i); //Atualiza as Hurt / Hitboxes
        }
        else if (player[i].frameTimeAtual > player[i].frameTimeTotal && gASG_system == TRUE)
        {
            if (player[i].frameTimeAtual > 1)
            {
                player[i].frameTimeAtual--;
            }
        }
    }
}
