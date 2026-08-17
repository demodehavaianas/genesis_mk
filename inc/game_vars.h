/**
 *  \file game_vars.h
 *  \brief Todas as varíaveis globais usadas no jogo
 *  \author Nil Obermüller Schaupp a.k.a demodehavaianas (demoniodehavaianas@outlook.com)
 *  \date 11/2025
 *
 * Mortal Kombat Plus - Game's global vars.
 */
#ifndef GAME_VARS_H
#define GAME_VARS_H

#include <genesis.h>
#include "estruturas.h"

extern bool debugEnabled; // habilita ou não debug
extern bool gPodeMover;   // Desabilita os inputs dos players em momentos especificos (inicio do Round por exemplo)
extern u8 gRoom;          // Sala atual do jogo
extern u8 gAlturaDoPiso;  // Altura do chão (posição Y dos personagens)
extern u16 gInd_tileset;  // Variable used to load background data
extern u16 gBG_Width;     // Largura do Cenario em pixels
extern u16 gBG_Height;    // Altura do Cenario em pixels
extern s16 gScrollValue;  // Scrolling de Cenario
extern u32 gFrames;       // Frame Counter
extern bool gASG_system;
extern s8 gClockL;     // Digito esquerdo do Relogio
extern s8 gClockR;     // Digito direito do Relogio
extern s8 gClockTimer; // relógio atualizado a cada segundo

extern GameLanguage language; // definição da linguagem do sistema
extern GraphicElement GE[25]; // Vetor global de elementos gráficos (GE)
extern Player player[2];      // Vetor global de jogadores (Player 1 e Player 2)

#endif
