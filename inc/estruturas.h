/**
 * @file estruturas.h
 * @brief Define as estruturas de dados complexas utilizadas no jogo,
 *        incluindo jogadores, elementos gráficos e configurações de idioma.
 */
#ifndef MKP_ESTRUTURAS_H
#define MKP_ESTRUTURAS_H

#include "maths.h"
#include "types.h"
#include "sprite_eng.h"

/**
 * @brief Enumeração dos idiomas suportados pelo jogo.
 */
typedef enum
{
  EN,
  BR
} GameLanguage;

/**
 * @brief Enumeração dos personagens disponíveis no jogo.
 */
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

/**
 * @brief Enumeração das salas do jogo.
 *        Cada valor representa uma tela ou fase específica do jogo.
 */
enum GAME_ROOM
{
  TELA_DEMO_INTRO,      // Tela de introdução do jogo
  TELA_TITULO,
  TELA_START,           // Tela de Start e Options do jogo
  SELECAO_PERSONAGENS,  // Tela de seleção de personagens
  BONUS_STAGE,          // Tela do Bonus Stage
  PALACE_GATES          // Tela do Palace Gates - Stage 1
};

/**
 * @brief Enumeração dos estados possíveis do jogador.
 */
enum PLAYER_STATUS
{
  PARADO,
  ABAIXANDO,
  ABAIXADO,
  LEVANTANDO,
  ANDAR_PRA_FRENTE,
  ANDAR_PRA_TRAS,
  CORRER,
  VIRAR, // mudar de lado
  BLOQUEIO_EM_PE_INI,
  BLOQUEIO_EM_PE,
  BLOQUEIO_EM_PE_FIM,
  BLOQUEIO_ABAIXADO_INI,
  BLOQUEIO_ABAIXADO,
  BLOQUEIO_ABAIXADO_FIM,
  SOCO_ALTO,          // HP
  SOCO_ALTO_CONTINUO, // HP, HP, HP ...
  SOCO_PERTO,         // HP encostado
  SOCO_BAIXO,         // LP
  SOCO_BAIXO_CONTINUO,// LP, LP, LP ...
  CHUTE_ALTO,         // HK
  CHUTE_BAIXO,        // LK
  CHUTE_PERTO,        // HK encostado
  RASTEIRA,           // <- + LK
  GIRATORIA,          // <- + HK
  GANCHO,             // v + HP
  CHUTE_ABAIXADO,     // v + HK
  INI_PULO_TRAS,
  INI_PULO_NEUTRO,
  FIM_PULO_NEUTRO,
  PULO_NEUTRO_SOLO,
  INI_PULO_FRENTE,
  VOADORA,
  VITORIA,
  ESPECIAL_1,
  ESPECIAL_2,
};

/**
 * @brief Estrutura para representar uma linha de texto e sua posição em tela.
 */
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

/**
 * @brief Estrutura para representar as operações de um lutador usando Strategy Pattern.
 */
typedef struct FighterOps
{
  enum Fighters id;
  void (*set_state)(u8 player, u16 state);
  void (*play_sound)(u8 player, u16 state);
  void (*update_physics)(u8 player);
} FighterOps;

typedef struct
{
  V2s16 pos;
  Sprite *sprite;
} projetil;

/**
 * @brief Estrutura para representar um jogador.
 */
typedef struct
{
  u8 id;
  Sprite *sprite;
  u16 paleta;       // PAL0, PAL1, PAL2, etc.
  s16 x;            // posição X do jogador
  s16 y;            // posição Y do jogador
  u8 w;             // Largura do Sprite
  u8 h;             // Altura do Sprite
  u8 axisX;         // Posição X do ponto pivot
  u8 axisY;         // Posição Y do ponto pivot
  s8 direcao;       // Direção para onde está olhando (1 - Direita, -1 - Esquerda)
  u16 state;        // Estado atual do jogador
  u8 hSpeed;        // Velocidade Horizontal
  s8 vSpeed;        // Velocidade Vertical
  u16 energia;      // Energia do jogador (0 a 160)
  bool selecionado; // TODO:talvez mudar pra ativo pra ser usado futuramente no topo da torre
  projetil especial;// magia 
  const FighterOps *ops;

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

#endif