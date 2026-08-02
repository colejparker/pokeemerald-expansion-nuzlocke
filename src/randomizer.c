#include "global.h"
#include "randomizer.h"

#if RANDOMIZER_AVAILABLE == TRUE

#include "event_data.h"
#include "item.h"
#include "new_game.h"
#include "pokemon.h"
#include "random.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/species.h"

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
    SPECIES_SOLGALEO,
    SPECIES_LUNALA,
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

static inline bool32 IsItemTM(u16 itemId)
{
    return itemId >= ITEM_TM01 && itemId <= ITEM_TM100;
}

u16 RandomizeFieldItem(u16 itemId, u32 mapNum, u32 mapGroup, u32 localId)
{
    rng_value_t rng;
    u32 seedData1, seedData2;
    u16 result;
    u32 tries;

    if (!IsFieldItemRandomizerEnabled() || !IsItemRandomizable(itemId))
        return itemId;

    seedData1 = (mapGroup << 8) | mapNum;
    seedData2 = localId ^ itemId;
    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_FIELD_ITEM, seedData1, seedData2);

    if (IsItemTM(itemId))
    {
        // TM slots only randomize to other TM slots.
        for (tries = 0; tries < 40; tries++)
        {
            result = ITEM_TM01 + (LocalRandom32(&rng) % (ITEM_TM100 - ITEM_TM01 + 1));
            if (GetItemPrice(result) != 0)
                return result;
        }
        return itemId;
    }

    // Everything else randomizes among non-TM/HM/key items.
    for (tries = 0; tries < 60; tries++)
    {
        result = 1 + (LocalRandom32(&rng) % (ITEMS_COUNT - 1));
        if (IsItemRandomizable(result) && !IsItemTM(result))
            return result;
    }

    return itemId;
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
    case ABILITY_DISGUISE:
    case ABILITY_BATTLE_BOND:
    case ABILITY_POWER_CONSTRUCT:
    case ABILITY_GULP_MISSILE:
    case ABILITY_HUNGER_SWITCH:
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

#endif // RANDOMIZER_AVAILABLE
