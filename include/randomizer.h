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
    RANDOMIZER_CONTEXT_TM_MOVES,
    RANDOMIZER_CONTEXT_MOVESET,
    RANDOMIZER_CONTEXT_SPECIES_TYPE,
    RANDOMIZER_CONTEXT_NPC_GIFT_ITEM,
    RANDOMIZER_CONTEXT_MEGA_STONE_SUBSET,
};

bool32 IsWildRandomizerEnabled(void);
bool32 IsTrainerRandomizerEnabled(void);
bool32 IsStarterAndGiftRandomizerEnabled(void);
bool32 IsFieldItemRandomizerEnabled(void);
bool32 IsAbilityRandomizerEnabled(void);
bool32 IsTMMoveRandomizerEnabled(void);
bool32 IsMovesetRandomizerEnabled(void);

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
// player. HMs and key items are never touched (unwinnable-run protection).
// Everything else - including TMs - draws from one shared pool (the curated
// whitelist plus the TM01-TM50 range), so a TM can turn into any regular
// item and any regular item can turn into a TM; which move that TM slot
// teaches is a separate roll, unaffected by this.
u16 RandomizeFieldItem(u16 itemId, u32 mapNum, u32 mapGroup, u32 localId);

// Randomizes an item an NPC hands over directly via the `giveitem` script
// command (Std_ObtainItem), on the same toggle, exclusions, and combined
// whitelist+TM pool as RandomizeFieldItem. Seeded off the map the script
// runs on plus the original item, since script gifts don't have a map
// object's local id to key off of the way item balls do. Doesn't touch
// `finditem`/Std_FindItem (item balls, hidden items), which already go
// through RandomizeFieldItem upstream of the pickup script.
u16 RandomizeNpcGiftItem(u16 itemId, u32 mapNum, u32 mapGroup);

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

// Shuffles which of the 50 TM-eligible moves each TM slot (TM01-TM50) teaches
// on this save. The pool of 50 moves doesn't change, only which slot number
// teaches which one of them (HMs and non-TM-eligible moves are untouched).
// `GetItemTMHMMoveId`/`GetTMHMItemIdFromMoveId` in include/item.h route
// through these, so every caller — teaching a TM, describing what a TM
// teaches, and CanLearnTeachableMove's "is this move mapped to some TM/HM
// slot" check — automatically sees the shuffled mapping with no other changes.
u16 RandomizeTMMove(u16 tmItem, u16 originalMove);
u16 RandomizeTMItemForMove(u16 move, u16 originalItem);

// Synthesizes a RANDOMIZER_MOVESET_SIZE-move level-up learnset for `species`:
// 7 STAB-eligible damaging moves, 7 non-STAB damaging moves, and 7 status
// moves, interleaved across a fixed level curve (1 up to 63) with each of the
// two damaging groups sorted so power increases with level. Returns
// `originalLearnset` unchanged when the feature is off. The returned pointer
// is only valid until the next call (single shared buffer - every caller
// fully consumes one species' learnset before requesting another's).
struct LevelUpMove;
const struct LevelUpMove *RandomizeLevelUpLearnset(u16 species, const struct LevelUpMove *originalLearnset);

// gameplan.md: "Shedinja always has Wonder Guard, but its type is
// randomized." Scoped to just Shedinja (not a general type randomizer);
// piggybacks on the ability-randomizer toggle since gameplan.md nests this
// under that same bullet. Both type slots are rolled independently from the
// 18 real types, so Shedinja may end up monotype or dual-type each save.
u16 RandomizeSpeciesType(u16 species, u8 slot, u16 originalType);

#endif // RANDOMIZER_AVAILABLE

#endif // GUARD_RANDOMIZER_H
