#ifndef GUARD_RANDOMIZER_H
#define GUARD_RANDOMIZER_H

#include "config/randomizer.h"

#if RANDOMIZER_AVAILABLE == TRUE

#include "global.h"
#include "wild_encounter.h"

enum RandomizerContext
{
    RANDOMIZER_CONTEXT_WILD_ENCOUNTER,
    RANDOMIZER_CONTEXT_FIXED_ENCOUNTER,
    RANDOMIZER_CONTEXT_TRAINER_PARTY,
    RANDOMIZER_CONTEXT_STARTER_AND_GIFT,
    RANDOMIZER_CONTEXT_FIELD_ITEM,
    RANDOMIZER_CONTEXT_ABILITY,
    RANDOMIZER_CONTEXT_TM_MOVES,
    RANDOMIZER_CONTEXT_MOVESET,
    RANDOMIZER_CONTEXT_SPECIES_TYPE,
    RANDOMIZER_CONTEXT_NPC_GIFT_ITEM,
    RANDOMIZER_CONTEXT_MEGA_STONE_SUBSET,
    RANDOMIZER_CONTEXT_ELITE_FOUR_ACE,
};

bool32 IsWildRandomizerEnabled(void);
bool32 IsTrainerRandomizerEnabled(void);
bool32 IsStarterAndGiftRandomizerEnabled(void);
bool32 IsFieldItemRandomizerEnabled(void);
bool32 IsAbilityRandomizerEnabled(void);
bool32 IsTMMoveRandomizerEnabled(void);
bool32 IsMovesetRandomizerEnabled(void);

// Returns TRUE if this species is allowed to be a randomizer target (or source).
bool32 IsSpeciesRandomizable(u16 species);

// Returns a replacement for `species` with base stat total within
// RANDOMIZER_BST_LENIENCY_PERCENT of the original
u16 GetRandomizedSpecies(enum RandomizerContext context, u32 data1, u32 data2, u16 species);

u16 RandomizeWildEncounter(u16 species, u32 mapNum, u32 mapGroup, enum WildPokemonArea area, u32 slot);

u16 RandomizeTrainerMon(const void *trainerIdentity, u32 slot, u32 totalMons, u16 species);

u16 RandomizeUniqueMonList(u32 slot, const u16 *originalSpecies, u32 count);

u16 RandomizeStaticGiftSpecies(u32 giftId, u16 originalSpecies);

u16 RandomizeFixedEncounter(u32 encounterId, u16 originalSpecies);

u16 RandomizeEliteFourAce(const void *trainerIdentity, u32 slot, u16 originalSpecies, u16 *outHeldItem);

u16 RandomizeFieldItem(u16 itemId, u32 mapNum, u32 mapGroup, u32 localId);

u16 RandomizeNpcGiftItem(u16 itemId, u32 mapNum, u32 mapGroup);

u16 RandomizeAbility(u16 species, u8 slot, u16 originalAbility);

u16 RandomizeTMMove(u16 tmItem, u16 originalMove);
u16 RandomizeTMItemForMove(u16 move, u16 originalItem);

struct LevelUpMove;
const struct LevelUpMove *RandomizeLevelUpLearnset(u16 species, const struct LevelUpMove *originalLearnset);

// Randomize Shedinja's type
u16 RandomizeSpeciesType(u16 species, u8 slot, u16 originalType);

#endif // RANDOMIZER_AVAILABLE

#endif // GUARD_RANDOMIZER_H
