#ifndef MKP_FSM_H
#define MKP_FSM_H

#include <genesis.h>

/* Finite State Machine of the fighters (engine-level, character-agnostic).
 * Character-specific specials are dispatched via CharacterOps. */
void fsm_update(void);

#endif