#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <genesis.h>
#include "estruturas.h"

enum ButtonStatus
{
    BUTTON_RELEASED = 0, // Botão não está pressionado
    BUTTON_PRESSED = 1,  // Botão acabou de ser pressionado
    BUTTON_HELD = 2,     // Botão está sendo mantido pressionado
    BUTTON_RELEASED_AFTER_PRESS = 3 // Botão acabou de ser solto após ter sido pressionado
};

/**
 * @brief Gerencia a entrada dos jogadores, lendo o estado atual e anterior dos botões de controle
 * para dois jogadores. Ela atualiza o status dos botões, indicando se estão pressionados,
 * segurados ou soltos, e armazena essas informações em uma estrutura associada a cada jogador.
 */
void inputSystem();

#endif