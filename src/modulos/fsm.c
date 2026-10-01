#include "mkplus.h"

static void fsm_update_andando(u8 i);
static void fsm_update_tras_e_chute(u8 i);
static void fsm_update_abaixando_levantando(u8 i);
static void fsm_update_trocando_de_lado(u8 atacante, u8 defensor, u8 i);
static void fsm_update_defesa(u8 defensor, u8 distancia_player_x_special, u8 atacante, u8 i);
static void fsm_update_bloqueio(u8 i);
static void fsm_update_pulo_neutro(u8 i);
static void fsm_update_golpe_perto(u8 i);;

void fsm_update(void)
{
    if (player[0].direcao == 1)
        gDistancia = player[1].x - player[0].x;
    else
        gDistancia = player[0].x - player[1].x;

    for (u8 i = 0; i < MAX_PLAYERS; i++)
    {
        u8 atacante;
        u8 defensor;
        u8 distancia_player_x_special = 250u;

        if (i == 0) { atacante = 0; defensor = 1; }
        else { atacante = 1; defensor = 0; }

        if (gDistancia > 50)
        {
            if (player[i].key_JOY_C_status == BUTTON_PRESSED &&
                (player[i].state == PARADO ||
                 player[i].state == ANDAR_PRA_FRENTE))
            {
                playerState(i, CHUTE_BAIXO);
            }
            if (player[i].key_JOY_Z_status == BUTTON_PRESSED &&
                (player[i].state == PARADO ||
                 player[i].state == ANDAR_PRA_FRENTE))
            {
                playerState(i, CHUTE_ALTO);
            }

            if(player[i].key_JOY_X_status == BUTTON_PRESSED && 
                (player[i].state == PARADO ||
                 player[i].state == ANDAR_PRA_FRENTE ||
                 player[i].state == ANDAR_PRA_TRAS))
            {
                playerState(i, SOCO_ALTO);
            }

            if(player[i].key_JOY_A_status == BUTTON_PRESSED && 
                (player[i].state == PARADO ||
                 player[i].state == ANDAR_PRA_FRENTE ||
                 player[i].state == ANDAR_PRA_TRAS))
            {
                playerState(i, SOCO_BAIXO);
            }

        }

        fsm_update_golpe_perto(i);

        fsm_update_pulo_neutro(i);

        fsm_update_andando(i);

        fsm_update_tras_e_chute(i);

        fsm_update_bloqueio(i);

        fsm_update_abaixando_levantando(i);

        if(player[i].state == ABAIXADO && (player[i].key_JOY_Z_status == BUTTON_PRESSED || player[i].key_JOY_C_status == BUTTON_PRESSED))
        {
            playerState(i, CHUTE_ABAIXADO);
        }

        fsm_update_trocando_de_lado(atacante, defensor, i);

        // defender magia
        /*if (player[defensor].especial.pos.x != 0 &&
            player[defensor].especial.pos.y != 0)
        {
            if (player[atacante].direcao == 1)
            {
                distancia_player_x_special = player[defensor].especial.pos.x - player[atacante].especial.pos.x + 25;
            }
            if (player[atacante].direcao == -1)
            {
                distancia_player_x_special = player[atacante].especial.pos.x - player[defensor].especial.pos.x;
            }
        }*/

        // fsm_update_defesa(defensor, distancia_player_x_special, atacante, i);

        // comentando por estar dando bug ao alterar animação
        // key_JOY_countdown é um timer regressivo,
        // ativado quando se aperta algum botao direcional,
        // usado para Corrida e Esquiva, entre outros...
        if (player[i].key_JOY_countdown[8] > 0)
        {
            player[i].key_JOY_countdown[8]--;
        }
        if (player[i].key_JOY_countdown[2] > 0)
        {
            player[i].key_JOY_countdown[2]--;
        }
        if (player[i].key_JOY_countdown[4] > 0)
        {
            player[i].key_JOY_countdown[4]--;
        }
        if (player[i].key_JOY_countdown[6] > 0)
        {
            player[i].key_JOY_countdown[6]--;
        }
/*
        // ativacao de 'key_JOY_countdown'...
        if (player[i].state == ANDAR_PRA_TRAS ||
            player[i].state == ANDAR_PRA_FRENTE ||
            player[i].state == ABAIXANDO ||
            player[i].state == INI_PULO_TRAS ||
            player[i].state == INI_PULO_NEUTRO ||
            player[i].state == INI_PULO_FRENTE)
        {
            if (player[i].key_JOY_UP_status == 1)
            {
                player[i].key_JOY_countdown[8] = 12;
                player[i].key_JOY_countdown[2] = 0;
            }
            if (player[i].key_JOY_LEFT_status == 1)
            {
                player[i].key_JOY_countdown[4] = 12;
                player[i].key_JOY_countdown[6] = 0;
            }
            if (player[i].key_JOY_RIGHT_status == 1)
            {
                player[i].key_JOY_countdown[6] = 12;
                player[i].key_JOY_countdown[4] = 0;
            }
            if (player[i].key_JOY_DOWN_status == 1)
            {
                player[i].key_JOY_countdown[2] = 12;
                player[i].key_JOY_countdown[8] = 0;
                player[i].key_JOY_countdown[4] = 0;
                player[i].key_JOY_countdown[6] = 0;
            }
        }
    */
    }
}

