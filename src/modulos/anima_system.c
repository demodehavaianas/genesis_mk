#include "mkplus.h"

void animate()
{
    //if(player[0].hitPause == 0 && player[1].hitPause == 0){
    gASG_system = FALSE;

    // jogadores 1 e 2
    for (u8 ind = 0; ind < 2; ind++)
    {
        player[ind].frameTimeAtual++;

        // hora de trocar o frame
        if (player[ind].frameTimeAtual > player[ind].frameTimeTotal && gASG_system == FALSE)
        {
            player[ind].animFrame++;

            //hora de trocar ou recarregar a animacao
            if (player[ind].animFrame > player[ind].animFrameTotal)
            {
                //--------------------- refactory
                if (player[ind].state == BLOQUEIO_EM_PE     ||
                    player[ind].state == BLOQUEIO_ABAIXADO  ||
                    player[ind].state == INI_PULO_NEUTRO    ||
                    player[ind].state == FIM_PULO_NEUTRO)
                {
                    //player[ind].animFrame = 1;
                    player[ind].animFrame = player[ind].animFrameTotal;
                }
                else
                {
                    bool bSegura = (player[ind].key_JOY_B_status == BUTTON_PRESSED ||
                                    player[ind].key_JOY_B_status == BUTTON_HELD);
                    bool downSegura = (player[ind].key_JOY_DOWN_status == BUTTON_PRESSED ||
                                    player[ind].key_JOY_DOWN_status == BUTTON_HELD);

                    gASG_system = TRUE;

                    switch (player[ind].state)
                    {
                        case PARADO:
                        case LEVANTANDO:
                        case VIRAR:
                        case CHUTE_BAIXO:
                        case CHUTE_ALTO:
                        case SOCO_PERTO:
                        case CHUTE_PERTO:
                        case GIRATORIA:
                        case BLOQUEIO_EM_PE_FIM:
                            playerState(ind, PARADO);
                            break;

                        case ANDAR_PRA_FRENTE:
                            playerState(ind, ANDAR_PRA_FRENTE);
                            break;

                        case ANDAR_PRA_TRAS:
                            playerState(ind, ANDAR_PRA_TRAS);
                            break;

                        case ABAIXANDO:
                        case ABAIXADO:
                        case CHUTE_ABAIXADO:
                            playerState(ind, ABAIXADO);
                            break;

                        case BLOQUEIO_EM_PE_INI:
                            playerState(ind, bSegura ? BLOQUEIO_EM_PE : BLOQUEIO_EM_PE_FIM);
                            break;

                        case BLOQUEIO_ABAIXADO_INI:
                            playerState(ind, bSegura ? BLOQUEIO_ABAIXADO : BLOQUEIO_ABAIXADO_FIM);
                            break;

                        case BLOQUEIO_ABAIXADO_FIM:
                            playerState(ind, downSegura ? ABAIXADO : LEVANTANDO);
                            break;
                        
                        case RASTEIRA:
                            if(player[ind].key_JOY_DOWN_status == BUTTON_RELEASED) 
                                playerState(ind, PARADO);
                            else if (player[ind].key_JOY_DOWN_status == BUTTON_HELD) 
                                playerState(ind, ABAIXADO);
                            break;
                        
                        case PULO_NEUTRO_SOLO:
                            player[ind].vSpeed = 0;
                            playerState(ind, PARADO);
                            break;
                    }
                }
                //---------------------
                /*gASG_system = TRUE;

                if(player[ind].state == PARADO)             playerState(ind, PARADO);//
                if(player[ind].state == ANDAR_PRA_FRENTE)   playerState(ind, ANDAR_PRA_FRENTE);//
                if(player[ind].state == ANDAR_PRA_TRAS)     playerState(ind, ANDAR_PRA_TRAS);//
                if(player[ind].state == ABAIXANDO)          playerState(ind, ABAIXADO);//
                if(player[ind].state == ABAIXADO)           playerState(ind, ABAIXADO);//
                if(player[ind].state == LEVANTANDO)         playerState(ind, PARADO);//
                if(player[ind].state == VIRAR)              playerState(ind, PARADO);//
                if(player[ind].state == CHUTE_BAIXO)        playerState(ind, PARADO);//
                if(player[ind].state == CHUTE_ALTO)         playerState(ind, PARADO);//
                if(player[ind].state == SOCO_PERTO)         playerState(ind, PARADO);//
                if(player[ind].state == CHUTE_PERTO)        playerState(ind, PARADO);//
                if(player[ind].state == RASTEIRA && 
                   player[ind].key_JOY_DOWN_status == BUTTON_RELEASED) playerState(ind, PARADO); //
                if(player[ind].state == RASTEIRA && 
                   player[ind].key_JOY_DOWN_status == BUTTON_HELD)     playerState(ind, ABAIXADO); //
                if(player[ind].state == GIRATORIA)          playerState(ind, PARADO);//
                if(player[ind].state == CHUTE_ABAIXADO)     playerState(ind, ABAIXADO);//

                if (player[ind].state == BLOQUEIO_EM_PE_INI)
                {
                    if (player[ind].key_JOY_B_status == BUTTON_PRESSED ||
                        player[ind].key_JOY_B_status == BUTTON_HELD)
                        playerState(ind, BLOQUEIO_EM_PE); // ainda segurando 
                    else
                        playerState(ind, BLOQUEIO_EM_PE_FIM); // soltou durante os 2 quadros 
                }
                else if (player[ind].state == BLOQUEIO_EM_PE)
                {
                    player[ind].animFrame = 1; // 1 quadro só: segura, não recria o sprite 
                }
                else if (player[ind].state == BLOQUEIO_EM_PE_FIM)
                    playerState(ind, PARADO);
                //--
                if (player[ind].state == BLOQUEIO_ABAIXADO_INI)
                {
                    if (player[ind].key_JOY_B_status == BUTTON_PRESSED ||
                        player[ind].key_JOY_B_status == BUTTON_HELD)
                        playerState(ind, BLOQUEIO_ABAIXADO);
                    else
                        playerState(ind, BLOQUEIO_ABAIXADO_FIM);
                }
                else if (player[ind].state == BLOQUEIO_ABAIXADO)
                {
                    player[ind].animFrame = 1; // pose de 1 quadro: não recria o sprite
                }
                else if (player[ind].state == BLOQUEIO_ABAIXADO_FIM)
                {
                    if (player[ind].key_JOY_DOWN_status == BUTTON_PRESSED ||
                        player[ind].key_JOY_DOWN_status == BUTTON_HELD)
                        playerState(ind, ABAIXADO);
                    else
                        playerState(ind, LEVANTANDO);
                }*/
            }

            player[ind].frameTimeAtual = 1;
            player[ind].frameTimeTotal = player[ind].dataAnim[player[ind].animFrame];
            
            SPR_setAnimAndFrame(player[ind].sprite, 0, player[ind].animFrame-1);
    
            //FUNCAO_FSM_HITBOXES(i); //Atualiza as Hurt / Hitboxes
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
