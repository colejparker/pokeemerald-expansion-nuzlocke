#include "global.h"
#include "randomizer.h"

#if RANDOMIZER_AVAILABLE == TRUE

#include "event_data.h"
#include "item.h"
#include "move.h"
#include "new_game.h"
#include "pokemon.h"
#include "random.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/tms_hms.h"

bool32 IsWildRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_WILD_MON);
}

bool32 IsTrainerRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_TRAINER_MON);
}

bool32 IsStarterAndGiftRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_STARTER_AND_GIFT_MON);
}

bool32 IsFieldItemRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_FIELD_ITEMS);
}

bool32 IsAbilityRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_ABILITIES);
}

bool32 IsTMMoveRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_TM_MOVES);
}

bool32 IsMovesetRandomizerEnabled(void)
{
    return FlagGet(RANDOMIZER_FLAG_MOVESETS);
}

// Whole species families excluded regardless of which form is passed in.
// gameplan.md lists these by family name with no form qualifier.
static const u16 sRestrictedLegendaryFamilies[] =
{
    SPECIES_ARCEUS_NORMAL,
    SPECIES_KYUREM,
    SPECIES_ZACIAN,
    SPECIES_ZAMAZENTA,
    SPECIES_GIRATINA_ALTERED,
    SPECIES_NECROZMA,
    SPECIES_SOLGALEO,
    SPECIES_LUNALA,
    SPECIES_SLAKING,
};

// Specific species/forms excluded; other forms in the same family (if any,
// e.g. base Calyrex, Hoopa Confined) are left randomizable.
static const u16 sRestrictedLegendarySpecies[] =
{
    SPECIES_ETERNATUS,
    SPECIES_MEWTWO,
    SPECIES_LUGIA,
    SPECIES_HO_OH,
    SPECIES_RAYQUAZA,
    SPECIES_DIALGA,
    SPECIES_PALKIA,
    SPECIES_RESHIRAM,
    SPECIES_ZEKROM,
    SPECIES_HOOPA_UNBOUND,
    SPECIES_CALYREX_ICE,
    SPECIES_CALYREX_SHADOW,
    SPECIES_KYOGRE,
    SPECIES_GROUDON,
    SPECIES_REGIGIGAS,
    SPECIES_KORAIDON,
    SPECIES_MIRAIDON,
};

static bool32 IsRestrictedLegendary(u16 species)
{
    u16 baseSpecies = GET_BASE_SPECIES_ID(species);
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sRestrictedLegendaryFamilies); i++)
    {
        if (baseSpecies == sRestrictedLegendaryFamilies[i])
            return TRUE;
    }
    for (i = 0; i < ARRAY_COUNT(sRestrictedLegendarySpecies); i++)
    {
        if (species == sRestrictedLegendarySpecies[i])
            return TRUE;
    }
    return FALSE;
}

bool32 IsSpeciesRandomizable(u16 species)
{
    const struct SpeciesInfo *info;

    if (species == SPECIES_NONE || species >= NUM_SPECIES || !IsSpeciesEnabled(species))
        return FALSE;

    info = &gSpeciesInfo[species];
    if (info->isMegaEvolution || info->isGigantamax || info->isTotem
     || info->isPrimalReversion || info->isUltraBurst || info->isTeraForm
     || info->cannotBeTraded)
        return FALSE;

    if (IsRestrictedLegendary(species))
        return FALSE;

    return TRUE;
}

// Every randomizer roll is seeded from the save's OT id (visible + secret,
// giving a stable per-save seed with no extra save data needed) mixed with a
// context tag and caller-supplied data, so the same input always resolves the
// same way on a given save.
static rng_value_t GetRandomizerSeed(enum RandomizerContext context, u32 data1, u32 data2)
{
    u32 seed = GetTrainerId(gSaveBlock2Ptr->playerTrainerId);
    seed = seed * 2654435761u + (u32)context;
    seed = (seed ^ data1) * 2654435761u;
    seed ^= data2;
    return LocalRandomSeed(seed);
}

