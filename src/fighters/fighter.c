#include "fighters/fighter.h"

const FighterOps* const ALL_FIGHTERS[FIGHTERS_COUNT] = {
    [JOHNNY_CAGE]   = &johnnyOps,
    [KANO]          = &kanoOps,
    [RAIDEN]        = &raidenOps,
    [LIU_KANG]      = &liuKangOps,
    [SUBZERO]       = &subzeroOps,
    [SCORPION]      = &scorpionOps,
    [SONYA]         = &sonyaOps,
    [GORO]          = NULL,//&goroOps,
    [SHANG_TSUNG]   = NULL,//&shangTsungOps,
    [REPTILE]       = &reptileOps
};