static void fsm_update_golpe_perto(u8 i)
{
    if (gDistancia < 50)
    {
        if ((player[i].key_JOY_Z_status == BUTTON_PRESSED || player[i].key_JOY_C_status == BUTTON_PRESSED) &&
            (player[i].state == PARADO ||
             player[i].state == ANDAR_PRA_FRENTE))
        {
            playerState(i, CHUTE_PERTO);
        }
        if (player[i].key_JOY_X_status == BUTTON_PRESSED &&
            (player[i].state == PARADO ||
             player[i].state == ANDAR_PRA_FRENTE))
        {
            playerState(i, SOCO_PERTO);
        }
    }
}

static void fsm_update_tras_e_chute(u8 i)
{
    if (player[i].state == ANDAR_PRA_TRAS || player[i].state == PARADO || player[i].state == ABAIXADO)
    {
        // executa a rasteira
        if (player[i].key_JOY_C_status == BUTTON_PRESSED && 
            ((player[i].key_JOY_LEFT_status == BUTTON_HELD && player[i].direcao == 1) || 
             (player[i].key_JOY_RIGHT_status == BUTTON_HELD && player[i].direcao == -1)))
        {
            playerState(i, RASTEIRA);
        }
  
        //TODO: retirar o chute giratório quando estiver abaixado, executar apenas quando estiver em pé.
        // executa o chute giratório
        if(player[i].key_JOY_Z_status == BUTTON_PRESSED && 
            ((player[i].key_JOY_LEFT_status == BUTTON_HELD && player[i].direcao == 1) ||
             (player[i].key_JOY_RIGHT_status == BUTTON_HELD && player[i].direcao == -1)))
        {
            playerState(i, GIRATORIA);
        }
    }
}

static void fsm_update_trocando_de_lado(u8 atacante, u8 defensor, u8 i)
{
    if (player[atacante].direcao == 1 &&
        player[defensor].x < player[atacante].x &&
        player[atacante].state == PARADO)
    {
        playerState(i, VIRAR);
        player[atacante].direcao = -1;
    }

    if (player[atacante].direcao == -1 &&
        player[atacante].x < player[defensor].x &&
        player[atacante].state == PARADO)
    {
        playerState(i, VIRAR);
        player[atacante].direcao = 1;
    }
}

