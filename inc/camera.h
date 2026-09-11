/**
 * camera.h — Camera de luta 2D (SGDK 2.11)
 *
 * Setup uma vez, spawn no round, tick no loop, shake no golpe.
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <genesis.h>
#include "estruturas.h"

/** Folga em pixels entre o corpo e a borda visivel da tela. */
#define CAMERA_SCREEN_MARGIN  8
/** Pes um pouco acima da borda inferior da tela no spawn. */
#define CAMERA_FEET_INSET     8

/**
 * @brief Dados do palco. Cada estagio preenche um destes.
 *
 * ring*     = onde o SPRITE para (mundo), independente da tela.
 * farBg*    = intervalo de scroll do BGB (parallax, sem ceu vazio).
 * start*    = canto superior-esquerdo da tela no spawn.
 */
typedef struct
{
    u16 mapWidth;
    u16 mapHeight;
    s16 ringLeft;
    s16 ringRight;
    s16 farBgLeft;
    s16 farBgRight;
    s16 startX;
    s16 startY;
} CameraStage;

typedef struct
{
    V2s16 pos;              /* canto da tela em coordenadas de mundo */
    s16   shakePower;
    u8    shakeTime;

    s16   ringLeft;         /* sprite.left  nao passa daqui */
    s16   ringRight;        /* sprite.right nao passa daqui */
    s16   scrollMinX;       /* 0 */
    s16   scrollMaxX;       /* mapWidth - tela */
    s16   scrollMaxY;       /* mapHeight - tela */

    s16   farBgLeft;        /* parallax BGB no canto esquerdo do palco */
    s16   farBgRight;       /* parallax BGB no canto direito do palco */

    Map  *fgMap;            /* BG_A, 1:1 com pos */
    Map  *bgMap;            /* BG_B, interpolado em [farBgLeft, farBgRight] */
} Camera;

/**
 * @brief Liga a camera ao palco. Chame depois dos MAP_create.
 */
void CAMERA_setup(Camera *cam, Map *fgMap, Map *bgMap, const CameraStage *stage);

/**
 * @brief P1 a esquerda e P2 a direita da visao atual (cam.pos).
 */
void CAMERA_spawn(Camera *cam, Player *p1, Player *p2);

/**
 * @brief Um frame: virar no cruzamento, seguir, prender nos cantos, desenhar.
 *
 * Substitui updateFacing + update + constrain + placeSprite.
 */
void CAMERA_tick(Camera *cam, Player *p1, Player *p2);

/**
 * @brief Tremor estilo MK arcade (onda quadrada).
 *
 * @param power  soquinho 2, chute 4, uppercut 8
 */
void CAMERA_shake(Camera *cam, s16 power);

#endif