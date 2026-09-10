/**
 * camera.h — Camera de luta 2D (SGDK 2.11)
 *
 * Os limites do estagio NAO podem ser so #define: camera.c e outro
 * .c e nao ve o palace_gates_room.h. Passe os valores em
 * CAMERA_init / CAMERA_setWalkBounds / CAMERA_setParallax.
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <genesis.h>
#include "estruturas.h"

typedef struct
{
    V2s16 pos;
    s16   shakePower;
    u8    shakeTime;

    s16   walkMinX;     /* onde o SPRITE para (canto esquerdo do ring)  */
    s16   walkMaxX;     /* onde o SPRITE para (canto direito do ring)   */
    s16   camMinX;      /* scroll minimo (em geral 0)                   */
    s16   camMaxX;      /* scroll maximo (mapW - tela)                  */
    s16   camMaxY;

    s16   bgbMinX;      /* parallax: nao mostrar o ceu vazio a esquerda */
    s16   bgbMaxX;      /* parallax: nao mostrar o ceu vazio a direita  */

    Map  *mapA;
    Map  *mapB;
} Camera;

/**
 * @brief Inicializa a camera e guarda as dimensoes reais do mapa.
 *
 * Chame depois dos MAP_create. camMinX = 0, camMaxX = mapW - tela.
 * Os cantos de ANDAR (66 / 912) vao em CAMERA_setWalkBounds.
 *
 * @param cam   Camera
 * @param mapA  Foreground (BG_A)
 * @param mapB  Fundo (BG_B), ou NULL
 * @param mapW  Largura do tilemap em pixels (Palace Gates: 1008)
 * @param mapH  Altura do tilemap em pixels (Palace Gates: 240)
 */
void CAMERA_init(Camera *cam, Map *mapA, Map *mapB, u16 mapW, u16 mapH);

/**
 * @brief Define o ponto inicial da camera neste estagio.
 *
 * Sem isto o spawn centra no ring. Palace Gates: x=272, y=16
 * (canto superior-esquerdo da tela em mundo).
 * Os lutadores nascem DENTRO dessa visao. Depois do round,
 * CAMERA_update volta a seguir o medio.
 *
 * @param cam  Camera
 * @param x    Mundo X do canto da tela
 * @param y    Mundo Y do canto da tela
 */
void CAMERA_setStart(Camera *cam, s16 x, s16 y);

/**
 * @brief Define onde o lutador PARA, independente da borda da tela.
 *
 * minX/maxX sao em MUNDO. Ex.: Palace Gates 66 e 912 (estatuas).
 * A camera continua podendo mostrar 0..mapW; so o sprite e bloqueado.
 *
 * @param cam   Camera
 * @param minX  Canto esquerdo (sprite.left >= minX)
 * @param maxX  Canto direito  (sprite.right <= maxX)
 */
void CAMERA_setWalkBounds(Camera *cam, s16 minX, s16 maxX);

/**
 * @brief Limita o scroll do BG_B para nao revelar o ceu vazio.
 *
 * O BGB rola a metade da camera, depois e preso em [minX, maxX].
 * Palace Gates (arte 184..516): minX=160, maxX=196.
 *
 * @param cam   Camera
 * @param minX  Scroll minimo do BGB
 * @param maxX  Scroll maximo do BGB
 */
void CAMERA_setParallax(Camera *cam, s16 minX, s16 maxX);

/**
 * @brief Vira os dois um para o outro so quando se cruzam.
 *
 * Nao vira ao andar para tras. Chame DEPOIS do movimento,
 * ANTES de CAMERA_update (a caixa de colisao depende da direcao).
 *
 * @param p1  Player 1
 * @param p2  Player 2
 */
void CAMERA_updateFacing(Player *p1, Player *p2);

/**
 * @brief Recoloca a camera no meio dos dois (inicio de round).
 */
void CAMERA_snap(Camera *cam, const Player *p1, const Player *p2);

/**
 * @brief Segue o medio X. Se os dois ja estao nas beiradas opostas
 *        da tela, a camera TRAVA ate alguem andar para o outro.
 *
 * @param cam  Camera
 * @param p1   Player 1
 * @param p2   Player 2
 */
void CAMERA_update(Camera *cam, const Player *p1, const Player *p2);

/**
 * @brief Paredes: cantos do ring (walkMin/Max) + bordas da tela (cam.pos).
 *
 * Chame DEPOIS de CAMERA_update, ANTES de CAMERA_placeSprite.
 */
void CAMERA_constrainPlayer(const Camera *cam, Player *p);

/**
 * @brief Tremor estilo Mortal Kombat arcade (onda quadrada).
 *
 * @param power  soquinho 3, chute 6, uppercut 12
 */
void CAMERA_shake(Camera *cam, s16 power);

/**
 * @brief Posiciona o sprite em tela e aplica HFlip conforme a direcao.
 *
 * Direita:  pos = (x - axisX, y - axisY) - camera.
 * Esquerda: SGDK espelha o FRAME (128 px), pe em (definition->w - axisX).
 */
void CAMERA_placeSprite(const Camera *cam, Player *p);

/**
 * @brief P1 a esquerda, P2 a direita, no centro do walk-bounds.
 */
void CAMERA_spawnPlayers(Camera *cam, Player *p1, Player *p2);

#endif