static void fsm_update_bloqueio(u8 i)
{
    u8 bStatus = player[i].key_JOY_B_status;
    u8 downStatus = player[i].key_JOY_DOWN_status;
    u16 state = player[i].state;
    bool bSegurado = (bStatus == BUTTON_PRESSED || bStatus == BUTTON_HELD);
    bool downSegurado = (downStatus == BUTTON_PRESSED || downStatus == BUTTON_HELD);
    bool neutro = (state == PARADO || state == ANDAR_PRA_FRENTE || state == ANDAR_PRA_TRAS);

    // já está bloqueando em pé e apertou para baixo -> abaixa bloqueando 
    if (bSegurado && downSegurado &&
        (state == BLOQUEIO_EM_PE_INI || state == BLOQUEIO_EM_PE))
    {
        playerState(i, BLOQUEIO_ABAIXADO_INI);
        return;
    }

    // já está bloqueando abaixado e soltou o baixo -> levanta bloqueando
    if (bSegurado && !downSegurado &&
        (state == BLOQUEIO_ABAIXADO_INI || state == BLOQUEIO_ABAIXADO))
    {
        playerState(i, BLOQUEIO_EM_PE_INI);
        return;
    }

    if (bSegurado && downSegurado &&
        (neutro || state == ABAIXANDO || state == ABAIXADO))
    {
        playerState(i, BLOQUEIO_ABAIXADO_INI);
        return;
    }

    if (bSegurado && !downSegurado && neutro)
    {
        playerState(i, BLOQUEIO_EM_PE_INI);
        return;
    }

    if (state == BLOQUEIO_EM_PE && !bSegurado)
        playerState(i, BLOQUEIO_EM_PE_FIM);
    else if (state == BLOQUEIO_ABAIXADO && !bSegurado)
        playerState(i, BLOQUEIO_ABAIXADO_FIM);
}

static void fsm_update_pulo_neutro(u8 i)
{
    bool cima = player[i].key_JOY_UP_status == BUTTON_PRESSED;
    bool dir_horizontal = ( player[i].key_JOY_LEFT_status > BUTTON_RELEASED ||
                            player[i].key_JOY_RIGHT_status > BUTTON_RELEASED);
    u16 estado = player[i].state;

    if (!cima || dir_horizontal)
        return;

    if (estado == PARADO || estado == ANDAR_PRA_FRENTE || estado == ANDAR_PRA_TRAS)
    {
        player[i].vSpeed = -27;
        playerState(i, INI_PULO_NEUTRO);
    }
}

static void fsm_update_defesa(u8 defensor, u8 distancia_player_x_special, u8 atacante, u8 i)
{
    // INICIO DEFESA EM Pé e ABAIXADO
    if ((player[defensor].state == SOCO_ALTO ||
         player[defensor].state == SOCO_ALTO_CONTINUO ||
         player[defensor].state == SOCO_PERTO ||
         player[defensor].state == SOCO_BAIXO ||
         player[defensor].state == SOCO_BAIXO_CONTINUO ||
         player[defensor].state == CHUTE_ALTO ||
         player[defensor].state == CHUTE_BAIXO ||
         player[defensor].state == CHUTE_PERTO ||
         player[defensor].state == CHUTE_ABAIXADO ||
         player[defensor].state == GANCHO ||
         player[defensor].state == VOADORA ||
         player[defensor].state == ESPECIAL_1 ||
         player[defensor].state == ESPECIAL_2 ||
         distancia_player_x_special <= 70) &&
        ((
             (player[atacante].direcao == 1 && player[atacante].key_JOY_LEFT_status == BUTTON_HELD) ||
             (player[atacante].direcao == -1 && player[atacante].key_JOY_RIGHT_status == BUTTON_HELD)) &&
         (player[atacante].state == PARADO ||
          player[atacante].state == ABAIXADO ||
          player[atacante].state == ANDAR_PRA_FRENTE ||
          player[atacante].state == ANDAR_PRA_TRAS)))
    {
        player[i].key_JOY_DOWN_status == BUTTON_HELD ? playerState(i, BLOQUEIO_ABAIXADO_INI) : playerState(i, BLOQUEIO_EM_PE_INI);
    }

    // soltar a defesa em pé
    if ((player[i].state == BLOQUEIO_EM_PE_INI || player[i].state == BLOQUEIO_EM_PE) &&
        ((player[atacante].direcao == 1 && player[atacante].key_JOY_LEFT_status == BUTTON_RELEASED) ||
         (player[atacante].direcao == -1 && player[atacante].key_JOY_RIGHT_status == BUTTON_RELEASED)))
    {
        playerState(i, BLOQUEIO_ABAIXADO_FIM);
    }

    // defesa se encerrou com o final do ataque - EM PE
    if (player[i].state == BLOQUEIO_EM_PE &&
        (player[defensor].state == PARADO ||
         player[defensor].state == ABAIXADO ||
         player[defensor].state == ANDAR_PRA_FRENTE ||
         player[defensor].state == ANDAR_PRA_TRAS))
    {
        playerState(i, BLOQUEIO_EM_PE_FIM);
    }

    // soltou a defesa - ABAIXADO
    if ((player[i].state == BLOQUEIO_ABAIXADO_INI || player[i].state == BLOQUEIO_ABAIXADO) &&
        ((player[atacante].direcao == 1 && player[atacante].key_JOY_LEFT_status == BUTTON_RELEASED) ||
         (player[atacante].direcao == -1 && player[atacante].key_JOY_RIGHT_status == BUTTON_RELEASED)))
    {
        playerState(i, BLOQUEIO_ABAIXADO_FIM);
    }

    // defesa se encerrou com o final do ataque - ABAIXADO
    if (player[i].state == BLOQUEIO_ABAIXADO &&
        (player[defensor].state == PARADO ||
         player[defensor].state == ABAIXADO ||
         player[defensor].state == ANDAR_PRA_FRENTE ||
         player[defensor].state == ANDAR_PRA_TRAS))
    {
        playerState(i, BLOQUEIO_ABAIXADO_FIM);
    }

    // defesa em pe e abaixado
    if (player[i].state == BLOQUEIO_EM_PE && player[i].key_JOY_DOWN_status == BUTTON_HELD)
    {
        playerState(i, BLOQUEIO_ABAIXADO);
    }
    if (player[i].state == BLOQUEIO_ABAIXADO && player[i].key_JOY_DOWN_status == BUTTON_RELEASED)
    {
        playerState(i, BLOQUEIO_EM_PE);
    }
}