// BST-similarity species table: every randomizable species, sorted ascending
// by base stat total, built once and cached for the rest of the session.
// Sorting (rather than a linear scan per roll) keeps "find every species
// within +/-10% BST" a pair of O(log n) binary searches instead of an O(n)
// scan on every single encounter/trainer mon/item pickup.
struct BstTableEntry
{
    u16 species;
    u16 bst;
};

EWRAM_DATA static struct BstTableEntry sBstTable[NUM_SPECIES] = {0};
EWRAM_DATA static u16 sBstTableCount = 0;
EWRAM_DATA static bool8 sBstTableBuilt = FALSE;

static void SwapBstEntries(u16 indexA, u16 indexB)
{
    struct BstTableEntry temp = sBstTable[indexA];
    sBstTable[indexA] = sBstTable[indexB];
    sBstTable[indexB] = temp;
}

static inline u16 LeftChildIndex(u16 index)
{
    return 2 * index + 1;
}

// Standard heap sort. Needed instead of a simpler insertion sort because this
// runs over every randomizable species (a four-digit count once forms are
// counted) and an O(n^2) sort would be a very noticeable hitch on GBA hardware.
static void HeapSortBstTable(void)
{
    u16 start, end, root;

    if (sBstTableCount < 2)
        return;

    start = sBstTableCount / 2;
    end = sBstTableCount - 1;

    while (end > 1)
    {
        if (start > 0)
        {
            start = start - 1;
        }
        else
        {
            end = end - 1;
            SwapBstEntries(end, 0);
        }
        root = start;
        while (LeftChildIndex(root) < end)
        {
            u16 child = LeftChildIndex(root);
            if (child + 1 < end && sBstTable[child].bst < sBstTable[child + 1].bst)
                child = child + 1;
            if (sBstTable[root].bst < sBstTable[child].bst)
            {
                SwapBstEntries(root, child);
                root = child;
            }
            else
                break;
        }
    }
}

static void BuildBstTable(void)
{
    u16 species;

    sBstTableCount = 0;
    for (species = 1; species < NUM_SPECIES; species++)
    {
        if (!IsSpeciesRandomizable(species))
            continue;
        sBstTable[sBstTableCount].species = species;
        sBstTable[sBstTableCount].bst = (u16)GetSpeciesBaseStatTotal(species);
        sBstTableCount++;
    }

    HeapSortBstTable();
    sBstTableBuilt = TRUE;
}

// Finds [*start, *end) in sBstTable covering every entry with bst in [minBst, maxBst].
static void GetBstRangeIndices(u16 minBst, u16 maxBst, u16 *start, u16 *end)
{
    u16 index, leftBound, rightBound;

    leftBound = 0;
    rightBound = sBstTableCount;
    while (leftBound < rightBound)
    {
        index = (leftBound + rightBound) / 2;
        if (sBstTable[index].bst < minBst)
            leftBound = index + 1;
        else
            rightBound = index;
    }
    *start = leftBound;

    rightBound = sBstTableCount;
    while (leftBound < rightBound)
    {
        index = (leftBound + rightBound) / 2;
        if (sBstTable[index].bst > maxBst)
            rightBound = index;
        else
            leftBound = index + 1;
    }
    *end = leftBound;
}

u16 GetRandomizedSpecies(enum RandomizerContext context, u32 data1, u32 data2, u16 species)
{
    rng_value_t rng;
    u32 bst, minBst, maxBst;
    u16 rangeStart, rangeEnd, rangeCount;

    if (!IsSpeciesRandomizable(species))
        return species;

    if (!sBstTableBuilt)
        BuildBstTable();

    if (sBstTableCount == 0)
        return species;

    bst = GetSpeciesBaseStatTotal(species);
    minBst = (bst * (100 - RANDOMIZER_BST_LENIENCY_PERCENT)) / 100;
    maxBst = (bst * (100 + RANDOMIZER_BST_LENIENCY_PERCENT)) / 100;

    GetBstRangeIndices((u16)minBst, (u16)maxBst, &rangeStart, &rangeEnd);
    rangeCount = rangeEnd - rangeStart;
    if (rangeCount == 0)
        return species;

    rng = GetRandomizerSeed(context, data1, data2 ^ species);
    return sBstTable[rangeStart + (LocalRandom32(&rng) % rangeCount)].species;
}

