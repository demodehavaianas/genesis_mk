#include "modulos/typewriter_printer.h"

void typewriterEffect(const char *text, u16 x, u16 y, u16 delay, VDPPlane plane, u16 numPal)
{
    u16 len = strlen(text);
    for (u16 i = 0; i < len; i++)
    {
        VDP_setTileMapXY(plane, TILE_ATTR_FULL(numPal, TRUE, FALSE, FALSE, TILE_FONT_INDEX + text[i] - ' '), x + i, y);
        SYS_doVBlankProcess(); // Espera o V-Blank

        if (delay != 0)
        {
            for (u16 j = 0; j < delay; j++)
            {
                SYS_doVBlankProcess(); // Atraso adicional
            }
        }
    }
}

// Função para escrever todas as linhas de texto de uma vez
void typewriterWriteAllLines(const TextLine *lines, u16 numLines, VDPPlane plane, u16 numPal)
{
    for (u16 i = 0; i < numLines; i++)
    {
        const char *text = lines[i].text;
        u16 x = lines[i].x;
        u16 y = lines[i].y;
        u16 len = strlen(text);
        for (u16 j = 0; j < len; j++)
        {
            VDP_setTileMapXY(plane, TILE_ATTR_FULL(numPal, TRUE, FALSE, FALSE, TILE_FONT_INDEX + text[j] - ' '), x + j, y);
        }
    }
}