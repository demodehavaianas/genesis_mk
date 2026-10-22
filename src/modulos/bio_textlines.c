#include "modulos/bio_textlines.h"
#include "audio.h"
#include "sprites.h"
#include "game_vars.h"

// Texto da Bio do Johnny Cage em Português
const TextLine johnnyCageLines_BR[] = {
    {"UM SUPERASTRO DAS ARTES MARCIAIS", 4, 17},
    {"TREINADO POR GRANDES MESTRES DE", 5, 18},
    {"TODO O MUNDO. CAGE USA SEUS", 6, 19},
    {"TALENTOS NAS TELAS DE CINEMA, ELE", 3, 20},
    {"E' O ATUAL CAMPEAO DE BILHETERIA", 4, 21},
    {"E ESTRELA DE FILMES COMO PUNHO DO", 3, 22},
    {"DRAGAO E PUNHO DO DRAGAO II,", 5, 23},
    {"BEM COMO O PREMIADO", 9, 24},
    {"VIOLENCIA SUBITA", 11, 25}
};

// Texto da Bio do Johnny Cage em Inglês
const TextLine johnnyCageLines_EN[] = {
    {"A MARTIAL ARTS SUPERSTAR TRAINED BY", 3, 17},
    {"GREAT MASTERS FROM AROUND THE WORLD.", 2, 18},
    {"CAGE USES HIS TALENTS ON", 7, 19},
    {"THE BIG SCREEN,", 10, 20},
    {"HE IS THE CURRENT BOX-OFFICE CHAMP", 3, 21},
    {"AND STAR OF SUCH MOVIES AS DRAGON", 3, 22},
    {"FIST AND DRAGON FIST II AS WELL", 4, 23},
    {"AS THE AWARD", 13, 24},
    {"WINNING SUDDEN VIOLENCE.", 7, 25},
};

// Texto da Bio do Kano em Português
const TextLine kanoLines_BR[] = {
    {"MERCENARIO, BANDIDO, EXTORSIONISTA,", 3, 17},
    {"LADRAO. KANO VIVE UMA VIDA DE", 5, 18},
    {"CRIMES E INJUSTICA. ELE E' UM", 5, 19},
    {"DEVOTADO MEMBRO DO DRAGAO NEGRO,", 4, 20},
    {"UM GRUPO PERIGOSO DE ASSASSINOS", 3, 21},
    {"lOUCOS, TEMIDOS E RESPEITADOS", 6, 22},
    {"POR TODOS OS CIRCULOS", 9, 23},
    {"INTERNOS DO CRIME.", 11, 24},
};

// Texto da Bio do Kano em Inglês
const TextLine kanoLines_EN[] = {
    {"A MERCENARY, THUG, EXTORTIONIST", 4, 17},
    {"THIEF - KANO LIVES A LIFE OF CRIME", 3, 18},
    {"AND INJUSTICE. HE IS A DEVOTED", 5, 19},
    {"MEMBER OF THE BLACK DRAGON,", 7, 20},
    {"A DANGEROUS GROUP OF CUT-THROAT", 5, 21},
    {"MADMEN FEARED AND RESPECTED", 7, 22},
    {"THROUGHOUT ALL OF CRIME'S", 6, 23},
    {"INNER CIRCLES.", 12, 24},
};

const TextLine liuKangLines_BR[] = {
    {"OUTRORA MEMBRO DA SUPER SECRETA", 5, 18},
    {"SOCIEDADE DO LOTUS BRANCO,", 7, 19},
    {"LIU KANG DEIXOU A ORGANIZACAO", 5, 20},
    {"A FIM DE REPRESENTAR OS", 7, 21},
    {"TEMPLOS SHAOLIN NO TORNEIO.", 6, 22},
    {"KANG E' FIRME EM SUAS CRENCAS", 5, 23},
    {"E DESPREZA SHANG TSUNG", 7, 24},
};

const TextLine liuKangLines_EN[] = {
    {"ONCE A MEMBER OF THE SUPER SECRET", 3, 18},
    {"WHITE LOTUS SOCIETY, LIU KANG LEFT", 3, 19},
    {"THE ORGANIZATION IN ORDER TO", 5, 20},
    {"REPRESENT SHAOLIN TEMPLES IN THE", 3, 21},
    {"TOURNAMENT, KANG IS STRONG IN HIS", 3, 22},
    {"BELIEFS AND DESPISES SHANG TSUNG", 3, 23},
};

