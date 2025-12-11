#include <genesis.h>

#include "game_vars.h"
#include "fighters.h"
#include "estruturas.h"
#include "cenarios.h"
#include "input_system.h"
#include "gfx.h"
#include "sprites.h"
#include "anima_system.h"

// -- DECLARACAO DE VARIAVEIS -- //
u16 gDistancia;          // Distancia entre os Players
s16 gMeioDaTela = 0;     // MEio da Câmera em X
s16 camPosX = 0;         // Posicao da Camera
s16 camPosXanterior = 0; // Posicao da Camera no frame Anterior

void resetGraphicElements();
void CLEAR_VDP();

int main(bool hardReset)
{
  SPR_init();
  VDP_setScreenWidth320();
  VDP_setScreenHeight224();
  // VDP_setHilightShadow(TRUE);

  debugEnabled = FALSE;
  gRoom = PALACE_GATES;
  gFrames = 0;
  gInd_tileset = 0;
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

    if(BONUS_STAGE == gRoom)
    {
      processBonusStage();
    }

    if (PALACE_GATES == gRoom)
    {
      initPalaceGatesRoom();
    }

    // -- DEBUG -- //
    if (debugEnabled)
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