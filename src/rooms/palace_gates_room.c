#include <genesis.h>
#include "palace_gates_room.h"
#include "stages.h"
#include "game_vars.h"
#include "camera.h"
#include "sprites.h"
#include "input_system.h"

#define SUBZERO_BODY_W 52
#define SUBZERO_BODY_H 117
#define SUBZERO_AXIS_X 22
#define SUBZERO_AXIS_Y 119

static const CameraStage palaceGatesCam = {
    .mapWidth = PALACE_GATES_MAP_W,
    .mapHeight = PALACE_GATES_MAP_H,
    .ringLeft = PALACE_GATES_RING_LEFT,
    .ringRight = PALACE_GATES_RING_RIGHT,
    .farBgLeft = PALACE_GATES_FARBG_LEFT,
    .farBgRight = PALACE_GATES_FARBG_RIGHT,
    .startX = PALACE_GATES_START_X,
    .startY = PALACE_GATES_START_Y,
};

static Map *bgaMap;
static Map *bgbMap;
static Camera camera;

void PALACE_GATES_init(void);
void PALACE_GATES_spawnPlayers(void);
static void PALACE_GATES_setupPlayer(Player *p, const SpriteDefinition *spr, u16 pal, s8 direcao);
static void PALACE_GATES_movePlayer(Player *p);

void initPalaceGatesRoom(void)
{
    bool sair = FALSE;

    while (!sair)
    {
        inputSystem();
        gFrames++;

        if (gFrames == 1)
        {
            SPR_init();

            PALACE_GATES_init();

            PALACE_GATES_setupPlayer(&player[0], &spr_subzero, PAL2, 1);
            PALACE_GATES_setupPlayer(&player[1], &spr_subzero, PAL3, -1);

            PALACE_GATES_spawnPlayers();
            gPodeMover = TRUE;
        }

        PALACE_GATES_movePlayer(&player[0]);
        PALACE_GATES_movePlayer(&player[1]);

        CAMERA_tick(&camera, &player[0], &player[1]);

        SPR_update();
        SYS_doVBlankProcess();
    }
}

void PALACE_GATES_init(void)
{
    gInd_tileset = TILE_USER_INDEX;

    VDP_setScreenWidth320();
    VDP_setPlaneSize(64, 32, TRUE);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    VDP_loadTileSet(&palace_gates_bgb_tileset, gInd_tileset, DMA);
    PAL_setPalette(PAL0, palace_gates_bgb_pal.data, DMA);
    bgbMap = MAP_create(&palace_gates_bgb_map, BG_B,
                        TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset));
    gInd_tileset += palace_gates_bgb_tileset.numTile;

    SYS_doVBlankProcess();

    VDP_loadTileSet(&palace_gates_bga_tileset, gInd_tileset, DMA);
    PAL_setPalette(PAL1, palace_gates_bga_pal.data, DMA);
    bgaMap = MAP_create(&palace_gates_bga_map, BG_A,
                        TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset));
    gInd_tileset += palace_gates_bga_tileset.numTile;

    CAMERA_setup(&camera, bgaMap, bgbMap, &palaceGatesCam);
}

static void PALACE_GATES_setupPlayer(Player *p, const SpriteDefinition *spr, u16 pal, s8 direcao)
{
    p->sprite = SPR_addSprite(spr, 0, 0, TILE_ATTR(pal, TRUE, FALSE, FALSE));
    PAL_setPalette(pal, spr->palette->data, DMA);
    SPR_setVRAMTileIndex(p->sprite, gInd_tileset);
    gInd_tileset += spr->maxNumTile;

    p->id = SUBZERO;
    p->state = PARADO;
    p->paleta = pal;
    p->hSpeed = 2;
    p->direcao = direcao;
    p->w = SUBZERO_BODY_W;
    p->h = SUBZERO_BODY_H;
    p->axisX = SUBZERO_AXIS_X;
    p->axisY = SUBZERO_AXIS_Y;
}

void PALACE_GATES_spawnPlayers(void)
{
    CAMERA_spawn(&camera, &player[0], &player[1]);
    SPR_update();
}

static void PALACE_GATES_movePlayer(Player *p)
{
    const u8 left = p->key_JOY_LEFT_status;
    const u8 right = p->key_JOY_RIGHT_status;

    if ((left == BUTTON_PRESSED) || (left == BUTTON_HELD))
        p->x -= p->hSpeed;
    else if ((right == BUTTON_PRESSED) || (right == BUTTON_HELD))
        p->x += p->hSpeed;

    if (p->key_JOY_A_status == BUTTON_PRESSED)
        CAMERA_shake(&camera, 2);
    if (p->key_JOY_B_status == BUTTON_PRESSED)
        CAMERA_shake(&camera, 4);
    if (p->key_JOY_C_status == BUTTON_PRESSED)
        CAMERA_shake(&camera, 8);
}