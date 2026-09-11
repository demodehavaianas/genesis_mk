#include "camera.h"

#define CAM_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CAM_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define BODY_WIDTH_FALLBACK  32

/**
 * @brief Caixa do CORPO em mundo, espelhada se o lutador olha pra esquerda.
 */
static void camera_bodyBoxX(const Player *p, s16 *bodyLeft, s16 *bodyRight)
{
    s16 bodyW  = (s16) p->w;
    s16 pivotX = (s16) p->axisX;

    if (bodyW < 8)
        bodyW = BODY_WIDTH_FALLBACK;

    if (p->direcao < 0)
    {
        *bodyRight = (s16) (p->x + pivotX);
        *bodyLeft  = (s16) (*bodyRight - bodyW);
    }
    else
    {
        *bodyLeft  = (s16) (p->x - pivotX);
        *bodyRight = (s16) (*bodyLeft + bodyW);
    }
}

/**
 * @brief Pivot X de desenho. O SGDK espelha o FRAME inteiro, nao o corpo.
 */
static s16 camera_drawPivotX(const Player *p)
{
    s16 pivotX = (s16) p->axisX;
    s16 frameW;

    if (p->direcao >= 0)
        return pivotX;

    frameW = (p->sprite && p->sprite->definition)
             ? (s16) p->sprite->definition->w
             : (s16) (pivotX + (s16) p->w);

    return (s16) (frameW - pivotX);
}

/**
 * @brief Scroll do BGB: interpola a camera no palco para [farBgLeft, farBgRight].
 */
static s16 camera_farBgX(const Camera *cam)
{
    s32 scrollSpan = (s32) cam->scrollMaxX - (s32) cam->scrollMinX;
    s32 farBgSpan  = (s32) cam->farBgRight - (s32) cam->farBgLeft;
    s32 along;

    if (scrollSpan <= 0 || farBgSpan <= 0)
        return cam->farBgLeft;

    along = (s32) cam->pos.x - (s32) cam->scrollMinX;
    if (along < 0) along = 0;
    if (along > scrollSpan) along = scrollSpan;

    return (s16) ((s32) cam->farBgLeft + (along * farBgSpan) / scrollSpan);
}

static void camera_scrollPlanes(const Camera *cam, bool splitDma)
{
    MAP_scrollTo(cam->fgMap, (u32) cam->pos.x, (u32) cam->pos.y);
    if (cam->bgMap)
    {
        if (splitDma)
            SYS_doVBlankProcess();
        MAP_scrollTo(cam->bgMap, (u32) camera_farBgX(cam), (u32) (cam->pos.y >> 1));
    }
}

/**
 * @brief Vira so quando um cruza o outro. Nao vira ao andar para tras.
 */
static void camera_faceEachOther(Player *p1, Player *p2)
{
    if (p1->x == p2->x)
        return;

    if (p1->x < p2->x)
    {
        p1->direcao =  1;
        p2->direcao = -1;
    }
    else
    {
        p1->direcao = -1;
        p2->direcao =  1;
    }
}

/**
 * @brief Medio X. Trava se cada um ja esta numa beirada oposta da tela.
 */
static void camera_follow(Camera *cam, const Player *p1, const Player *p2, bool applyShake)
{
    const s16 screenW = (s16) VDP_getScreenWidth();
    s16 bodyLeft1, bodyRight1, bodyLeft2, bodyRight2;
    s32 midpointX;
    s32 keepOnScreenMin;
    s32 keepOnScreenMax;
    s16 shakeX = 0;
    s16 shakeY = 0;

    camera_bodyBoxX(p1, &bodyLeft1, &bodyRight1);
    camera_bodyBoxX(p2, &bodyLeft2, &bodyRight2);

    midpointX = (((s32) p1->x + (s32) p2->x) >> 1) - (screenW >> 1);

    /* menor X que ainda deixa o da DIREITA visivel / maior X que deixa o da ESQUERDA */
    keepOnScreenMin = (s32) CAM_MAX(bodyRight1, bodyRight2) - (s32) screenW + CAMERA_SCREEN_MARGIN;
    keepOnScreenMax = (s32) CAM_MIN(bodyLeft1,  bodyLeft2)  - CAMERA_SCREEN_MARGIN;

    if (keepOnScreenMin <= keepOnScreenMax)
        midpointX = clamp(midpointX, keepOnScreenMin, keepOnScreenMax);
    else
        midpointX = cam->pos.x;   /* os dois nas beiradas: camera presa */

    midpointX = clamp(midpointX, (s32) cam->scrollMinX, (s32) cam->scrollMaxX);

    cam->pos.x = (s16) midpointX;
    cam->pos.y = cam->scrollMaxY;

    if (applyShake && cam->shakeTime)
    {
        shakeX = (cam->shakeTime & 1) ? cam->shakePower : (s16) -cam->shakePower;
        shakeY = shakeX >> 1;

        cam->shakeTime--;
        if (((cam->shakeTime & 1) == 0) && (cam->shakePower > 0))
            cam->shakePower--;
        if (cam->shakeTime == 0)
            cam->shakePower = 0;
    }

    cam->pos.x = (s16) clamp((s32) cam->pos.x + shakeX, (s32) cam->scrollMinX, (s32) cam->scrollMaxX);
    cam->pos.y = (s16) clamp((s32) cam->pos.y + shakeY, 0, (s32) cam->scrollMaxY);
}