const TextLine scorpionLines_BR[] = {
    {"ASSIM COMO SUB-ZERO, O VERDADEIRO", 4, 17},
    {"NOME E ORIGEM DE SCORPION", 8, 18},
    {"NAO SAO CONHECIDOS.", 10, 19},
    {"ELE DEMONSTROU DE TEMPOS EM TEMPOS", 3, 20},
    {"DESCONFIANCA E ODIO EM RELACAO", 5, 21},
    {"A SUB-ZERO. ENTRE NINJAS, ISSO", 4, 22},
    {"GERALMENTE E' UM SINAL DE", 6, 23},
    {"CLAS RIVAIS", 14, 24}};

const TextLine scorpionLines_EN[] = {
    {"LIKE SUB-ZERO, SCORPION'S TRUE", 5, 18},
    {"NAME AND ORIGIN ARE NOT KNOWN", 5, 19},
    {"HE HAS SHOWN FROM TIME TO TIME", 5, 20},
    {"DISTRUST AND HATRED TOWARDS", 7, 21},
    {"SUB-ZERO. BETWEEN NINJAS, THIS", 5, 22},
    {"IS USUALLY A SIGN OF", 11, 23},
    {"OPPOSING CLANS", 13, 24},
};

const TextLine subzeroLines_BR[] = {
    {"O VERDADEIRO NOME OU IDENTIDADE", 4, 18},
    {"DESSE GUERREIRO E' DESCONHECIDA.", 5, 19},
    {"NO ENTANTO COM BASE NAS MARCAS DE", 3, 20},
    {"SEU UNIFORME, ACREDITA-SE QUE", 5, 21},
    {"ELE PERTENCA AOS LIN KUEI,", 6, 22},
    {"UM LENDARIO CLA DE", 9, 23},
    {"NINJAS CHINESES.", 10, 24}};

const TextLine subzeroLines_EN[] = {
    {"THE ACTUAL NAME OR IDENTITY OF", 5, 18},
    {"THIS WARRIOR IS UNKNOWN. HOWEVER", 4, 19},
    {"BASED ON THE MARKINGS OF HIS", 5, 20},
    {"UNIFORM, IT IS BELIEVED HE BELONGS", 3, 21},
    {"TO THE LIN KUEI, A LEGENDARY", 5, 22},
    {"CLAN OF CHINESE NINJA.", 10, 23},
};

const TextLine raidenLines_BR[] = {
    {"O NOME RAIDEN E' NA VERDADE", 5, 18},
    {"O DE UMA DEIDADE CONHECIDA COMO", 4, 19},
    {"DEUS DO TROVAO.", 11, 20},
    {"HA' RUMORES DE QUE ELE RECEBEU", 4, 21},
    {"UM CONVITE PESSOAL", 9, 22},
    {"DO PROPRIO SHANG TSUNG", 7, 23},
    {"E ASSUMIU A FORMA HUMANA PARA", 4, 24},
    {"COMPETIR NO TORNEIO", 9, 25}};

const TextLine raidenLines_EN[] = {
    {"THE NAME RAIDEN IS ACTUALLY THAT OF", 3, 18},
    {"A DEITY KNOWN AS THE THUNDER GOD.", 4, 19},
    {"IT IS RUMORED HE RECEIVED A", 7, 20},
    {"PERSONAL INVITATION BY SHANG TSUNG", 3, 21},
    {"HIMSELF AND TOOK THE FORM OF A", 5, 22},
    {"HUMAN TO COMPETE IN THE TOURNAMENT", 3, 23},
};

const TextLine sonyaLines_BR[] = {
    {"SONYA E MEMBRA DE UMA ELITE DA", 5, 17},
    {"UNIDADE DE FORCAS ESPECIAIS DOS EUA", 3, 18},
    {"SUA EQUIPE ESTAVA NA PISTA DA", 6, 19},
    {"ORGANIZACAO DRAGAO NEGRO DE KANO.", 4, 20},
    {"ELES OS SEGUIRAM ATE UMA", 7, 21},
    {"ILHA INEXPLORADA ONDE FORAM", 5, 22},
    {"EMBOSCADOS PELO EXERCITO PESSOAL", 4, 23},
    {"DE SHANG TSUNG.", 13, 24}};

