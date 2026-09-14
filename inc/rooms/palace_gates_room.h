#ifndef _PALACE_GATES_H_
#define _PALACE_GATES_H_

#define PALACE_GATES_MAP_W  1008
#define PALACE_GATES_MAP_H   240

/* Limite da tela onde o jogador pode se mover */
#define PALACE_GATES_RING_LEFT   66
#define PALACE_GATES_RING_RIGHT  927

/* spawn dos lutadores */
#define PALACE_GATES_START_X  272
#define PALACE_GATES_START_Y   16

/* parallax BGB (arte 184..516). 270 = ~86 px de viagem */
#define PALACE_GATES_FARBG_LEFT   184
#define PALACE_GATES_FARBG_RIGHT  270

void initPalaceGatesRoom(void);

#endif