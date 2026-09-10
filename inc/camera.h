/**
 * camera.h — Camera de luta 2D (SGDK 2.11)
 *
 * Defina no header do estagio, ANTES de incluir este arquivo:
 *
 *   #define MAP_WIDTH  1008
 *   #define MAP_HEIGHT 240
 *   #define CAMERA_STAGE_MIN_X  0      // canto esquerdo do ring (independente)
 *   #define CAMERA_STAGE_MAX_X  1008   // canto direito do ring (independente)
 *   #include "camera.h"
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <genesis.h>
#include "estruturas.h"

#ifndef MAP_WIDTH
#define MAP_WIDTH  1008
#endif
#ifndef MAP_HEIGHT
#define MAP_HEIGHT 240
#endif

/* Cantos do CENARIO — cada estagio define o seu. Nao usam a margem da tela. */
#ifndef CAMERA_STAGE_MIN_X
#define CAMERA_STAGE_MIN_X  0
#endif
#ifndef CAMERA_STAGE_MAX_X
#define CAMERA_STAGE_MAX_X  MAP_WIDTH
#endif

/* Folga da TELA (anda com a camera). Esquerda e direita independentes. */
#ifndef CAMERA_SCREEN_MARGIN_L
#define CAMERA_SCREEN_MARGIN_L  8
#endif
#ifndef CAMERA_SCREEN_MARGIN_R
#define CAMERA_SCREEN_MARGIN_R  8
#endif

#define CAMERA_MIN_POS_X (CAMERA_STAGE_MIN_X)
#define CAMERA_MAX_POS_X (CAMERA_STAGE_MAX_X - VDP_getScreenWidth())
#define CAMERA_MAX_POS_Y (MAP_HEIGHT - VDP_getScreenHeight())

typedef struct
{
    V2s16 pos;
    s16   shakePower;
    u8    shakeTime;
    Map  *mapA;
    Map  *mapB;
} Camera;

/**
 * @brief Inicializa a camera no chao do mapa e posiciona os planes.
 *
 * @param cam   Camera
 * @param mapA  MAP do foreground (BG_A)
 * @param mapB  MAP do fundo (BG_B), ou NULL
 */
void CAMERA_init(Camera *cam, Map *mapA, Map *mapB);

/**
 * @brief Recoloca a camera no meio dos dois jogadores (inicio de round).
 */
void CAMERA_snap(Camera *cam, const Player *p1, const Player *p2);

/**
 * @brief Segue o medio X dos dois pivots e prende a camera no cenario.
 *
 * Se a dupla nao cabe na tela, trava o scroll (nao deixa um sair).
 * Y fica no chao (CAMERA_MAX_POS_Y).
 *
 * @param cam  Camera
 * @param p1   Player 1
 * @param p2   Player 2
 */
void CAMERA_update(Camera *cam, const Player *p1, const Player *p2);

/**
 * @brief Paredes que ANDAM com a camera + cantos independentes do cenario.
 *
 * 1. Canto esquerdo: sprite.left  >= CAMERA_STAGE_MIN_X
 * 2. Canto direito:  sprite.right <= CAMERA_STAGE_MAX_X
 *    Os dois cantos nao compartilham margem.
 * 3. Borda esquerda da TELA: sprite.left  >= cam.pos.x + SCREEN_MARGIN_L
 * 4. Borda direita da TELA:  sprite.right <= cam.pos.x + tela - SCREEN_MARGIN_R
 *    Estas duas andam com a camera. Sem isto o player sai da tela
 *    no meio do mapa.
 *
 * Chame DEPOIS de CAMERA_update, ANTES de CAMERA_placeSprite.
 *
 * @param cam  Camera ja atualizada neste frame
 * @param p    Player a prender
 */
void CAMERA_constrainPlayer(const Camera *cam, Player *p);

/**
 * @brief Tremor de tela (onda quadrada).
 *
 * @param power  fraco 3, médio 6, uppercut 12
 */
void CAMERA_shake(Camera *cam, s16 power);

/**
 * @brief Posiciona o sprite em tela e aplica HFlip conforme a direcao.
 *
 * Direita:  pos = (x - axisX, y - axisY) - camera.
 * Esquerda: o SGDK espelha o FRAME inteiro (128 px), o pe vai para
 * (definition->w - axisX). A caixa de colisao (w do corpo) espelha
 * em volta do pivot, nao do frame.
 */
void CAMERA_placeSprite(const Camera *cam, Player *p);

/**
 * @brief P1 a esquerda, P2 a direita, no centro do estágio. Exige axisX/axisY/w.
 */
void CAMERA_spawnPlayers(Camera *cam, Player *p1, Player *p2);

#endif