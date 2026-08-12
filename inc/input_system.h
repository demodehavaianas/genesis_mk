#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <genesis.h>
#include "estruturas.h"

/**
 * @brief Gerencia a entrada dos jogadores, lendo o estado atual e anterior dos botões de controle
 * para dois jogadores. Ela atualiza o status dos botões, indicando se estão pressionados,
 * segurados ou soltos, e armazena essas informações em uma estrutura associada a cada jogador.
 */
void inputSystem();

#endif