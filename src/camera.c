#include "camera.h"

#define CAM_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CAM_MAX(a, b) (((a) > (b)) ? (a) : (b))

/**
 * @brief Caixa do CORPO em mundo, espelhada se direcao < 0.
 */
static void camera_boxX(const Player *p, s16 *left, s16 *right)
{
    s16 w = (s16)p->w;
    s16 ax = (s16)p->axisX;

    if (w < 8)
        w = 32;

    if (p->direcao < 0)
    {
        *right = (s16)(p->x + ax);
        *left = (s16)(*right - w);
    }
    else
    {
        *left = (s16)(p->x - ax);
        *right = (s16)(*left + w);
    }
}

/**
 * @brief axisX de DESENHO. HFlip espelha o frame inteiro, nao o corpo.
 */
static s16 camera_drawAxisX(const Player *p)
{
    s16 ax = (s16)p->axisX;
    s16 frameW;

    if (p->direcao >= 0)
        return ax;

    frameW = (p->sprite && p->sprite->definition)
                 ? (s16)p->sprite->definition->w
                 : (s16)(ax + (s16)p->w);

    return (s16)(frameW - ax);
}

/**
 * @brief Medio X. Se a dupla ja ocupa as duas beiradas, nao mexe.
 */
static void camera_follow(Camera *cam, const Player *p1, const Player *p2, bool applyShake)
{
    const s16 screenW = (s16)VDP_getScreenWidth();
    s16 l1, r1, l2, r2;
    s32 wantX, keepMin, keepMax, camMin, camMax;
    s16 sx = 0;
    s16 sy = 0;

    camera_boxX(p1, &l1, &r1);
    camera_boxX(p2, &l2, &r2);

    wantX = (((s32)p1->x + (s32)p2->x) >> 1) - (screenW >> 1);

    camMin = (s32)cam->camMinX;
    camMax = (s32)cam->camMaxX;
    if (camMax < camMin)
        camMax = camMin;

    keepMin = (s32)CAM_MAX(r1, r2) - (s32)screenW + 8;
    keepMax = (s32)CAM_MIN(l1, l2) - 8;

    if (keepMin <= keepMax)
    {
        /* os dois cabem: segue o medio, preso nas beiradas da dupla */
        wantX = clamp(wantX, keepMin, keepMax);
    }
    else
    {
        /* A num canto da TELA e B no outro: camera PRESA.
         * So anda de novo quando alguem fecha a distancia
         * (keepMin volta a ser <= keepMax). */
        wantX = cam->pos.x;
    }

    wantX = clamp(wantX, camMin, camMax);

    cam->pos.x = (s16)wantX;
    cam->pos.y = cam->camMaxY;

    if (applyShake && cam->shakeTime)
    {
        sx = (cam->shakeTime & 1) ? cam->shakePower : (s16)-cam->shakePower;
        sy = sx >> 1;

        cam->shakeTime--;
        if (((cam->shakeTime & 1) == 0) && (cam->shakePower > 0))
            cam->shakePower--;
        if (cam->shakeTime == 0)
            cam->shakePower = 0;
    }

    cam->pos.x = (s16)clamp((s32)cam->pos.x + sx, camMin, camMax);
    cam->pos.y = (s16)clamp((s32)cam->pos.y + sy, 0, (s32)cam->camMaxY);
}

/**
 * @brief Scroll do BGB: interpola cam.pos.x em [camMin, camMax]
 *        para [bgbMin, bgbMax]. Sem isto o clamp(cam/2) congela
 *        nas pontas e dispara no centro (o "salto").
 */
static s16 camera_bgbX(const Camera *cam)
{
    s32 spanCam = (s32)cam->camMaxX - (s32)cam->camMinX;
    s32 spanBgb = (s32)cam->bgbMaxX - (s32)cam->bgbMinX;
    s32 t;

    if (spanCam <= 0)
        return cam->bgbMinX;

    t = (s32)cam->pos.x - (s32)cam->camMinX;
    if (t < 0)
        t = 0;
    if (t > spanCam)
        t = spanCam;

    return (s16)((s32)cam->bgbMinX + (t * spanBgb) / spanCam);
}

/**
 * @brief Scroll BG_A 1:1. BG_B interpolado no intervalo de parallax.
 */
static void camera_scroll(const Camera *cam, bool forceVBlank)
{
    MAP_scrollTo(cam->mapA, (u32)cam->pos.x, (u32)cam->pos.y);
    if (cam->mapB)
    {
        if (forceVBlank)
            SYS_doVBlankProcess();
        MAP_scrollTo(cam->mapB, (u32)camera_bgbX(cam), (u32)(cam->pos.y >> 1));
    }
}

void CAMERA_init(Camera *cam, Map *mapA, Map *mapB, u16 mapW, u16 mapH)
{
    const s16 screenW = (s16)VDP_getScreenWidth();
    const s16 screenH = (s16)VDP_getScreenHeight();

    cam->mapA = mapA;
    cam->mapB = mapB;
    cam->shakePower = 0;
    cam->shakeTime = 0;

    cam->walkMinX = 0;
    cam->walkMaxX = (s16)mapW;
    cam->camMinX = 0;
    cam->camMaxX = (s16)(mapW - screenW);
    cam->camMaxY = (s16)(mapH - screenH);
    if (cam->camMaxX < 0)
        cam->camMaxX = 0;
    if (cam->camMaxY < 0)
        cam->camMaxY = 0;

    /* default: BGB rola a metade, preso para nao sair do tilemap */
    cam->bgbMinX = 0;
    cam->bgbMaxX = cam->camMaxX >> 1;

    cam->pos.x = cam->camMinX;
    cam->pos.y = cam->camMaxY;

    MAP_scrollTo(mapA, (u32)cam->pos.x, (u32)cam->pos.y);
    if (mapB)
    {
        SYS_doVBlankProcess();
        MAP_scrollTo(mapB, (u32)cam->bgbMinX, (u32)(cam->pos.y >> 1));
    }
}

