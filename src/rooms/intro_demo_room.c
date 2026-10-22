#include "mkplus.h"

#include "modulos/typewriter_printer.h"
#include "sprites.h"
#include "stages.h"
#include "audio.h"
#include "gfx.h"
#include "modulos/bio_textlines.h"

void loadBrainAtWorkScreen();
void loadMidwayTitleMKScreen();
void loadGoroLivesScreen();
void loadBioScreen(const BioData *data);
void saida();
void softClearPlane();

void saida()
{
    SPR_clear();
    PAL_fadeOut(0, 15, 5, FALSE);
    PAL_fadeOut(16, 30, 5, FALSE);
    PAL_setColors(0, palette_black, 64, DMA);
    XGM2_stop();
    SND_PCM4_stopPlay(SOUND_PCM_CH1);
    SPR_reset();
    gFrames = 0;
    gRoom = TELA_START;
    softClearPlane();
}

void softClearPlane()
{
    VDP_waitVSync();
    SYS_disableInts();
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setBackgroundColor(0); // Define preto
    gInd_tileset = TILE_USER_INDEX;
    SYS_enableInts();
}

void processIntro()
{
    bool sair = FALSE;

    while (!sair)
    {
        inputSystem();
        gFrames++;

        loadBrainAtWorkScreen();

        loadMidwayTitleMKScreen();

        loadGoroLivesScreen();

        if (gFrames == 1300)
        {
            VDP_waitVSync();

            switch (player[0].id) // preguiça de criar uma variável só pra isso
            {
            case JOHNNY_CAGE:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = KANO;
                break;
            case KANO:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = RAIDEN;
                break;
            case RAIDEN:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = LIU_KANG;
                break;
            case LIU_KANG:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = SUBZERO;
                break;
            case SUBZERO:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = SCORPION;
                break;
            case SCORPION:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = SONYA;
                break;
            case SONYA:
                loadBioScreen(getFighterBio(player[0].id));
                player[0].id = JOHNNY_CAGE;
                break;
            default:
                break;
            }
        }

        if (gFrames > 2100)
        {
            PAL_fadeOut(0, 15, 5, FALSE);
            PAL_fadeOut(16, 30, 5, FALSE);
            SPR_reset();
            XGM2_stop();
            softClearPlane();
            gFrames = 0;
        }

        // start só funciona a partir da tela título do MK ...
        if ((player[0].key_JOY_START_status > 0 || player[1].key_JOY_START_status > 0) && gFrames > 410)
        {
            sair = TRUE;
        }

        SPR_update(); // Atualização dos sprites na tela
        SYS_doVBlankProcess();
    }
    saida();
}

// TELA BRAIN AT WORK
void loadBrainAtWorkScreen()
{
    if (gFrames == 1)
    {
        SYS_disableInts();
        VDP_loadTileSet(baw_a.tileset, gInd_tileset, DMA);
        VDP_setTileMapEx(BG_A, baw_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA_QUEUE);
        PAL_setPalette(PAL0, baw_a.palette->data, DMA);
        gInd_tileset += baw_a.tileset->numTile;

        VDP_loadTileSet(baw_b.tileset, gInd_tileset, DMA);
        VDP_setTileMapEx(BG_B, baw_b.tilemap, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA_QUEUE);
        PAL_setPalette(PAL1, baw_b.palette->data, DMA);
        gInd_tileset += baw_b.tileset->numTile;

        PAL_fadeIn(0, 15, baw_a.palette->data, 8, FALSE);
        PAL_fadeIn(16, 30, baw_b.palette->data, 8, FALSE);
    }

    if (gFrames == 200)
    {
        PAL_fadeOut(0, 15, 5, FALSE);
        PAL_fadeOut(16, 30, 5, FALSE);

        softClearPlane();
    }
}

void loadMidwayTitleMKScreen()
{
    // TELA MIDWAY
    if (gFrames == 230)
    {
        softClearPlane();
        XGM2_stop();
        XGM2_playPCM(mus_midway, sizeof(mus_midway), SOUND_PCM_CH1_MSK);

        VDP_loadTileSet(midway.tileset, gInd_tileset, DMA);
        VDP_setTileMapEx(BG_A, midway.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA_QUEUE);
        PAL_setPalette(PAL0, midway.palette->data, DMA);
        gInd_tileset += midway.tileset->numTile;
    }

    // TELA TITULO MK
    if (gFrames == 370)
    {
        softClearPlane();

        XGM2_stop();
        XGM2_playPCM(mus_title, sizeof(mus_title), SOUND_PCM_CH1_MSK);

        VDP_loadTileSet(mk_title.tileset, gInd_tileset, DMA);
        VDP_setTileMapEx(BG_A, mk_title.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA_QUEUE);
        PAL_setPalette(PAL0, mk_title.palette->data, DMA);
        gInd_tileset += mk_title.tileset->numTile;

        // TODO: Refazer essa parte para usar mk_title_b
        //  VDP_loadTileSet(mk_title_b.tileset, gInd_tileset, DMA);
        //  VDP_setTileMapEx(BG_B, mk_title_b.tilemap, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA_QUEUE);
        //  PAL_setPalette(PAL1, mk_title_b.palette->data, DMA);
        //  gInd_tileset += mk_title_b.tileset->numTile;
    }
    gPodeMover = TRUE;
}

