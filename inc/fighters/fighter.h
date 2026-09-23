#ifndef FIGHTERS_H
#define FIGHTERS_H

#include <genesis.h>
#include "estruturas.h"

#define FIGHTERS_COUNT 10 // quantidade de lutadores no jogo. Ver enum Fighters em estruturas.h
#define MAX_LIFE 160

#define SPRITE_FLAGS (SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | \
                      SPR_FLAG_AUTO_VISIBILITY |              \
                      SPR_FLAG_AUTO_VRAM_ALLOC |              \
                      SPR_FLAG_AUTO_TILE_UPLOAD)

extern const FighterOps johnnyOps;
extern const FighterOps sonyaOps;
extern const FighterOps scorpionOps;
extern const FighterOps subzeroOps;
extern const FighterOps raidenOps;
extern const FighterOps liuKangOps;
extern const FighterOps kanoOps;
//extern const FighterOps goroOps;                      
//extern const FighterOps shangTsungOps;
extern const FighterOps reptileOps;

/*
 * Tabela indexada pelo enum Fighters. Goro/Shang ficam NULL até existirem.
 * Inicializadores: se alguém inserir um lutador no meio do enum,
 * o slot certo continua apontando pro handler certo.
*/ 
extern const FighterOps* const ALL_FIGHTERS[FIGHTERS_COUNT];

#endif // FIGHTERS_H