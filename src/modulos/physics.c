#include "mkplus.h"

static void physics_andar(u8 i);
static void physics_pulo_neutro(u8 i);

void physics_update(void)
{
    for (u8 i = 0; i < MAX_PLAYERS; i++)
    {
        physics_andar(i);
        physics_pulo_neutro(i);
    }
}

static void physics_andar(u8 i)
{
    if (player[i].state == ANDAR_PRA_FRENTE ||
        player[i].state == ANDAR_PRA_TRAS)
    {
        player[i].hSpeed = 2;

        if (player[i].state == ANDAR_PRA_FRENTE)
        {
            player[i].x += player[i].hSpeed * player[i].direcao;
        }
        if (player[i].state == ANDAR_PRA_TRAS)
        {
            player[i].x += player[i].hSpeed * (player[i].direcao * -1);
        }
    }
}

static void physics_pulo_neutro(u8 i)
{
    u16 estado = player[i].state;

    if (estado != INI_PULO_NEUTRO)// && estado != FIM_PULO_NEUTRO)
        return;

    player[i].vSpeed += 1;
    player[i].y += player[i].vSpeed >> 2;

    if (player[i].y >= (s16) gAlturaDoPiso)
    {
        player[i].y = (s16) gAlturaDoPiso;
        player[i].vSpeed = 0;
        playerState(i, PULO_NEUTRO_SOLO);
        return;
    }

    if (estado == INI_PULO_NEUTRO && player[i].vSpeed >= 0)
        //playerState(i, FIM_PULO_NEUTRO);
        playerState(i, INI_PULO_NEUTRO);
}