// TELA GORO LIVES
void loadGoroLivesScreen()
{
    if (gFrames == 700)
    {
        softClearPlane();

        SND_PCM4_stopPlay(SOUND_PCM_CH1);
        SND_PCM4_startPlay(mus_goro_lives, sizeof(mus_goro_lives), SOUND_PCM_CH1, FALSE);

        VDP_loadTileSet(goro_lives.tileset, gInd_tileset, DMA);
        VDP_setTileMapEx(BG_A, goro_lives.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset),
                         0, 0, 0, 0, 40, 28, DMA_QUEUE);
        PAL_setPalette(PAL0, goro_lives.palette->data, DMA);
        gInd_tileset += goro_lives.tileset->numTile;
    }

    // TELA GORO E HISTORIA
    if (gFrames == 855)
    {
        softClearPlane();

        PAL_setColors(0, palette_black, 64, DMA);
        VDP_loadTileSet(goro_a.tileset, gInd_tileset, DMA);
        VDP_setTileMapEx(BG_A, goro_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, 0, 0, 40, 28, DMA_QUEUE);
        PAL_setPalette(PAL0, goro_a.palette->data, DMA);
        gInd_tileset += goro_a.tileset->numTile;

        VDP_setTextPlane(BG_B);
        VDP_loadFontData(font_a.tiles, font_a.numTile, CPU);
        PAL_setPalette(PAL1, font_a_pal.data, DMA);
        VDP_setTextPalette(PAL1);
    }

    if (gFrames == 930)
    {
        const BioData *goroBio = getFighterBio(GORO);
        u16 sizeLine = (language == EN) ? goroBio->num_lines_br : goroBio->num_lines_en;

        for (u16 i = 0; i < sizeLine; i++)
        {
            if (language == EN)
            {
                typewriterEffect(goroBio->lines_EN[i].text, goroBio->lines_EN[i].x, goroBio->lines_EN[i].y, 0, BG_B, PAL1);
            }
            else
            {
                typewriterEffect(goroBio->lines_BR[i].text, goroBio->lines_BR[i].x, goroBio->lines_BR[i].y, 0, BG_B, PAL1);
            }
        }
    }

    if (gFrames == 1280)
    {
        PAL_setColors(0, palette_black, 64, DMA);
        SND_PCM4_stopPlay(SOUND_PCM_CH1);
    }
}

void loadBioScreen(const BioData *data)
{
    softClearPlane();
    PAL_fadeOut(0, 15, 5, FALSE);
    PAL_fadeOut(16, 30, 5, FALSE);

    VDP_loadTileSet(bio_b.tileset, gInd_tileset, DMA);
    VDP_setTileMapEx(BG_A, bio_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset),
                     0, 0, 0, 0, 40, 28, DMA_QUEUE);
    PAL_setPalette(PAL0, bio_b.palette->data, DMA);
    gInd_tileset += bio_b.tileset->numTile;

    Sprite *bioAnimation = SPR_addSprite(data->bio, 112, 48, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
    PAL_setPalette(PAL1, data->bio->palette->data, DMA);
    SPR_setAnimationLoop(bioAnimation, FALSE);
    SPR_setDepth(bioAnimation, 0);

    u16 size_name = (VDP_getScreenWidth() / 2) - (data->name->w / 2);
    Sprite *headName = SPR_addSprite(data->name, size_name, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
    PAL_setPalette(PAL2, data->name->palette->data, DMA);
    SPR_setDepth(headName, 0);

    // TODO: carregar a fonte apenas uma vez no início do jogo
    VDP_setTextPlane(BG_B);
    VDP_loadFontData(font_a.tiles, font_a.numTile, CPU);
    PAL_setPalette(PAL3, font_a_pal.data, DMA);
    VDP_setTextPalette(PAL3);

    XGM2_playPCM(data->voice, data->voice_size, SOUND_PCM_CH2);

    if(language == EN){

        typewriterWriteAllLines(data->lines_EN, data->num_lines_en, BG_B, PAL3);
    } else {
        typewriterWriteAllLines(data->lines_BR, data->num_lines_br, BG_B, PAL3);
    }

    XGM2_play(mus_the_beginning);
}