void CAMERA_setStart(Camera *cam, s16 x, s16 y)
{
    cam->pos.x = clamp(x, cam->camMinX, cam->camMaxX);
    cam->pos.y = clamp(y, 0, cam->camMaxY);
}

void CAMERA_setWalkBounds(Camera *cam, s16 minX, s16 maxX)
{
    cam->walkMinX = minX;
    cam->walkMaxX = maxX;
}

void CAMERA_setParallax(Camera *cam, s16 minX, s16 maxX)
{
    cam->bgbMinX = minX;
    cam->bgbMaxX = maxX;
    if (cam->bgbMaxX < cam->bgbMinX)
        cam->bgbMaxX = cam->bgbMinX;
}

void CAMERA_updateFacing(Player *p1, Player *p2)
{
    if (p1->x == p2->x)
        return;

    if (p1->x < p2->x)
    {
        p1->direcao = 1;
        p2->direcao = -1;
    }
    else
    {
        p1->direcao = -1;
        p2->direcao = 1;
    }
}

void CAMERA_snap(Camera *cam, const Player *p1, const Player *p2)
{
    camera_follow(cam, p1, p2, FALSE);
    camera_scroll(cam, TRUE);
    SYS_doVBlankProcess();
}

void CAMERA_update(Camera *cam, const Player *p1, const Player *p2)
{
    camera_follow(cam, p1, p2, TRUE);
    camera_scroll(cam, FALSE);
}

void CAMERA_constrainPlayer(const Camera *cam, Player *p)
{
    const s16 screenW = (s16)VDP_getScreenWidth();
    s16 left, right;
    s16 viewL, viewR;

    camera_boxX(p, &left, &right);

    /* cantos do RING (66 / 912) — independentes da tela */
    if (left < cam->walkMinX)
    {
        p->x = (s16)(p->x + (cam->walkMinX - left));
        camera_boxX(p, &left, &right);
    }

    if (right > cam->walkMaxX)
    {
        p->x = (s16)(p->x + (cam->walkMaxX - right));
        camera_boxX(p, &left, &right);
    }

    /* bordas da TELA — andam com cam.pos */
    viewL = (s16)(cam->pos.x + 8);
    viewR = (s16)(cam->pos.x + screenW - 8);

    if (left < viewL)
    {
        p->x = (s16)(p->x + (viewL - left));
        camera_boxX(p, &left, &right);
    }

    if (right > viewR)
        p->x = (s16)(p->x + (viewR - right));
}

void CAMERA_shake(Camera *cam, s16 power)
{
    u8 duration;

    if (power < 1)
        return;
    if (power < cam->shakePower)
        return;

    cam->shakePower = power;
    duration = (u8)(power + (power >> 1));
    if (duration < 2)
        duration = 2;
    cam->shakeTime = duration;
}

void CAMERA_placeSprite(const Camera *cam, Player *p)
{
    s16 ax;

    if (p->sprite == NULL)
        return;

    ax = camera_drawAxisX(p);

    SPR_setHFlip(p->sprite, (p->direcao < 0) ? TRUE : FALSE);
    SPR_setPosition(p->sprite,
                    (s16)(p->x - ax - cam->pos.x),
                    (s16)(p->y - (s16)p->axisY - cam->pos.y));
}

void CAMERA_spawnPlayers(Camera *cam, Player *p1, Player *p2)
{ /*
     const s16 screenW = (s16)VDP_getScreenWidth();
     const s16 screenH = (s16)VDP_getScreenHeight();
     const s16 dist = (s16)(screenW >> 2);
     const s16 ringMid = (s16)((cam->walkMinX + cam->walkMaxX) >> 1);

     p1->x = (s16)(ringMid - dist);
     p2->x = (s16)(ringMid + dist);
     p1->y = (s16)(cam->camMaxY + screenH - 8);
     p2->y = p1->y;

     p1->direcao = 1;
     p2->direcao = -1;

     CAMERA_snap(cam, p1, p2);
     CAMERA_placeSprite(cam, p1);
     CAMERA_placeSprite(cam, p2);*/
    const s16 screenW = (s16)VDP_getScreenWidth();
    const s16 screenH = (s16)VDP_getScreenHeight();
    const s16 dist = (s16)(screenW >> 2);

    /* visao ja esta em cam.pos (setStart ou o default do init) */
    p1->x = (s16)(cam->pos.x + dist);
    p2->x = (s16)(cam->pos.x + screenW - dist);
    p1->y = (s16)(cam->pos.y + screenH - 8);
    p2->y = p1->y;

    p1->direcao = 1;
    p2->direcao = -1;

    camera_scroll(cam, TRUE);
    SYS_doVBlankProcess();
    CAMERA_placeSprite(cam, p1);
    CAMERA_placeSprite(cam, p2);
}