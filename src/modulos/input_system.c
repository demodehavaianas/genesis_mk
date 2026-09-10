#include "input_system.h"
#include "game_vars.h"

void inputSystem()
{
    static u16 currentJoyState[2] = {0};
    static u16 previousJoyState[2] = {0};
    u16 buttons[] = {BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_X, BUTTON_Y, BUTTON_Z, BUTTON_START, BUTTON_MODE};
    
    if(gPodeMover) // travar o controle dos jogadores
    for (int jogador = 0; jogador < 2; jogador++)
    {
        previousJoyState[jogador] = currentJoyState[jogador];

        if (jogador == 0)
        {
            currentJoyState[jogador] = JOY_readJoypad(JOY_1);
        }
        else if (jogador == 1)
        {
            currentJoyState[jogador] = JOY_readJoypad(JOY_2);
        }

        int numButtons = sizeof(buttons) / sizeof(buttons[0]);

        for (int i = 0; i < numButtons; i++)
        {
            u16 button = buttons[i];

            if (!(currentJoyState[jogador] & button))
            {
                // não esta apertado
                player[jogador].key_JOY_status[i] = BUTTON_RELEASED;
            }
            else if ((currentJoyState[jogador] & button) && !(previousJoyState[jogador] & button))
            {
                // acabou de apertar
                player[jogador].key_JOY_status[i] = BUTTON_PRESSED;
            }
            else if (currentJoyState[jogador] & button)
            {
                // mantendo apertado
                player[jogador].key_JOY_status[i] = BUTTON_HELD;
            }
            else if (!(currentJoyState[jogador] & button) && (previousJoyState[jogador] & button))
            {
                // acabou de soltar
                player[jogador].key_JOY_status[i] = BUTTON_RELEASED_AFTER_PRESS;
            }

            if (i == numButtons - 1) // end of loop, assignments
            {
                player[jogador].key_JOY_UP_status = player[jogador].key_JOY_status[0];
                player[jogador].key_JOY_DOWN_status = player[jogador].key_JOY_status[1];
                player[jogador].key_JOY_LEFT_status = player[jogador].key_JOY_status[2];
                player[jogador].key_JOY_RIGHT_status = player[jogador].key_JOY_status[3];
                player[jogador].key_JOY_A_status = player[jogador].key_JOY_status[4];
                player[jogador].key_JOY_B_status = player[jogador].key_JOY_status[5];
                player[jogador].key_JOY_C_status = player[jogador].key_JOY_status[6];
                player[jogador].key_JOY_X_status = player[jogador].key_JOY_status[7];
                player[jogador].key_JOY_Y_status = player[jogador].key_JOY_status[8];
                player[jogador].key_JOY_Z_status = player[jogador].key_JOY_status[9];
                player[jogador].key_JOY_START_status = player[jogador].key_JOY_status[10];
                player[jogador].key_JOY_MODE_status = player[jogador].key_JOY_status[11];
            }
        }
    }
}