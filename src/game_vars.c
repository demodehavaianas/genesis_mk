/**
 * @file game_vars.c
 * @author Nil Obermüller Schaupp a.k.a demodehavaianas (demoniodehavaianas@outlook.com)
 * @brief Definições das variáveis globais do jogo.
 * @version 0.1
 * @date 2026-08-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "game_vars.h"
#include "estruturas.h"

bool debugEnabled;
bool gPodeMover = FALSE;
u8 gRoom;
u8 gAlturaDoPiso;
u16 gInd_tileset;
u16 gBG_Width;
u16 gBG_Height;
s16 gScrollValue;
u32 gFrames;
bool gASG_system;
s8 gClockL = 9;
s8 gClockR = 9;
s8 gClockTimer = 60;

GameLanguage language = BR;
GraphicElement GE[25];
Player player[2];