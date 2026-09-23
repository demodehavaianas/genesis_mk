#ifndef ANIMA_SYSTEM_H
#define ANIMA_SYSTEM_H

#include <genesis.h>

void anima();

/**
 * @brief atualiza o estado de um jogador com base no índice fornecido e no novo estado,
 *        liberando o sprite atual, configurando os parâmetros de animação e 
 *        chamando uma função específica do estado do jogador, se disponível. 
 *        Além disso, a função ajusta a orientação e a profundidade do sprite, 
 *        garantindo que a animação correta seja exibida.
 *
 * @param ind Índice do jogador
 * @param state Novo estado do jogador estruturas.h "PLAYER_STATUS"
 */
void playerState(int ind, u16 state);

#endif // ANIMA_SYSTEM_H