u16 RandomizeWildEncounter(u16 species, u32 mapNum, u32 mapGroup, enum WildPokemonArea area, u32 slot)
{
    u32 seedData1, seedData2;

    if (!IsWildRandomizerEnabled())
        return species;

    seedData1 = (mapGroup << 8) | mapNum;
    seedData2 = ((u32)area << 8) | slot;

    return GetRandomizedSpecies(RANDOMIZER_CONTEXT_WILD_ENCOUNTER, seedData1, seedData2, species);
}

u16 RandomizeTrainerMon(const void *trainerIdentity, u32 slot, u32 totalMons, u16 species)
{
    u32 seedData1, seedData2;

    if (!IsTrainerRandomizerEnabled())
        return species;

    seedData1 = (u32)(uintptr_t)trainerIdentity;
    seedData2 = (totalMons << 8) | slot;

    return GetRandomizedSpecies(RANDOMIZER_CONTEXT_TRAINER_PARTY, seedData1, seedData2, species);
}

#define RANDOMIZER_MAX_UNIQUE_LIST_SIZE 8

u16 RandomizeUniqueMonList(u32 slot, const u16 *originalSpecies, u32 count)
{
    u16 results[RANDOMIZER_MAX_UNIQUE_LIST_SIZE];
    u32 i, tries;

    if (!IsStarterAndGiftRandomizerEnabled())
        return originalSpecies[slot];

    if (count > RANDOMIZER_MAX_UNIQUE_LIST_SIZE)
        count = RANDOMIZER_MAX_UNIQUE_LIST_SIZE;

    for (i = 0; i < count; i++)
    {
        results[i] = GetRandomizedSpecies(RANDOMIZER_CONTEXT_STARTER_AND_GIFT, 0, i, originalSpecies[i]);
        for (tries = 1; tries < 20; tries++)
        {
            bool32 collision = FALSE;
            u32 j;

            for (j = 0; j < i; j++)
            {
                if (results[j] == results[i])
                {
                    collision = TRUE;
                    break;
                }
            }
            if (!collision)
                break;
            results[i] = GetRandomizedSpecies(RANDOMIZER_CONTEXT_STARTER_AND_GIFT, tries, i, originalSpecies[i]);
        }
    }

    return results[slot];
}

static bool32 IsItemRandomizable(u16 itemId)
{
    if (itemId == ITEM_NONE || itemId >= ITEMS_COUNT)
        return FALSE;
    if (itemId >= ITEM_HM01 && itemId <= ITEM_HM08)
        return FALSE;
    if (GetItemPocket(itemId) == POCKET_KEY_ITEMS)
        return FALSE;
    // Some TM/HM pocket slots are unpriced placeholders, not real obtainable items.
    if (GetItemPocket(itemId) == POCKET_TM_HM && GetItemPrice(itemId) == 0)
        return FALSE;
    return TRUE;
}

#include "data/randomizer/item_whitelist.h"

// TM01-TM50 (the Gen 3/4 TM range) are folded into the same pool as the
// curated whitelist, so a TM can turn into any regular item and any regular
// item pickup can turn into a TM - which move that TM slot teaches is a
// separate roll (BuildRandomizedTmMoves), unaffected by this.
#define TM_POOL_SIZE (ITEM_TM50 - ITEM_TM01 + 1)
#define COMBINED_ITEM_POOL_SIZE (ITEM_WHITELIST_SIZE + TM_POOL_SIZE + RANDOMIZER_MEGA_STONE_POOL_SIZE)

// Every Mega Stone in the game is a valid *candidate*, but only
// RANDOMIZER_MEGA_STONE_POOL_SIZE of them are ever actually obtainable on a
// given save - picked once via reservoir sampling (Algorithm R), same
// technique as the moveset randomizer, so the full ~90-item Mega Stone list
// never needs to be held in memory at once.
EWRAM_DATA static u16 sMegaStoneSubset[RANDOMIZER_MEGA_STONE_POOL_SIZE] = {0};
EWRAM_DATA static bool8 sMegaStoneSubsetBuilt = FALSE;

