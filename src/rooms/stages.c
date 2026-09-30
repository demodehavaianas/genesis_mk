#include "mkplus.h"

static Map *bgaMap;
static Map *bgbMap;
static Camera camera;

static void _setupPlayer(void);
void _spawnPlayers2(void);

void init(void)
{
    bool sair = FALSE;

    while (!sair)
    {
        inputSystem();
        gFrames++;

        if (gFrames == 1)
        {
            SPR_init();

            // fazer seleção do estágio e carregar seus assets
            // if palace gates entao init

            VDP_setWindowOnTop(2);

            initLifebar();

            // init relógio

            _setupPlayer();

            _spawnPlayers2();
        }

        drawLifeBar(WINDOW, PAL0, 1, 1, player[0].energia, 160, 17);
        drawLifeBar(WINDOW, PAL0, 22, 1, player[1].energia, 160, 17);

        if (gFrames == 15)
        {
            // iniciar sprites do round 1,2.3 FIGHT

            gPodeMover = TRUE;
        }

        animate();

        fsm_update();

        physics_update();

        CAMERA_tick(&camera, &player[0], &player[1]);

        SPR_update();
        SYS_doVBlankProcess();
    }
}

static void _setupPlayer(void)
{
    player_bind(&player[0], SUBZERO);
    player[0].paleta = PAL2;
    player[0].direcao = 1;
    player[0].energia = MAX_LIFE;
    player[0].state = PARADO;
    player[0].ops->set_state(0, PARADO);
    SPR_setVRAMTileIndex(player[0].sprite, gInd_tileset);
    gInd_tileset += player[0].sprite->definition->maxNumTile;

    player_bind(&player[1], SUBZERO);
    player[1].paleta = PAL3;
    player[1].direcao = -1;
    player[1].energia = MAX_LIFE;
    player[1].state = PARADO;
    player[0].ops->set_state(1, PARADO);
    SPR_setVRAMTileIndex(player[1].sprite, gInd_tileset);
    gInd_tileset += player[1].sprite->definition->maxNumTile;
}

void _spawnPlayers2(void)
{
    CAMERA_spawn(&camera, &player[0], &player[1]);
    SPR_update();
}