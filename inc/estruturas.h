#ifndef _ESTRUTURAS_H_
#define _ESTRUTURAS_H_

#include "types.h"
#include "sprite_eng.h"

// Lista de Personagens
enum Fighters
{
  JOHNNY_CAGE,
  KANO,
  RAIDEN,
  LIU_KANG,
  SUBZERO,
  SCORPION,
  SONYA,
  GORO,
  SHANG_TSUNG,
  REPTILE
};

// Lista de 'Salas' para segmentação do jogo
enum GAME_ROOM
{
  TELA_DEMO_INTRO,
  TELA_TITULO,
  TELA_START,
  SELECAO_PERSONAGENS,
  PALACE_GATES
};

enum PLAYER_STATUS
{
  PARADO,
  ABAIXANDO,
  ANDAR_FRENTE,
  ANDAR_TRAS,
  INI_PULO_TRAS,
  INI_PULO_NEUTRO,
  INI_PULO_FRENTE
};

// Linhas de texto e sua posição em tela
typedef struct
{
  const char *text;
  u16 x;
  u16 y;
} TextLine;

typedef struct
{
  Sprite *sprite;
} GraphicElement;

typedef struct
{
  u8 id;
  Sprite *sprite;
  int paleta;
  s16 x;      // posição X do jogador
  s16 y;      // posição Y do jogador
  u8 w;       // Largura do Sprite
  u8 h;       // Altura do Sprite
  u8 axisX;   // Posição X do ponto pivot
  u8 axisY;   // Posição Y do ponto pivot
  s8 direcao; // Direção para onde está olhando (1 - Direita, -1 - Esquerda)
  u16 state;  // Estado atual do jogador
  u8 hSpeed;  // Velocidade Horizontal

  u16 animFrame;      // frame de animação atual
  u16 animFrameTotal; // quantidade total de frames deste estado de animação
  u16 frameTimeAtual; // tempo atual do frame de animação corrente
  u16 frameTimeTotal; // tempo total do frame de animação corrente

  u16 dataAnim[60]; // total de frames disponíveis para cada estado (animação)

  // JOYSTICK
  u8 key_JOY_UP_status;
  u8 key_JOY_DOWN_status;
  u8 key_JOY_LEFT_status;
  u8 key_JOY_RIGHT_status;
  u8 key_JOY_A_status;
  u8 key_JOY_B_status;
  u8 key_JOY_C_status;
  u8 key_JOY_X_status;
  u8 key_JOY_Y_status;
  u8 key_JOY_Z_status;
  u8 key_JOY_START_status;
  u8 key_JOY_MODE_status;
  u8 key_JOY_countdown[10];
  // Key_JOY_status informa se o botao
  // 0 - nao esta apertado
  // 1 - acabou de apertar
  // 2 - mantendo apertado
  // 3 - acabou de soltar
  u8 key_JOY_status[12];
} Player;

extern Player player[2];

#endif