static void BuildMegaStoneSubset(void)
{
    rng_value_t rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_MEGA_STONE_SUBSET, 0, 0);
    u32 count = 0;
    u16 item;

    for (item = 1; item < ITEMS_COUNT; item++)
    {
        if (GetItemHoldEffect(item) != HOLD_EFFECT_MEGA_STONE)
            continue;

        if (count < RANDOMIZER_MEGA_STONE_POOL_SIZE)
        {
            sMegaStoneSubset[count] = item;
        }
        else
        {
            u32 j = LocalRandom32(&rng) % (count + 1);
            if (j < RANDOMIZER_MEGA_STONE_POOL_SIZE)
                sMegaStoneSubset[j] = item;
        }
        count++;
    }

    sMegaStoneSubsetBuilt = TRUE;
}

static u16 RandomizeItemFromSeed(rng_value_t rng, u16 itemId)
{
    u16 result;
    u32 tries;
    u32 pick;

    if (!sMegaStoneSubsetBuilt)
        BuildMegaStoneSubset();

    for (tries = 0; tries < 60; tries++)
    {
        pick = LocalRandom32(&rng) % COMBINED_ITEM_POOL_SIZE;
        if (pick < ITEM_WHITELIST_SIZE)
            result = sRandomizerItemWhitelist[pick];
        else if (pick < ITEM_WHITELIST_SIZE + TM_POOL_SIZE)
            result = ITEM_TM01 + (pick - ITEM_WHITELIST_SIZE);
        else
            result = sMegaStoneSubset[pick - ITEM_WHITELIST_SIZE - TM_POOL_SIZE];
        if (IsItemRandomizable(result))
            return result;
    }

    return itemId;
}

u16 RandomizeFieldItem(u16 itemId, u32 mapNum, u32 mapGroup, u32 localId)
{
    rng_value_t rng;
    u32 seedData1, seedData2;

    if (!IsFieldItemRandomizerEnabled() || !IsItemRandomizable(itemId))
        return itemId;

    seedData1 = (mapGroup << 8) | mapNum;
    seedData2 = localId ^ itemId;
    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_FIELD_ITEM, seedData1, seedData2);

    return RandomizeItemFromSeed(rng, itemId);
}

u16 RandomizeNpcGiftItem(u16 itemId, u32 mapNum, u32 mapGroup)
{
    rng_value_t rng;
    u32 seedData1, seedData2;

    if (!IsFieldItemRandomizerEnabled() || !IsItemRandomizable(itemId))
        return itemId;

    seedData1 = (mapGroup << 8) | mapNum;
    seedData2 = itemId;
    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_NPC_GIFT_ITEM, seedData1, seedData2);

    return RandomizeItemFromSeed(rng, itemId);
}

// Abilities tightly coupled to a specific species' form-change machinery
// (battle-form triggers, Ditto's transform, Shedinja's HP=1 gimmick) rather
// than being a generic combat ability. Placing these on an unrelated species
// wouldn't just be "weird", the form-change code backing them typically
// assumes a specific species/form table and can misbehave. Excluded from the
// random pool regardless of the broad-whitelist philosophy elsewhere.
static bool32 IsAbilityRandomizable(u16 ability)
{
    switch (ability)
    {
    case ABILITY_NONE:
    case ABILITY_WONDER_GUARD:
    case ABILITY_IMPOSTER:
    case ABILITY_ZEN_MODE:
    case ABILITY_STANCE_CHANGE:
    case ABILITY_SHIELDS_DOWN:
    case ABILITY_SCHOOLING:
    case ABILITY_POWER_CONSTRUCT:
    case ABILITY_GULP_MISSILE:
    case ABILITY_HUNGER_SWITCH:
    case ABILITY_ICE_FACE:
    case ABILITY_RKS_SYSTEM:
    case ABILITY_MULTITYPE:
    case ABILITY_COMMANDER:
    case ABILITY_EMBODY_ASPECT_TEAL_MASK:
    case ABILITY_EMBODY_ASPECT_HEARTHFLAME_MASK:
    case ABILITY_EMBODY_ASPECT_WELLSPRING_MASK:
    case ABILITY_EMBODY_ASPECT_CORNERSTONE_MASK:
    case ABILITY_TERA_SHIFT:
    case ABILITY_TERAFORM_ZERO:
    case ABILITY_ZERO_TO_HERO:
    case ABILITY_FIRE_MANE:
    case ABILITY_EELEVATE:
        return FALSE;
    default:
        return TRUE;
    }
}

