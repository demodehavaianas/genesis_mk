#ifndef _PALACE_GATES_H_
#define _PALACE_GATES_H_

// define a largura e altura do mapa em pixels
#define MAP_WIDTH 1008
#define MAP_HEIGHT 240

/* cantos do cenário independentes.
Ajuste se a arte tiver parede/estatua. */
#define CAMERA_STAGE_MIN_X 66
#define CAMERA_STAGE_MAX_X 912

/* canto superior-esquerdo da tela no spawn deste estagio */
#define CAMERA_START_X  272
#define CAMERA_START_Y   16

/*
 * Arte do BGB so existe em x=184 até 516.
 * Scroll do BGB e cam.x/2, preso aqui para a montanha nao
 * abrir ceu vazio a esquerda nem sumir a direita (arcade).
 */
#define CAMERA_BGB_MIN_X  184
#define CAMERA_BGB_MAX_X  270

void initPalaceGatesRoom(void);

#endif