#include <genesis.h>

#include "char_select_room.h"
#include "input_system.h"
#include "stages.h"
#include "sprites.h"
#include "audio.h"
#include "game_vars.h"
#include "fighters.h"
#include "anima_system.h"

#define TILEMAP_WIDTH 40
#define TILEMAP_HEIGHT 28
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224
#define LINE_HEIGHT 2 // Altura da linha em pixels
#define PLAYER_1_POS_X 10
#define PLAYER_1_POS_Y 104
#define PLAYER_2_POS_X 182
#define PLAYER_2_POS_Y 104
#define CURSOR_P1 0
#define CURSOR_P2 1

void updateSelector(int ind);
void playerSelected(int ind);
void playerSelectBlinkEffect(int ind, u8 selectorBlinkTimer[2]);
void initScrollLine(s16 *scrollLine);
void drawBackground();
void revealBackground(s16 *scrollLine);
void initPlayer();
void initSelectorSprite();
void playCursor();
void playMusic();
void freeScrollLine(s16 *scrollLine);
void exit();

// Seguir sempre a ordem do Enum
// CAGE      20, 44
// KANO      76, 44
// RAIDEN    76, 108
// LIU KANG 132, 108
// SUBZERO  188, 44
// SCORPION 188, 108
// SONYA    244, 44
const u8 OPTIONS_X[7] = {20, 76, 76, 132, 188, 188, 244};
const u8 OPTIONS_Y[7] = {44, 44, 108, 108, 44, 108, 44};

typedef struct
{
  const u8 *locutor;
  int size;
  u8 x, y;
} CharSelectData;

// TODO: acertar isso pois não dá para pegar o tamanho dinamicamente.
static const CharSelectData charData[7] = {
    {loc_jc, 13056, OPTIONS_X[JOHNNY_CAGE], OPTIONS_Y[JOHNNY_CAGE]},
    {loc_kano, 8448, OPTIONS_X[KANO], OPTIONS_Y[KANO]},
    {loc_raiden, 7680, OPTIONS_X[RAIDEN], OPTIONS_Y[RAIDEN]},
    {loc_liu_kang, 13056, OPTIONS_X[LIU_KANG], OPTIONS_Y[LIU_KANG]},
    {loc_suzero, 15872, OPTIONS_X[SUBZERO], OPTIONS_Y[SUBZERO]},
    {loc_scorpion, 13568, OPTIONS_X[SCORPION], OPTIONS_Y[SCORPION]},
    {loc_sonya, 11264, OPTIONS_X[SONYA], OPTIONS_Y[SONYA]},
};

void exit()
{
  SPR_clear();
  // VDP_resetSprites();

  if (GE[1].sprite)
  {
    SPR_releaseSprite(GE[1].sprite);
    GE[1].sprite = NULL;
  }
  if (GE[2].sprite)
  {
    SPR_releaseSprite(GE[2].sprite);
    GE[2].sprite = NULL;
  }
  if (GE[3].sprite)
  {
    SPR_releaseSprite(GE[3].sprite);
    GE[3].sprite = NULL;
  }
  if (GE[4].sprite)
  {
    SPR_releaseSprite(GE[4].sprite);
    GE[4].sprite = NULL;
  }
  VDP_releaseAllSprites();
  gFrames = 0;
  gRoom = TELA_DEMO_INTRO;
  // VDP_resetScreen();
  VDP_setBackgroundColor(0); // Define preto
  gInd_tileset = TILE_USER_INDEX;

  waitMs(1000);
  VDP_waitVSync();
  SYS_enableInts();
}

