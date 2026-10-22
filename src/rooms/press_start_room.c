#include "mkplus.h"

#include "stages.h"
#include "sprites.h"
#include "gfx.h"

void loadBackground();
void loadMenu(u8 mainMenuOpt);

#define ACELERACAO 2 // aceleração da rolagem de tela
// Variáveis Locais
static s16 vecTilesScreen[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void processPressStart()
{
    u8 mainMenuOpt = 0;
    bool sair = FALSE;
    gPodeMover = TRUE;

    while (!sair)
    {
        inputSystem();

        gFrames++;

        if (gFrames == 5)
        {
            loadBackground();

            loadMenu(mainMenuOpt);
        }
        // --------------------------- MENU ------------------ FAZER UM MAIS ELABORADO ----------------------
        if (mainMenuOpt == 0) // enquanto não implementar timer de tela
        {
            if (player[0].key_JOY_B_status == 1)
            {
                gRoom = TELA_DEMO_INTRO;
                sair = TRUE;
            }
        }
        // VAI PARA OPTIONS
        if ((player[0].key_JOY_DOWN_status == 1 || player[1].key_JOY_DOWN_status == 1) && mainMenuOpt == 0)
        {
            mainMenuOpt = 1;
            SPR_setFrame(GE[0].sprite, mainMenuOpt);
        }
        // VAI PARA START
        if ((player[0].key_JOY_UP_status == 1 || player[1].key_JOY_UP_status == 1) && mainMenuOpt == 1)
        {
            mainMenuOpt = 0;
            // SPR_setAnim(GE[0].sprite, mainMenuOpt);
            SPR_setFrame(GE[0].sprite, mainMenuOpt);
        }
        // SE ESTIVER EM OPTIONS
        if ((player[0].key_JOY_START_status > 0 || player[1].key_JOY_START_status > 0) && mainMenuOpt == 1)
        {
            mainMenuOpt = 2;
            SPR_setFrame(GE[0].sprite, mainMenuOpt);
            (language == EN) ? SPR_setPosition(GE[1].sprite, 104, 80) : SPR_setPosition(GE[1].sprite, 144, 80);
            SPR_setVisibility(GE[1].sprite, VISIBLE);
        }
        // SE TIVER DENTRO DO MENU DE OPÇÕES
        if (mainMenuOpt == 2)
        {
            if ((player[0].key_JOY_LEFT_status == 1 || player[1].key_JOY_LEFT_status == 1) ||
                (player[0].key_JOY_RIGHT_status == 1 || player[1].key_JOY_RIGHT_status == 1))
            {
                if (language == EN)
                {
                    SPR_setPosition(GE[1].sprite, 144, 80);
                    language = BR;
                }
                else
                {
                    SPR_setPosition(GE[1].sprite, 104, 80);
                    language = EN;
                }
            }
            if ((player[0].key_JOY_B_status == 1 || player[1].key_JOY_B_status == 1))
            {
                mainMenuOpt = 0;
                SPR_setFrame(GE[0].sprite, mainMenuOpt);
                SPR_setVisibility(GE[1].sprite, HIDDEN);
            }
        }
        // SE APERTAR START
        if ((player[0].key_JOY_START_status > 0 || player[1].key_JOY_START_status > 0) &&
            (gFrames > 30 && mainMenuOpt == 0))
        {
            gRoom = SELECAO_PERSONAGENS;
            sair = TRUE;
        }
        // -------------------------------------------------------------------------------------------------

        vecTilesScreen[0] -= ACELERACAO;
        vecTilesScreen[1] -= ACELERACAO;
        vecTilesScreen[2] -= ACELERACAO;
        vecTilesScreen[17] -= ACELERACAO;
        vecTilesScreen[18] -= ACELERACAO;
        vecTilesScreen[19] -= ACELERACAO;

        if (vecTilesScreen[0] == -512) // evitar que estore contador
        {
            vecTilesScreen[0] = 0;
            vecTilesScreen[1] = 0;
            vecTilesScreen[2] = 0;
            vecTilesScreen[17] = 0;
            vecTilesScreen[18] = 0;
            vecTilesScreen[19] = 0;
        }

        VDP_setVerticalScrollTile(BG_A, 0, vecTilesScreen, 20, CPU);
        VDP_setVerticalScrollTile(BG_B, 0, vecTilesScreen, 20, CPU);

        SPR_update();
        SYS_doVBlankProcess();
    }

    // --- metodo novo ----
    SPR_reset();
    PAL_fadeOut(0, 15, 5, FALSE);
    PAL_fadeOut(16, 30, 5, FALSE);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setBackgroundColor(0);
    VDP_resetScreen();
    VDP_setHorizontalScroll(BG_A, 0);
    VDP_setHorizontalScroll(BG_B, 0);
    VDP_setVerticalScroll(BG_A, 0);
    VDP_setVerticalScroll(BG_B, 0);
    gFrames = 0;
    gInd_tileset = TILE_USER_INDEX;
    SYS_enableInts();

    // ---- metodo antigo ---
    // SPR_clear();
    // PAL_fadeOut(0, 15, 5, FALSE);
    // PAL_fadeOut(16, 30, 5, FALSE);
    // PAL_setColors(0, palette_black, 64, DMA);
    // gFrames = 0;
    // // gRoom = SELECAO_PERSONAGENS;
    // //--
    // VDP_waitVSync();
    // SYS_disableInts();
    // VDP_resetScreen();
    // // VDP_clearPlane(BG_A, TRUE);
    // // VDP_clearPlane(BG_B, TRUE);
    // // VDP_setBackgroundColor(0); // Define preto
    // gInd_tileset = TILE_USER_INDEX;
    // SYS_enableInts();
}

// carrega os sprites do menu
void loadMenu(u8 mainMenuOpt)
{
    GE[0].sprite = SPR_addSprite(&spMainMenu, 96, 80, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    PAL_setPalette(PAL0, spMainMenu.palette->data, DMA);
    SPR_setFrame(GE[0].sprite, mainMenuOpt);

    GE[1].sprite = SPR_addSprite(&spOpt, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    PAL_setPalette(PAL0, spOpt.palette->data, DMA);
    SPR_setVisibility(GE[1].sprite, HIDDEN);
}

// carrega os planos 
void loadBackground()
{
    VDP_setPlaneSize(64, 64, TRUE);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_COLUMN);

    VDP_loadTileSet(screen_opt_a.tileset, gInd_tileset, DMA);
    VDP_setTileMapEx(BG_A, screen_opt_a.tilemap,
                     TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 64, CPU);
    PAL_setPalette(PAL0, char_select_a_pal.data, DMA);
    gInd_tileset += screen_opt_a.tileset->numTile;

    VDP_loadTileSet(screen_opt_b.tileset, gInd_tileset, DMA);
    VDP_setTileMapEx(BG_B, screen_opt_b.tilemap,
                     TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 64, CPU);
    PAL_setPalette(PAL1, char_select_b_pal.data, DMA);
    gInd_tileset += screen_opt_b.tileset->numTile;
}