static void fsm_update_abaixando_levantando(u8 i)
{
    // abaixando / levantando
    if ((player[i].key_JOY_DOWN_status == BUTTON_PRESSED || player[i].key_JOY_DOWN_status == BUTTON_HELD) &&
        (player[i].state == PARADO ||
         player[i].state == ANDAR_PRA_FRENTE ||
         player[i].state == ANDAR_PRA_TRAS))
    {
        if (player[i].key_JOY_countdown[2] == 0)
        {
            playerState(i, ABAIXANDO);
        }
    }

    if (player[i].key_JOY_DOWN_status == BUTTON_RELEASED && player[i].state == ABAIXADO)
    {
        playerState(i, LEVANTANDO);
    }
}

static void fsm_update_andando(u8 i)
{
    // ANDANDO --
    if (player[i].direcao == 1)
    {
        if (player[i].key_JOY_LEFT_status > BUTTON_RELEASED && player[i].state == PARADO)
        {
            if (player[i].key_JOY_countdown[4] == 0)
                playerState(i, ANDAR_PRA_TRAS);
        }
        if (player[i].key_JOY_RIGHT_status > BUTTON_RELEASED && player[i].state == PARADO)
        {
            if (player[i].key_JOY_countdown[6] == 0)
                playerState(i, ANDAR_PRA_FRENTE);
            //else if(player[i].key_JOY_Y_status == BUTTON_PRESSED)
            //    playerState(i, CORRER);
        }
        if ((player[i].key_JOY_LEFT_status == BUTTON_RELEASED && player[i].state == ANDAR_PRA_TRAS) ||
            (player[i].key_JOY_RIGHT_status == BUTTON_RELEASED && player[i].state == ANDAR_PRA_FRENTE) 
            //|| (player[i].key_JOY_Y_status == BUTTON_RELEASED && player[i].state == ANDAR_PRA_FRENTE)
            )
        {
            playerState(i, PARADO);
        }
    }
    else
    {
        if (player[i].key_JOY_LEFT_status > BUTTON_RELEASED && player[i].state == PARADO)
        {
            if (player[i].key_JOY_countdown[4] == 0)
                playerState(i, ANDAR_PRA_FRENTE);
        }
        if (player[i].key_JOY_RIGHT_status > BUTTON_RELEASED && player[i].state == PARADO)
        {
            if (player[i].key_JOY_countdown[6] == 0)
                playerState(i, ANDAR_PRA_TRAS);
        }
        if ((player[i].key_JOY_LEFT_status == BUTTON_RELEASED && player[i].state == ANDAR_PRA_FRENTE) ||
            (player[i].key_JOY_RIGHT_status == BUTTON_RELEASED && player[i].state == ANDAR_PRA_TRAS))
        {
            player[i].hSpeed = 0;
            playerState(i, PARADO);
        }
    }
}