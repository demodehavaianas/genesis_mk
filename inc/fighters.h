#ifndef FIGHTERS_H
#define FIGHTERS_H

#include <genesis.h>

#define SPRITE_FLAGS (SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | \
                      SPR_FLAG_AUTO_VISIBILITY |              \
                      SPR_FLAG_AUTO_VRAM_ALLOC |              \
                      SPR_FLAG_AUTO_TILE_UPLOAD)

void playerState_Kano(int numPlayer, u16 State);
void playerState_SubZero(int numPlayer, u16 State);
void playerState_Johnny(int numPlayer, u16 State);
void playerState_Raiden(int numPlayer, u16 State);
void playerState_LiuKang(int numPlayer, u16 State);
void playerState_Scorpion(int numPlayer, u16 State);
void playerState_Sonya(int numPlayer, u16 State);
void playerState_Reptile(int numPlayer, u16 State);

#endif // FIGHTERS_H