// Strong abilities specifically for Mega Evolutions, which are not shared with the base species' family.
static const u16 sRandomizerStrongAbilityWhitelist[] =
{
    ABILITY_DROUGHT, ABILITY_DRIZZLE, ABILITY_SAND_STREAM, ABILITY_SNOW_WARNING,
    ABILITY_INTIMIDATE, ABILITY_LEVITATE, ABILITY_MULTISCALE, ABILITY_REGENERATOR,
    ABILITY_MAGIC_GUARD, ABILITY_UNAWARE, ABILITY_PRANKSTER, ABILITY_SPEED_BOOST,
    ABILITY_ADAPTABILITY, ABILITY_HUGE_POWER, ABILITY_PURE_POWER, ABILITY_PROTEAN,
    ABILITY_LIBERO, ABILITY_TECHNICIAN, ABILITY_SHEER_FORCE, ABILITY_TOUGH_CLAWS,
    ABILITY_GUTS, ABILITY_MARVEL_SCALE, ABILITY_POISON_HEAL, ABILITY_WATER_ABSORB,
    ABILITY_VOLT_ABSORB, ABILITY_FLASH_FIRE, ABILITY_MOTOR_DRIVE, ABILITY_SAP_SIPPER,
    ABILITY_CONTRARY, ABILITY_MOXIE, ABILITY_BEAST_BOOST, ABILITY_DOWNLOAD,
    ABILITY_TRACE, ABILITY_SIMPLE, ABILITY_SKILL_LINK, ABILITY_IRON_FIST,
    ABILITY_CHLOROPHYLL, ABILITY_SWIFT_SWIM, ABILITY_SAND_RUSH, ABILITY_SLUSH_RUSH,
    ABILITY_ANALYTIC, ABILITY_TINTED_LENS, ABILITY_SCRAPPY, ABILITY_MOLD_BREAKER,
    ABILITY_TERAVOLT, ABILITY_TURBOBLAZE, ABILITY_MAGIC_BOUNCE, ABILITY_STEELWORKER,
    ABILITY_TRANSISTOR, ABILITY_PUNK_ROCK, ABILITY_DRAGONS_MAW, ABILITY_ROCKY_PAYLOAD,
    ABILITY_SWORD_OF_RUIN, ABILITY_VESSEL_OF_RUIN, ABILITY_TABLETS_OF_RUIN, ABILITY_BEADS_OF_RUIN,
    ABILITY_FRIEND_GUARD, ABILITY_AS_ONE_ICE_RIDER, ABILITY_AS_ONE_SHADOW_RIDER, ABILITY_BATTERY,
    ABILITY_COSTAR, ABILITY_FLOWER_GIFT, ABILITY_HOSPITALITY, ABILITY_NEUTRALIZING_GAS,
    ABILITY_VICTORY_STAR, ABILITY_WATER_BUBBLE, ABILITY_STAMINA, ABILITY_POWER_SPOT,
    ABILITY_FAIRY_AURA, ABILITY_GRASSY_SURGE, ABILITY_MISTY_SURGE, ABILITY_ELECTRIC_SURGE, ABILITY_PSYCHIC_SURGE,
    ABILITY_DARK_AURA, ABILITY_GALE_WINGS, ABILITY_PRISM_ARMOR, ABILITY_LIGHTNING_ROD, 
    ABILITY_SHADOW_TAG, ABILITY_DEFIANT, ABILITY_INNER_FOCUS, ABILITY_TELEPATHY, 
    ABILITY_FUR_COAT, ABILITY_SERENE_GRACE, ABILITY_SURGE_SURFER, ABILITY_BATTLE_BOND,
    ABILITY_DISGUISE, ABILITY_QUEENLY_MAJESTY, ABILITY_SOUL_HEART, ABILITY_INTREPID_SWORD,
    ABILITY_DAUNTLESS_SHIELD, ABILITY_COTTON_DOWN, ABILITY_UNSEEN_FIST, ABILITY_WELL_BAKED_BODY,
    ABILITY_PROTOSYNTHESIS, ABILITY_QUARK_DRIVE, ABILITY_GOOD_AS_GOLD, ABILITY_ORICHALCUM_PULSE,
    ABILITY_HADRON_ENGINE, ABILITY_SUPREME_OVERLORD, ABILITY_EARTH_EATER, ABILITY_PIERCING_DRILL,
    ABILITY_MEGA_SOL, ABILITY_SPICY_SPRAY 
};

