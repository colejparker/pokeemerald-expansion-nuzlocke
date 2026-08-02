#ifndef GUARD_RANDOMIZER_H
#define GUARD_RANDOMIZER_H

#include "config/randomizer.h"

#if RANDOMIZER_AVAILABLE == TRUE

#include "global.h"
#include "wild_encounter.h"

// Every randomizer roll (species/item/ability substitution) is a deterministic
// function of the save's trainer ID plus a context-specific seed, not a fresh
// reroll on every visit. This is what lets the same wild-grass tile, the same
// trainer, or the same item ball always resolve to the same result on a given
// save, while different saves get different results.
enum RandomizerContext
{
    RANDOMIZER_CONTEXT_WILD_ENCOUNTER,
    RANDOMIZER_CONTEXT_FIXED_ENCOUNTER,
    RANDOMIZER_CONTEXT_TRAINER_PARTY,
    RANDOMIZER_CONTEXT_STARTER_AND_GIFT,
    RANDOMIZER_CONTEXT_FIELD_ITEM,
    RANDOMIZER_CONTEXT_ABILITY,
};

bool32 IsWildRandomizerEnabled(void);
bool32 IsTrainerRandomizerEnabled(void);
bool32 IsStarterAndGiftRandomizerEnabled(void);
bool32 IsFieldItemRandomizerEnabled(void);
bool32 IsAbilityRandomizerEnabled(void);

// Returns TRUE if this species is allowed to be a randomizer target (or source).
// FALSE for placeholder/disabled species, gimmick-only forms (Mega/Gigantamax/
// Totem/Primal/Ultra Burst/Tera-only), and the curated restricted-legendary list.
bool32 IsSpeciesRandomizable(u16 species);

// Returns a replacement for `species` with base stat total within
// RANDOMIZER_BST_LENIENCY_PERCENT of the original, deterministically chosen from
// `context` + `data1` + `data2`. Returns `species` unchanged if it isn't
// randomizable or no substitute could be found.
u16 GetRandomizedSpecies(enum RandomizerContext context, u32 data1, u32 data2, u16 species);

u16 RandomizeWildEncounter(u16 species, u32 mapNum, u32 mapGroup, enum WildPokemonArea area, u32 slot);

// `trainerIdentity` should be the trainer's `const struct Trainer *` pointer.
// It's a stable, unique-per-trainer-entry value baked into the ROM at compile
// time (no numeric trainer ID is threaded through the party-building call
// chain), so it works as a seed component the same way a trainer ID would.
u16 RandomizeTrainerMon(const void *trainerIdentity, u32 slot, u32 totalMons, u16 species);

// Randomizes a fixed small list of species (e.g. the 3 starter choices),
// avoiding giving two slots the same randomized result. `count` must be small
// (bounded by RANDOMIZER_MAX_UNIQUE_LIST_SIZE) since this recomputes the
// whole list's rolls on every call rather than caching them.
u16 RandomizeUniqueMonList(u32 slot, const u16 *originalSpecies, u32 count);

// Randomizes a field item (visible item ball or hidden item) given to the
// player. HMs and key items are never touched (unwinnable-run protection); a
// TM always randomizes to another TM, everything else randomizes among
// non-TM/HM/key items.
u16 RandomizeFieldItem(u16 itemId, u32 mapNum, u32 mapGroup, u32 localId);

// Randomizes a species' ability slot from a broad whitelist. Shedinja and
// Ditto are left untouched (Wonder Guard / Imposter are gameplay-defining).
// Abilities are maintained across an evolution family (every stage rolls the
// same result for a given slot, seeded off the family's base species) EXCEPT
// for Mega Evolutions, which roll independently from a separate, smaller
// "stronger" pool instead. `originalAbility` is returned unchanged if it was
// ABILITY_NONE to begin with, preserving "does this species have a 2nd/3rd
// ability slot at all" checks used throughout battle AI, DexNav, and the
// Ability Capsule/Patch party menu logic.
u16 RandomizeAbility(u16 species, u8 slot, u16 originalAbility);

#endif // RANDOMIZER_AVAILABLE

#endif // GUARD_RANDOMIZER_H