void processSelecaoPersonagens()
{
  bool sair = FALSE;
  u8 selectorBlinkTimer[2] = {0, 0}; // timer para o efeito de piscar o retrato do personagem selecionado

  s16 countDown = -1;

  while (!sair)
  {
    inputSystem();

    gFrames++;

    // desabilita interações, ajusta planos e modo de scrolling
    if (gFrames == 1)
    {
      SYS_disableInts();
      gPodeMover = FALSE;
      VDP_setPlaneSize(128, 64, TRUE);
      VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
    }

    // toca o gongo e inicia o efeito persinana revelando a tela de fundo
    if (gFrames == 20)
    {
      s16 *scrollLine = (s16 *)malloc(SCREEN_HEIGHT * sizeof(s16));

      XGM2_playPCMEx(snd_gongo, sizeof(snd_gongo), SOUND_PCM_CH2, 0, FALSE, 0);

      initScrollLine(scrollLine);

      drawBackground();

      revealBackground(scrollLine);
    }

    // toca música, inicia o player, sprites ...
    if (gFrames == 40)
    {
      playMusic();

      initPlayer();

      initSelectorSprite();

      VDP_waitVSync();
      gPodeMover = TRUE; // previnir bug de sprite aparecer errado se ficar movendo o dpad
    }

    // permite interações
    if (gFrames > 100)
    {
      SYS_enableInts();
    }

    for (int ind = 0; ind < 2; ind++) // para cada jogador
    {
      updateSelector(ind);

      playerSelected(ind);

      playerSelectBlinkEffect(ind, selectorBlinkTimer);
    }

    // TODO: repensar uma forma melhor
    if (player[0].selecionado && player[1].selecionado)
    {
      if (countDown == -1)
      {
        countDown = 150;
      }
      else if (countDown == 0)
      {
        sair = TRUE;
      }
      else if (countDown == 50)
      {
        SYS_disableInts();
        PAL_fadeOutAll(18, FALSE);
        XGM2_stop();
        VDP_clearPlane(BG_A, TRUE);
        VDP_clearPlane(BG_B, TRUE);
      }
      countDown--;
    }

    // Mostra os IDs dos personagens
    if (debugEnabled)
    {
      static char stri[64];
      sprintf(stri, "p1: %d", player[0].id);
      VDP_drawText(stri, 1, 1);
      sprintf(stri, "p2: %d", player[1].id);
      VDP_drawText(stri, 1, 2);
      sprintf(stri, "gframes: %ld", gFrames);
      VDP_drawText(stri, 1, 3);
    }

    SPR_update();
    SYS_doVBlankProcess();
  }

  exit();
}

// faz o efeito de piscar o retrato ao selecionar o personagem
void playerSelectBlinkEffect(int ind, u8 selectorBlinkTimer[2])
{
  // Se o sprite P&B da foto do personagem estiver visível..
  if (SPR_isVisible(GE[ind + 2].sprite, TRUE))
  {
    // para piscar novamente caso selecionem o mesmo personagem
    if ((player[0].selecionado && player[1].selecionado))
    {
      if (player[0].id == player[1].id)
      {
        SPR_setDepth(GE[ind + 2].sprite, SPR_MIN_DEPTH);
        SPR_setAnim(GE[ind + 2].sprite, player[ind].id);
      }
    }

    selectorBlinkTimer[ind]++;
    // se divisível por 5 ...
    if (selectorBlinkTimer[ind] % 5 == 0)
    {
      SPR_nextFrame(GE[ind + 2].sprite);
    }
    // para a animação
    if (selectorBlinkTimer[ind] > 30)
    {
      SPR_setAnimationLoop(GE[ind + 2].sprite, FALSE);
    }
  }
}

/**
 * @brief Atualiza a posição do cursor de seleção de acordo com o ID
 *
 * @param ind indice usado pelo GraphicElements
 */