// Walks pre-evolutions back to the root of the family (e.g. Charizard -> Charmeleon
// -> Charmander), so every stage can be seeded identically for a given ability slot.
static u16 GetBaseEvolutionSpecies(u16 species)
{
    u16 preEvo;
    u32 guard;

    for (guard = 0; guard < 10; guard++)
    {
        preEvo = GetSpeciesPreEvolution(species);
        if (preEvo == SPECIES_NONE)
            break;
        species = preEvo;
    }
    return species;
}

u16 RandomizeAbility(u16 species, u8 slot, u16 originalAbility)
{
    rng_value_t rng;
    u16 result;
    u32 tries;

    if (!IsAbilityRandomizerEnabled() || originalAbility == ABILITY_NONE)
        return originalAbility;

    // Wonder Guard/Imposter are these two species' entire identity, not
    // ordinary combat abilities to shuffle.
    if (species == SPECIES_SHEDINJA || species == SPECIES_DITTO)
        return originalAbility;

    if (gSpeciesInfo[species].isMegaEvolution)
    {
        // Mega abilities roll independently per Mega, not shared with the base
        // species' family, from the smaller "stronger" pool.
        rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_ABILITY, species, slot);
        for (tries = 0; tries < 30; tries++)
        {
            result = sRandomizerStrongAbilityWhitelist[LocalRandom32(&rng) % ARRAY_COUNT(sRandomizerStrongAbilityWhitelist)];
            if (IsAbilityRandomizable(result))
                return result;
        }
        return originalAbility;
    }

    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_ABILITY, GetBaseEvolutionSpecies(species), slot);
    for (tries = 0; tries < 60; tries++)
    {
        result = 1 + (LocalRandom32(&rng) % (ABILITIES_COUNT - 1));
        if (IsAbilityRandomizable(result))
            return result;
    }

    return originalAbility;
}

// TM moves are drawn from every move in the game, not just the original 50 TM
// moves - so e.g. TM17 could end up teaching Dragon Dance, something that was
// never TM-teachable before. HM moves are the one carve-out: they stay
// exclusively HM-taught (gameplan.md: "HM's are kept the same"), otherwise
// the same move could become reachable through two different items and the
// reverse lookup below wouldn't have a single correct answer.
static bool32 IsHMMove(u16 move)
{
    switch (move)
    {
#define HM_MOVE_CASE(name) case MOVE_##name: return TRUE;
    FOREACH_HM(HM_MOVE_CASE)
#undef HM_MOVE_CASE
    default:
        return FALSE;
    }
}

static bool32 IsMoveEligibleForTM(u16 move)
{
    if (move == MOVE_NONE || move >= MOVES_COUNT)
        return FALSE;
    if (move == MOVE_STRUGGLE || move == MOVE_SPORE)
        return FALSE;
    if (IsHMMove(move))
        return FALSE;
    return TRUE;
}

EWRAM_DATA static u16 sRandomizedTmMoves[NUM_TECHNICAL_MACHINES] = {0};
EWRAM_DATA static bool8 sTmMovesBuilt = FALSE;

