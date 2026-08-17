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
#include "cenarios.h"
#include "input_system.h"
#include "gfx.h"
#include "sprites.h"
#include "anima_system.h"

// -- DECLARACAO DE VARIAVEIS -- //
u16 gDistancia;          // Distancia entre os Players
s16 gMeioDaTela = 0;     // Meio da Câmera em X
s16 camPosX = 0;         // Posicao da Camera
s16 camPosXanterior = 0; // Posicao da Camera no frame Anterior
s16 scrollOffset = 0;
s16 scrollValues[48];

void resetGraphicElements();
void CLEAR_VDP();

int main(bool hardReset)
{
  SPR_init();
  VDP_setScreenWidth320();
  VDP_setScreenHeight224();
  // VDP_setHilightShadow(TRUE);

  debugEnabled = FALSE;
  gRoom = SELECAO_PERSONAGENS;
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
      if (gFrames == 1)
        CLEAR_VDP();

      initPalaceGatesRoom();

      player[0].sprite = SPR_addSprite(&spr_subzero, 24, 96, TILE_ATTR(PAL2, 0, FALSE, FALSE));
      PAL_setPalette(PAL2, spr_subzero.palette->data, DMA);

      player[1].sprite = SPR_addSprite(&spr_reptile, 168, 96, TILE_ATTR(PAL3, 0, FALSE, TRUE));
      PAL_setPalette(PAL3, spr_reptile.palette->data, DMA);

      player[0].id = SUBZERO;
      player[1].id = REPTILE;
      player[0].state = PARADO;
      player[1].state = PARADO;
      player[0].paleta = PAL2;
      player[1].paleta = PAL3;

      scrollOffset += 1;
      VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_COLUMN);

      for (int i = 0; i < 48; i++)
      {
        scrollValues[i] = scrollOffset;
      }

      VDP_setHorizontalScrollLine(BG_B, 0, scrollValues, 48, CPU);
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