void updateSelector(int ind)
{
  if (GE[ind].sprite->visibility == HIDDEN) // se o seletor estiver invisível não permitir mover o cursor.
    return;

  int prevId = player[ind].id; // guarda personagem anterior

  switch (player[ind].id)
  {
  case KANO:

    if (player[ind].key_JOY_LEFT_status == 1) // se apertar para esquerda, seleciona CAGE
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[JOHNNY_CAGE], OPTIONS_Y[JOHNNY_CAGE]);
      player[ind].id = JOHNNY_CAGE;
    }
    else if (player[ind].key_JOY_RIGHT_status == 1) // se apertar para direita, Subzero
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SUBZERO], OPTIONS_Y[SUBZERO]);
      player[ind].id = SUBZERO;
    }
    else if (player[ind].key_JOY_DOWN_status == 1) // se apertar para baixo, Raiden
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[RAIDEN], OPTIONS_Y[RAIDEN]);
      player[ind].id = RAIDEN;
    }
    break;

  case JOHNNY_CAGE:

    if (player[ind].key_JOY_RIGHT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[KANO], OPTIONS_Y[KANO]);
      player[ind].id = KANO;
    }
    else if (player[ind].key_JOY_LEFT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SONYA], OPTIONS_Y[SONYA]);
      player[ind].id = SONYA;
    }
    break;

  case SUBZERO:

    if (player[ind].key_JOY_LEFT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[KANO], OPTIONS_Y[KANO]);
      player[ind].id = KANO;
    }
    else if (player[ind].key_JOY_RIGHT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SONYA], OPTIONS_Y[SONYA]);
      player[ind].id = SONYA;
    }
    else if (player[ind].key_JOY_DOWN_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SCORPION], OPTIONS_Y[SCORPION]);
      player[ind].id = SCORPION;
    }
    break;

  case SONYA:

    if (player[ind].key_JOY_LEFT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SUBZERO], OPTIONS_Y[SUBZERO]);
      player[ind].id = SUBZERO;
    }
    else if (player[ind].key_JOY_RIGHT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[JOHNNY_CAGE], OPTIONS_Y[JOHNNY_CAGE]);
      player[ind].id = JOHNNY_CAGE;
    }
    break;

  case SCORPION:

    if (player[ind].key_JOY_LEFT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[LIU_KANG], OPTIONS_Y[LIU_KANG]);
      player[ind].id = LIU_KANG;
    }
    else if (player[ind].key_JOY_UP_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SUBZERO], OPTIONS_Y[SUBZERO]);
      player[ind].id = SUBZERO;
    }
    break;

  case LIU_KANG:

    if (player[ind].key_JOY_RIGHT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[SCORPION], OPTIONS_Y[SCORPION]);
      player[ind].id = SCORPION;
    }
    else if (player[ind].key_JOY_LEFT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[RAIDEN], OPTIONS_Y[RAIDEN]);
      player[ind].id = RAIDEN;
    }
    break;

  case RAIDEN:

    if (player[ind].key_JOY_RIGHT_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[LIU_KANG], OPTIONS_Y[LIU_KANG]);
      player[ind].id = LIU_KANG;
    }
    else if (player[ind].key_JOY_UP_status == 1)
    {
      playCursor();
      SPR_setPosition(GE[ind].sprite, OPTIONS_X[KANO], OPTIONS_Y[KANO]);
      player[ind].id = KANO;
    }
    break;

  default:
    break;
  }

  // se o ID mudou, troca o sprite do personagem
  if (prevId != player[ind].id)
  {
    playerState(ind, PARADO);
  }
}

// TODO: ver o que fazer quando ambos selecionam ao mesmo tempo
// Verifica se o jogador selecionou um personagem
void playerSelected(int ind)
{
  if (GE[ind].sprite->visibility == HIDDEN)
    return;
  if (player[ind].key_JOY_START_status == 0)
    return;

  const CharSelectData *d = &charData[player[ind].id];
  XGM2_playPCMEx(d->locutor, d->size, SOUND_PCM_CH_AUTO, 0, FALSE, FALSE);
  GE[ind + 2].sprite = SPR_addSprite(&spPortrait, d->x + 4, d->y + 4, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
  SPR_setAnim(GE[ind + 2].sprite, player[ind].id);
  SPR_setDepth(GE[ind + 2].sprite, 2);
  GE[ind].sprite->visibility = HIDDEN;
  player[ind].selecionado = TRUE;
}

/**
 * @brief move a tela para fora da parte visível a esquerda
 * para iniciar o efeito de Persiana.
 */
void initScrollLine(s16 *scrollLine)
{
  // Aloca dinamicamente memória
  // scrollLine = (s16 *)malloc(SCREEN_HEIGHT * sizeof(s16));
  if (!scrollLine) // Verifica se a alocação foi bem-sucedida
  {
    // Falha na alocação
    return;
  }

  for (int x = 0; x < SCREEN_HEIGHT; x++)
  {
    scrollLine[x] = -SCREEN_WIDTH; // inicia as linhas com 320px
  }
  VDP_setHorizontalScrollLine(BG_A, 0, scrollLine, SCREEN_HEIGHT, DMA);
  VDP_setHorizontalScrollLine(BG_B, 0, scrollLine, SCREEN_HEIGHT, DMA);
}

/**
 * @brief Carrega as informações dos backgrounds
 */
void drawBackground()
{
  // BACKGROUND A
  VDP_loadTileSet(stage_char_select_a.tileset, gInd_tileset, DMA);
  VDP_setTileMapEx(BG_A, stage_char_select_a.tilemap,
                   TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset),
                   0, 0,
                   0, 0,
                   TILEMAP_WIDTH, TILEMAP_HEIGHT, DMA);
  PAL_setPalette(PAL0, char_select_a_pal.data, DMA);
  gInd_tileset += stage_char_select_a.tileset->numTile;

  // BACKGROUND B
  VDP_loadTileSet(stage_char_select_b.tileset, gInd_tileset, DMA);
  VDP_setTileMapEx(BG_B, stage_char_select_b.tilemap,
                   TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset),
                   0, 0,
                   0, 0,
                   TILEMAP_WIDTH, TILEMAP_HEIGHT, DMA);
  PAL_setPalette(PAL1, char_select_b_pal.data, DMA);
  gInd_tileset += stage_char_select_b.tileset->numTile;
}

