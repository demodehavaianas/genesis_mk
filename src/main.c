/**
 * @file main.c
 * @author Nil Obermüller Schaupp a.k.a demodehavaianas (demoniodehavaianas@outlook.com)
 * @brief Orquestrador principal que gerencia o fluxo do jogo.
 * @version 0.1
 * @date 2025-08-11
 *
 */

#include <genesis.h>

#include "game_vars.h"
#include "fighters.h"
#include "estruturas.h"
#include "rooms/cenarios.h"
#include "modulos/input_system.h"
#include "gfx.h"
#include "sprites.h"
#include "modulos/anima_system.h"
#include "modulos/lifebar.h"

#include "stages.h"

// define width and height of tile map in pixels (resolution of map image)
#define MAP_WIDTH 1008
#define MAP_HEIGHT 240

// auxiliary macros
#define CAMERA_MAX_POS_X (MAP_WIDTH - VDP_getScreenWidth())
#define CAMERA_MAX_POS_Y (MAP_HEIGHT - VDP_getScreenHeight())

typedef struct
{
  V2s16 pos;
  V2s16 prevPos;
} Camera;

// -- DECLARACAO DE VARIAVEIS -- //
u16 gDistancia;          // Distancia entre os Players
s16 gMeioDaTela = 0;     // Meio da Câmera em X
s16 camPosX = 0;         // Posicao da Camera
s16 camPosXanterior = 0; // Posicao da Camera no frame Anterior
s16 scrollOffset = 0;
s16 scrollValues[48];

Map *palaceGatesMap; // Ponteiro para o mapa do Palace Gates
Map *palaceGatesbMap;
Camera camera;

void resetGraphicElements();
void CLEAR_VDP();

void CameraMoveByOffset(s16 x, s16 y)
{
  camera.pos.x += x;
  camera.pos.y += y;

  if (camera.pos.x < 0)
    camera.pos.x = 0;
  else if (camera.pos.x > CAMERA_MAX_POS_X)
    camera.pos.x = CAMERA_MAX_POS_X;

  if (camera.pos.y < 0)
    camera.pos.y = 0;
  else if (camera.pos.y > CAMERA_MAX_POS_Y)
    camera.pos.y = CAMERA_MAX_POS_Y;

  if ((camera.pos.x != camera.prevPos.x) || (camera.pos.y != camera.prevPos.y))
  {
    MAP_scrollTo(palaceGatesMap, camera.pos.x, camera.pos.y);
    MAP_scrollTo(palaceGatesbMap, camera.pos.x - 87, camera.pos.y >> 2);
    camera.prevPos = camera.pos;
  }
}

int main(bool hardReset)
{
  SPR_init();
  VDP_setScreenWidth320();
  VDP_setScreenHeight224();
  //VDP_setPlaneSize(64, 32, TRUE);
  // VDP_setScreenHeight240();
  //  VDP_setHilightShadow(TRUE);

  debugEnabled = FALSE;
  gRoom = PALACE_GATES;
  gFrames = 0;
  gInd_tileset = TILE_USER_INDEX;
  player[0].id = JOHNNY_CAGE;

  if (!hardReset)
  {
    SYS_hardReset();
  }

  resetGraphicElements();

  while (TRUE)
  {
    inputSystem();

    if (TELA_DEMO_INTRO == gRoom)
    {
      processIntro();
    }

    if (TELA_START == gRoom)
    {
      processPressStart();
    }

    if (SELECAO_PERSONAGENS == gRoom)
    {
      processSelecaoPersonagens();
    }

    if (BONUS_STAGE == gRoom)
    {
      processBonusStage();
    }
    // gFrames++;

    if (PALACE_GATES == gRoom)
    {
      initPalaceGatesRoom();
    }

    if(debugEnabled)
    {
      char str[64];
      sprintf(str, "tiles nos BGs: %d", gInd_tileset);
      VDP_drawText(str, 1, 1);
      sprintf(str, "contagem de frames: %ld", gFrames);
      VDP_drawText(str, 1, 2);
    }

    // -- FINALIZAÇÕES -- //
    // VDP_showFPS(1, 1, 1);
    SPR_update();          // Atualização dos sprites na tela
    SYS_doVBlankProcess(); // Sincroniza com o VBlank
  }

  return 0;
}

void CLEAR_VDP()
{
  SYS_disableInts();
  SPR_reset();
  VDP_resetSprites();
  VDP_releaseAllSprites();
  SPR_defragVRAM();
  VDP_clearPlane(BG_A, TRUE);
  VDP_clearPlane(BG_B, TRUE);
  // VDP_setTextPlane(BG_B);
  // VDP_setHorizontalScroll(BG_B, 0);
  // VDP_setVerticalScroll(BG_B, 0);
  // VDP_setHorizontalScroll(BG_A, 0);
  // VDP_setVerticalScroll(BG_A, 0);
  VDP_setBackgroundColor(0);
  VDP_resetScreen();
  // PAL_setColors(0, palette_black, 64, DMA);
  SYS_enableInts();
  gInd_tileset = 0;
}

void resetGraphicElements()
{
  for (int ind = 0; ind < 25; ind++)
  {
    if (GE[ind].sprite)
    {
      SPR_releaseSprite(GE[ind].sprite);
      GE[ind].sprite = NULL;
    }
  }
}