const TextLine sonyaLines_EN[] = {
    {"SONYA IS A MEMBER OF A TOP U.S.", 5, 18},
    {"SPECIAL FORCES UNIT, HER TEAM WAS", 4, 19},
    {"HOT ON THE TRAIL OF KANO'S BLACK", 5, 20},
    {"DRAGON ORGANIZATION. THEY", 8, 21},
    {"FOLLOWED THEM TO AN UNCHARTED", 6, 22},
    {"ISLAND WHERE THEY WERE AMBUSHED", 5, 23},
    {"BY SHANG TSUNG'S PERSONAL ARMY.", 5, 24},
};

const TextLine goroLines_BR[] = {
    {"UM DRAGAO MEIO HUMANO DE 2.000 ANOS", 3, 7},
    {"GORO PERMANECE INVENCIVEL NOS ULTIMOS", 2, 9},
    {"500 ANOS. ELE CONQUISTOU O TITULO DE", 2, 11},
    {"GRANDE CAMPEAO AO DERROTAR KUNG LAO,", 3, 13},
    {"UM GUERREIRO MONGE SHAOLIN. FOI", 3, 15},
    {"DURANTE ESSE PERIODO QUE O TORNEIO", 2, 17},
    {"CAIU NAS MAOS DE SHANG TSUNG", 5, 19},
    {"E FOI CORROMPIDO", 12, 21},
};

const TextLine goroLines_EN[] = {
    {"A 2.000 YEAR OLD HALF HUMAN DRAGON", 3, 7},
    {"GORO REMAINS UNDERFEATED FOR THE PAST", 2, 9},
    {"500 YEARS. HE WON THE TITLE OF GRAND", 2, 11},
    {"CHAMPION BY DEFEATING KUNG LAO, A", 3, 13},
    {"SHAOLIN FIGHTING MONK. IT WAS DURING", 2, 15},
    {"THIS PERIOD THAT TOURNAMENT FELL", 2, 17},
    {"INTO SHANG TSUNG'S HANDS AND WAS", 4, 19},
    {"CORRUPTED", 15, 21},
};

static const BioData fighterBios[] = {
    {&jc_bio,       &jc_name,       loc_jc,       sizeof(loc_jc),       johnnyCageLines_EN, sizeof(johnnyCageLines_EN) / sizeof(TextLine), johnnyCageLines_BR, sizeof(johnnyCageLines_BR) / sizeof(TextLine)},
    {&kano_bio,     &kano_name,     loc_kano,     sizeof(loc_kano),     kanoLines_EN,       sizeof(kanoLines_EN) / sizeof(TextLine),       kanoLines_BR,       sizeof(kanoLines_BR) / sizeof(TextLine)},
    {&raiden_bio,   &raiden_name,   loc_raiden,   sizeof(loc_raiden),   raidenLines_EN,     sizeof(raidenLines_EN) / sizeof(TextLine),     raidenLines_BR,     sizeof(raidenLines_BR) / sizeof(TextLine)},
    {&liu_kang_bio, &liukang_name,  loc_liu_kang, sizeof(loc_liu_kang), liuKangLines_EN,    sizeof(liuKangLines_EN) / sizeof(TextLine),    liuKangLines_BR,    sizeof(liuKangLines_BR) / sizeof(TextLine)},
    {&subzero_bio,  &subzero_name,  loc_suzero,   sizeof(loc_suzero),   subzeroLines_EN,    sizeof(subzeroLines_EN) / sizeof(TextLine),    subzeroLines_BR,    sizeof(subzeroLines_BR) / sizeof(TextLine)},
    {&scorpion_bio, &scorpion_name, loc_scorpion, sizeof(loc_scorpion), scorpionLines_EN,   sizeof(scorpionLines_EN) / sizeof(TextLine),   scorpionLines_BR,   sizeof(scorpionLines_BR) / sizeof(TextLine)},
    {&sonya_bio,    &sonya_name,    loc_sonya,    sizeof(loc_sonya),    sonyaLines_EN,      sizeof(sonyaLines_EN) / sizeof(TextLine),      sonyaLines_BR,      sizeof(sonyaLines_BR) / sizeof(TextLine)},
    {NULL,          NULL,           NULL,         NULL,                 goroLines_EN,       sizeof(goroLines_EN) / sizeof(TextLine),       goroLines_BR,       sizeof(goroLines_BR) / sizeof(TextLine)}
};

// retorna o BioData correto baseado no lutador
const BioData* getFighterBio(u16 fighterIndex)
{
    return &fighterBios[fighterIndex];
}