/**
 * @brief Ring (ringLeft/Right) + paredes da tela (andam com cam.pos).
 */
static void camera_keepInside(const Camera *cam, Player *p)
{
    const s16 screenW = (s16) VDP_getScreenWidth();
    s16 bodyLeft, bodyRight;
    s16 screenLeft, screenRight;

    camera_bodyBoxX(p, &bodyLeft, &bodyRight);

    if (bodyLeft < cam->ringLeft)
    {
        p->x = (s16) (p->x + (cam->ringLeft - bodyLeft));
        camera_bodyBoxX(p, &bodyLeft, &bodyRight);
    }
    if (bodyRight > cam->ringRight)
    {
        p->x = (s16) (p->x + (cam->ringRight - bodyRight));
        camera_bodyBoxX(p, &bodyLeft, &bodyRight);
    }

    screenLeft  = (s16) (cam->pos.x + CAMERA_SCREEN_MARGIN);
    screenRight = (s16) (cam->pos.x + screenW - CAMERA_SCREEN_MARGIN);

    if (bodyLeft < screenLeft)
    {
        p->x = (s16) (p->x + (screenLeft - bodyLeft));
        camera_bodyBoxX(p, &bodyLeft, &bodyRight);
    }
    if (bodyRight > screenRight)
        p->x = (s16) (p->x + (screenRight - bodyRight));
}

static void camera_drawFighter(const Camera *cam, Player *p)
{
    s16 pivotX;

    if (p->sprite == NULL)
        return;

    pivotX = camera_drawPivotX(p);
    SPR_setHFlip(p->sprite, (p->direcao < 0) ? TRUE : FALSE);
    SPR_setPosition(p->sprite,
                    (s16) (p->x - pivotX - cam->pos.x),
                    (s16) (p->y - (s16) p->axisY - cam->pos.y));
}

void CAMERA_setup(Camera *cam, Map *fgMap, Map *bgMap, const CameraStage *stage)
{
    const s16 screenW = (s16) VDP_getScreenWidth();
    const s16 screenH = (s16) VDP_getScreenHeight();

    cam->fgMap      = fgMap;
    cam->bgMap      = bgMap;
    cam->shakePower = 0;
    cam->shakeTime  = 0;

    cam->ringLeft   = stage->ringLeft;
    cam->ringRight  = stage->ringRight;
    cam->farBgLeft  = stage->farBgLeft;
    cam->farBgRight = stage->farBgRight;
    if (cam->farBgRight < cam->farBgLeft)
        cam->farBgRight = cam->farBgLeft;

    cam->scrollMinX = 0;
    cam->scrollMaxX = (s16) (stage->mapWidth  - screenW);
    cam->scrollMaxY = (s16) (stage->mapHeight - screenH);
    if (cam->scrollMaxX < 0) cam->scrollMaxX = 0;
    if (cam->scrollMaxY < 0) cam->scrollMaxY = 0;

    cam->pos.x = clamp(stage->startX, cam->scrollMinX, cam->scrollMaxX);
    cam->pos.y = clamp(stage->startY, 0, cam->scrollMaxY);

    camera_scrollPlanes(cam, TRUE);
}

void CAMERA_spawn(Camera *cam, Player *p1, Player *p2)
{
    const s16 screenW    = (s16) VDP_getScreenWidth();
    const s16 screenH    = (s16) VDP_getScreenHeight();
    const s16 spawnInset = (s16) (screenW >> 2);

    p1->x = (s16) (cam->pos.x + spawnInset);
    p2->x = (s16) (cam->pos.x + screenW - spawnInset);
    p1->y = (s16) (cam->pos.y + screenH - CAMERA_FEET_INSET);
    p2->y = p1->y;

    p1->direcao =  1;
    p2->direcao = -1;

    camera_scrollPlanes(cam, TRUE);
    SYS_doVBlankProcess();
    camera_drawFighter(cam, p1);
    camera_drawFighter(cam, p2);
}

void CAMERA_tick(Camera *cam, Player *p1, Player *p2)
{
    camera_faceEachOther(p1, p2);
    camera_follow(cam, p1, p2, TRUE);
    camera_keepInside(cam, p1);
    camera_keepInside(cam, p2);
    camera_scrollPlanes(cam, FALSE);
    camera_drawFighter(cam, p1);
    camera_drawFighter(cam, p2);
}

void CAMERA_shake(Camera *cam, s16 power)
{
    u8 frames;

    if (power < 1)
        return;
    if (power < cam->shakePower)
        return;

    cam->shakePower = power;
    frames = (u8) (power + (power >> 1));
    if (frames < 2)
        frames = 2;
    cam->shakeTime = frames;
}