static void BuildRandomizedTmMoves(void)
{
    rng_value_t rng;
    u32 seenMoveBitVector[(MOVES_COUNT - 1) / 32 + 1] = {0};
    u32 slot, wordIndex;
    u16 candidate;
    u8 bitIndex;

    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_TM_MOVES, 0, 0);

    for (slot = 0; slot < NUM_TECHNICAL_MACHINES; slot++)
    {
        do
        {
            candidate = 1 + (LocalRandom32(&rng) % (MOVES_COUNT - 1));
        } while (!IsMoveEligibleForTM(candidate)
              || (seenMoveBitVector[(candidate - 1) / 32] & (1u << ((candidate - 1) & 31))));

        wordIndex = (candidate - 1) / 32;
        bitIndex = (candidate - 1) & 31;
        seenMoveBitVector[wordIndex] |= 1u << bitIndex;
        sRandomizedTmMoves[slot] = candidate;
    }

    sTmMovesBuilt = TRUE;
}

u16 RandomizeTMMove(u16 tmItem, u16 originalMove)
{
    if (!IsTMMoveRandomizerEnabled() || tmItem < ITEM_TM01 || tmItem > ITEM_TM50)
        return originalMove;

    if (!sTmMovesBuilt)
        BuildRandomizedTmMoves();

    return sRandomizedTmMoves[tmItem - ITEM_TM01];
}

u16 RandomizeTMItemForMove(u16 move, u16 originalItem)
{
    u32 i;

    if (!IsTMMoveRandomizerEnabled())
        return originalItem;

    if (!sTmMovesBuilt)
        BuildRandomizedTmMoves();

    // Check the randomized pool first (a formerly non-TM move may have been
    // assigned a slot this save), regardless of what it originally mapped to.
    for (i = 0; i < NUM_TECHNICAL_MACHINES; i++)
    {
        if (sRandomizedTmMoves[i] == move)
            return ITEM_TM01 + i;
    }

    // Not in this save's TM pool. HM moves are untouched, so keep whatever
    // the raw lookup found (its own HM item, or ITEM_NONE for anything else).
    return originalItem;
}

// --- Moveset randomizer ---
// Every randomizable species gets a synthesized RANDOMIZER_MOVESET_SIZE-move
// level-up learnset: 7 STAB-eligible damaging moves, 7 non-STAB damaging
// moves, and 7 status moves, picked via reservoir sampling (so this never
// needs to hold the full ~900-move candidate list in memory, just up to 7 per
// category at a time) and interleaved across a fixed level curve so power
// increases with level, per gameplan.md.

// 21 levels from 1 to 63, spaced as evenly as 21 slots allow.
static const u8 sMovesetLevelCurve[RANDOMIZER_MOVESET_SIZE] =
{
    1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 32, 35, 38, 41, 44, 47, 50, 53, 56, 59, 63,
};

#define MOVESET_CATEGORY_SIZE (RANDOMIZER_MOVESET_SIZE / 3)

struct MoveReservoir
{
    u16 moves[MOVESET_CATEGORY_SIZE];
    u32 count;
};

// Algorithm R reservoir sampling: gives a uniformly random, unique subset of
// up to MOVESET_CATEGORY_SIZE moves from a stream of unknown total length,
// in a single pass, without storing every candidate seen.
static void ReservoirAdd(struct MoveReservoir *reservoir, u16 move, rng_value_t *rng)
{
    if (reservoir->count < MOVESET_CATEGORY_SIZE)
    {
        reservoir->moves[reservoir->count] = move;
    }
    else
    {
        u32 j = LocalRandom32(rng) % (reservoir->count + 1);
        if (j < MOVESET_CATEGORY_SIZE)
            reservoir->moves[j] = move;
    }
    reservoir->count++;
}

// Ascending by power, so weaker moves land at the earlier (lower level)
// slots and stronger moves at later ones.
static void SortReservoirByPower(struct MoveReservoir *reservoir)
{
    u32 count = (reservoir->count < MOVESET_CATEGORY_SIZE) ? reservoir->count : MOVESET_CATEGORY_SIZE;
    u32 i, j;

    for (i = 1; i < count; i++)
    {
        u16 key = reservoir->moves[i];
        u32 keyPower = GetMovePower(key);
        j = i;
        while (j > 0 && GetMovePower(reservoir->moves[j - 1]) > keyPower)
        {
            reservoir->moves[j] = reservoir->moves[j - 1];
            j--;
        }
        reservoir->moves[j] = key;
    }
}

