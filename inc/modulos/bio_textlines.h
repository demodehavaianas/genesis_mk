#ifndef _BIO_TEXTLINES_H_
#define _BIO_TEXTLINES_H_

#include <genesis.h>
#include "estruturas.h"

typedef struct
{
    const SpriteDefinition *bio;  // sprites da apresentação da biografia do personagem
    const SpriteDefinition *name; // sprite do nome do personagem
    const u8 *voice;              // locutor dizendo o nome
    const u16 voice_size;         // tamanho do audio do locutor
    const TextLine *lines_EN;     // textos da biografia em inglês
    const u16 num_lines_en;       // quantidade de linhas do texto em EN
    const TextLine *lines_BR;     // textos da biografia em Português
    const u16 num_lines_br;       // quantidade de linhas do texto em BR
} BioData;

const BioData* getFighterBio(u16 fighterIndex);

#endif