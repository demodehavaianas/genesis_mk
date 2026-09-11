#ifndef _PALACE_GATES_H_
#define _PALACE_GATES_H_

#define PALACE_GATES_MAP_W  1008
#define PALACE_GATES_MAP_H   240

/* aneis: onde o sprite PARA (mundo), nao a borda da tela */
#define PALACE_GATES_RING_LEFT   66
#define PALACE_GATES_RING_RIGHT  912

/* spawn: canto da tela em mundo */
#define PALACE_GATES_START_X  272
#define PALACE_GATES_START_Y   16

/* parallax BGB (arte 184..516). 270 = ~86 px de viagem, flui */
#define PALACE_GATES_FARBG_LEFT   184
#define PALACE_GATES_FARBG_RIGHT  270

void initPalaceGatesRoom(void);

#endif