static bool32 IsMoveEligibleForLearnset(u16 move)
{
    if (move == MOVE_NONE || move >= MOVES_COUNT)
        return FALSE;
    if (move == MOVE_STRUGGLE || move == MOVE_SPORE)
        return FALSE;
    return TRUE;
}

EWRAM_DATA static struct LevelUpMove sRandomizedLearnsetBuffer[RANDOMIZER_MOVESET_SIZE + 1] = {0};

const struct LevelUpMove *RandomizeLevelUpLearnset(u16 species, const struct LevelUpMove *originalLearnset)
{
    rng_value_t rng;
    struct MoveReservoir stabMoves = {0};
    struct MoveReservoir nonStabMoves = {0};
    struct MoveReservoir statusMoves = {0};
    enum Type type1, type2;
    u16 move;
    u32 stabIndex = 0, nonStabIndex = 0, statusIndex = 0;
    u32 slot;

    if (!IsMovesetRandomizerEnabled() || !IsSpeciesRandomizable(species))
        return originalLearnset;

    type1 = GetSpeciesType(species, 0);
    type2 = GetSpeciesType(species, 1);

    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_MOVESET, species, 0);

    for (move = 1; move < MOVES_COUNT; move++)
    {
        if (!IsMoveEligibleForLearnset(move))
            continue;

        if (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS)
            ReservoirAdd(&statusMoves, move, &rng);
        else if (GetMoveType(move) == type1 || GetMoveType(move) == type2)
            ReservoirAdd(&stabMoves, move, &rng);
        else
            ReservoirAdd(&nonStabMoves, move, &rng);
    }

    SortReservoirByPower(&stabMoves);
    SortReservoirByPower(&nonStabMoves);

    for (slot = 0; slot < RANDOMIZER_MOVESET_SIZE; slot++)
    {
        u16 chosenMove;

        switch (slot % 3)
        {
        case 0:
            chosenMove = (stabIndex < stabMoves.count && stabIndex < MOVESET_CATEGORY_SIZE)
                ? stabMoves.moves[stabIndex++] : MOVE_NONE;
            break;
        case 1:
            chosenMove = (nonStabIndex < nonStabMoves.count && nonStabIndex < MOVESET_CATEGORY_SIZE)
                ? nonStabMoves.moves[nonStabIndex++] : MOVE_NONE;
            break;
        default:
            chosenMove = (statusIndex < statusMoves.count && statusIndex < MOVESET_CATEGORY_SIZE)
                ? statusMoves.moves[statusIndex++] : MOVE_NONE;
            break;
        }

        sRandomizedLearnsetBuffer[slot].move = chosenMove;
        sRandomizedLearnsetBuffer[slot].level = sMovesetLevelCurve[slot];
    }

    sRandomizedLearnsetBuffer[RANDOMIZER_MOVESET_SIZE].move = LEVEL_UP_MOVE_END;
    sRandomizedLearnsetBuffer[RANDOMIZER_MOVESET_SIZE].level = 0;

    return sRandomizedLearnsetBuffer;
}

static enum Type PickRandomMonType(rng_value_t *rng)
{
    enum Type type;

    // TYPE_MYSTERY sits in the middle of the real 1-19 type range (it's a
    // leftover placeholder, not a real typing) - reroll if it comes up.
    do
    {
        type = TYPE_NORMAL + (LocalRandom32(rng) % (TYPE_FAIRY - TYPE_NORMAL + 1));
    } while (type == TYPE_MYSTERY);

    return type;
}

u16 RandomizeSpeciesType(u16 species, u8 slot, u16 originalType)
{
    rng_value_t rng;

    // Scoped to Shedinja only - not a general type randomizer.
    if (!IsAbilityRandomizerEnabled() || species != SPECIES_SHEDINJA)
        return originalType;

    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_SPECIES_TYPE, species, slot);
    return PickRandomMonType(&rng);
}

#endif // RANDOMIZER_AVAILABLE