/**
 * @brief Revela a tela fazendo o efeito de persiana.
 */
void revealBackground(s16 *scrollLine)
{
  struct VenetianBlindsEffect
  {
    u16 startLine;   // linha onde começa o efeito
    u16 nextLine;    // linha onde deve começar o próximo efeito
    u16 endLine;     // linha onde termina o efeito
    u16 currentLine; // linha atual da iteração
  };

  struct VenetianBlindsEffect persiana[7] = {
      {0, 13, 31, 0},
      {32, 43, 63, 0},
      {64, 73, 95, 0},
      {96, 105, 127, 0},
      {128, 137, 159, 0},
      {160, 167, 191, 0},
      {192, 0, 223, 0},
  };

  for (int y = 0; y < SCREEN_HEIGHT / 2; y += 1)
  {
    // 0
    if (y >= persiana[0].startLine && y <= persiana[0].endLine)
    {
      persiana[0].currentLine = y;
      scrollLine[persiana[0].currentLine] = 0;
      scrollLine[persiana[0].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, y, &scrollLine[persiana[0].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, y, &scrollLine[persiana[0].currentLine], LINE_HEIGHT, DMA);
    }
    // 1
    if (y >= persiana[0].nextLine && persiana[1].currentLine <= persiana[1].endLine)
    {
      if (persiana[0].nextLine == y)
        persiana[1].currentLine = persiana[1].startLine;
      else
        persiana[1].currentLine += 1;
      scrollLine[persiana[1].currentLine] = 0;
      scrollLine[persiana[1].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, persiana[1].currentLine, &scrollLine[persiana[1].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, persiana[1].currentLine, &scrollLine[persiana[1].currentLine], LINE_HEIGHT, DMA);
    }
    // 2
    if (persiana[1].currentLine >= persiana[1].nextLine && persiana[2].currentLine <= persiana[2].endLine)
    {
      if (persiana[1].nextLine == persiana[1].currentLine)
        persiana[2].currentLine = persiana[2].startLine;
      else
        persiana[2].currentLine += 1;
      scrollLine[persiana[2].currentLine] = 0;
      scrollLine[persiana[2].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, persiana[2].currentLine, &scrollLine[persiana[2].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, persiana[2].currentLine, &scrollLine[persiana[2].currentLine], LINE_HEIGHT, DMA);
    }
    // 3
    if (persiana[2].currentLine >= persiana[2].nextLine && persiana[3].currentLine <= persiana[3].endLine)
    {
      if (persiana[2].nextLine == persiana[2].currentLine)
        persiana[3].currentLine = persiana[3].startLine;
      else
        persiana[3].currentLine += 1;
      scrollLine[persiana[3].currentLine] = 0;
      scrollLine[persiana[3].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, persiana[3].currentLine, &scrollLine[persiana[3].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, persiana[3].currentLine, &scrollLine[persiana[3].currentLine], LINE_HEIGHT, DMA);
    }
    // 4
    if (persiana[3].currentLine >= persiana[3].nextLine && persiana[4].currentLine <= persiana[4].endLine)
    {
      if (persiana[3].nextLine == persiana[3].currentLine)
        persiana[4].currentLine = persiana[4].startLine;
      else
        persiana[4].currentLine += 1;
      scrollLine[persiana[4].currentLine] = 0;
      scrollLine[persiana[4].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, persiana[4].currentLine, &scrollLine[persiana[4].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, persiana[4].currentLine, &scrollLine[persiana[4].currentLine], LINE_HEIGHT, DMA);
    }
    // 5
    if (persiana[4].currentLine >= persiana[4].nextLine && persiana[5].currentLine <= persiana[5].endLine)
    {
      if (persiana[4].nextLine == persiana[4].currentLine)
        persiana[5].currentLine = persiana[5].startLine;
      else
        persiana[5].currentLine += 1;
      scrollLine[persiana[5].currentLine] = 0;
      scrollLine[persiana[5].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, persiana[5].currentLine, &scrollLine[persiana[5].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, persiana[5].currentLine, &scrollLine[persiana[5].currentLine], LINE_HEIGHT, DMA);
    }
    // 6
    if (persiana[5].currentLine >= persiana[5].nextLine && persiana[6].currentLine <= persiana[6].endLine)
    {
      if (persiana[5].nextLine == persiana[5].currentLine)
        persiana[6].currentLine = persiana[6].startLine;
      else
        persiana[6].currentLine += 1;
      scrollLine[persiana[6].currentLine] = 0;
      scrollLine[persiana[6].currentLine + 1] = 0;
      VDP_setHorizontalScrollLine(BG_A, persiana[6].currentLine, &scrollLine[persiana[6].currentLine], LINE_HEIGHT, DMA);
      VDP_setHorizontalScrollLine(BG_B, persiana[6].currentLine, &scrollLine[persiana[6].currentLine], LINE_HEIGHT, DMA);
    }

    SYS_doVBlankProcess();
  }

  freeScrollLine(scrollLine);
}

void initPlayer()
{
  // for (int ind = 0; ind < 2; ind++)
  // {
  //   player[ind].key_JOY_countdown[2] = 0;
  //   player[ind].key_JOY_countdown[4] = 0;
  //   player[ind].key_JOY_countdown[6] = 0;
  //   player[ind].key_JOY_countdown[8] = 0;
  // }
  gAlturaDoPiso = PLAYER_1_POS_Y;
  player[0].id = KANO;
  player[0].state = PARADO;
  player[0].paleta = PAL2;
  player[0].direcao = 1;
  player[0].x = PLAYER_1_POS_X;
  player[0].y = PLAYER_1_POS_Y;
  player[0].selecionado = FALSE;

  player[1].id = SUBZERO;
  player[1].state = PARADO;
  player[1].paleta = PAL3;
  player[1].direcao = -1;
  player[1].x = PLAYER_2_POS_X;
  player[1].y = PLAYER_2_POS_Y;
  player[1].selecionado = FALSE;

  playerState(0, PARADO);
  playerState(1, PARADO);

  memset(player[0].key_JOY_countdown, 0, sizeof(player[0].key_JOY_countdown));
  memset(player[1].key_JOY_countdown, 0, sizeof(player[1].key_JOY_countdown));

  // player[0].key_JOY_countdown[2] = 0;
  // player[0].key_JOY_countdown[4] = 0;
  // player[0].key_JOY_countdown[6] = 0;
  // player[0].key_JOY_countdown[8] = 0;

  // player[1].key_JOY_countdown[2] = 0;
  // player[1].key_JOY_countdown[4] = 0;
  // player[1].key_JOY_countdown[6] = 0;
  // player[1].key_JOY_countdown[8] = 0;
}

void initSelectorSprite()
{
  // indice 0 e 1 do GE serão usados para carregar o gráfico do seletor
  GE[0].sprite = SPR_addSpriteSafe(&player_seletor,
                               OPTIONS_X[KANO], OPTIONS_Y[KANO],
                               TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
  SPR_setAnim(GE[0].sprite, CURSOR_P1); // animação P1
  SPR_setDepth(GE[0].sprite, SPR_MIN_DEPTH);

  GE[1].sprite = SPR_addSpriteSafe(&player_seletor,
                               OPTIONS_X[SUBZERO], OPTIONS_Y[SUBZERO],
                               TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
  SPR_setAnim(GE[1].sprite, CURSOR_P2); // animação P2
  SPR_setDepth(GE[1].sprite, 1);        // profundidade menor que o seletor do P1
  // inicia a animação dos sprites
  SPR_setAnimationLoop(GE[0].sprite, TRUE);
  SPR_setAnimationLoop(GE[1].sprite, TRUE);
}

/**
 * @brief Executa o áudio do cursor */
void playCursor()
{
  XGM2_playPCMEx(snd_cursor, sizeof(snd_cursor), SOUND_PCM_CH_AUTO, 0, FALSE, 0);
}

void playMusic()
{
  XGM2_play(mus_select_player);
}

void freeScrollLine(s16 *scrollLine)
{
  if (scrollLine)
  {
    free(scrollLine); // Libera a memória alocada dinamicamente
    scrollLine = NULL;
  }
}