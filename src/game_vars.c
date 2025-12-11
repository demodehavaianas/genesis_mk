#include "game_vars.h"
#include "estruturas.h"

// ===============================
// Definições das variáveis globais
// ===============================

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
s16 scrollOffset = 0;
s16 scrollValues[48];

GameLanguage language = BR;
GraphicElement GE[25];
Player player[2];