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
    SPECIES_COSMOG,
    SPECIES_COSMOEM,
    SPECIES_SLAKOTH,
    SPECIES_VIGOROTH,
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

struct CosmeticFormFamily
{
    const u16 *members;
    u16 count;
};

static const u16 sAlcremieForms[] = {
    SPECIES_ALCREMIE_STRAWBERRY_VANILLA_CREAM, SPECIES_ALCREMIE_BERRY_VANILLA_CREAM,
    SPECIES_ALCREMIE_LOVE_VANILLA_CREAM, SPECIES_ALCREMIE_STAR_VANILLA_CREAM,
    SPECIES_ALCREMIE_CLOVER_VANILLA_CREAM, SPECIES_ALCREMIE_FLOWER_VANILLA_CREAM,
    SPECIES_ALCREMIE_RIBBON_VANILLA_CREAM,
};

static const u16 sUnownForms[] = {
    SPECIES_UNOWN, SPECIES_UNOWN_B, SPECIES_UNOWN_C, SPECIES_UNOWN_D, SPECIES_UNOWN_E,
    SPECIES_UNOWN_F, SPECIES_UNOWN_G, SPECIES_UNOWN_H, SPECIES_UNOWN_I, SPECIES_UNOWN_J,
    SPECIES_UNOWN_K, SPECIES_UNOWN_L, SPECIES_UNOWN_M, SPECIES_UNOWN_N, SPECIES_UNOWN_O,
    SPECIES_UNOWN_P, SPECIES_UNOWN_Q, SPECIES_UNOWN_R, SPECIES_UNOWN_S, SPECIES_UNOWN_T,
    SPECIES_UNOWN_U, SPECIES_UNOWN_V, SPECIES_UNOWN_W, SPECIES_UNOWN_X, SPECIES_UNOWN_Y,
    SPECIES_UNOWN_Z, SPECIES_UNOWN_EXCLAMATION, SPECIES_UNOWN_QUESTION,
};

static const u16 sScatterbugForms[] = {
    SPECIES_SCATTERBUG_ICY_SNOW, SPECIES_SCATTERBUG_POLAR, SPECIES_SCATTERBUG_TUNDRA,
    SPECIES_SCATTERBUG_CONTINENTAL, SPECIES_SCATTERBUG_GARDEN, SPECIES_SCATTERBUG_ELEGANT,
    SPECIES_SCATTERBUG_MEADOW, SPECIES_SCATTERBUG_MODERN, SPECIES_SCATTERBUG_MARINE,
    SPECIES_SCATTERBUG_ARCHIPELAGO, SPECIES_SCATTERBUG_HIGH_PLAINS, SPECIES_SCATTERBUG_SANDSTORM,
    SPECIES_SCATTERBUG_RIVER, SPECIES_SCATTERBUG_MONSOON, SPECIES_SCATTERBUG_SAVANNA,
    SPECIES_SCATTERBUG_SUN, SPECIES_SCATTERBUG_OCEAN, SPECIES_SCATTERBUG_JUNGLE,
    SPECIES_SCATTERBUG_FANCY, SPECIES_SCATTERBUG_POKEBALL,
};

static const u16 sSpewpaForms[] = {
    SPECIES_SPEWPA_ICY_SNOW, SPECIES_SPEWPA_POLAR, SPECIES_SPEWPA_TUNDRA,
    SPECIES_SPEWPA_CONTINENTAL, SPECIES_SPEWPA_GARDEN, SPECIES_SPEWPA_ELEGANT,
    SPECIES_SPEWPA_MEADOW, SPECIES_SPEWPA_MODERN, SPECIES_SPEWPA_MARINE,
    SPECIES_SPEWPA_ARCHIPELAGO, SPECIES_SPEWPA_HIGH_PLAINS, SPECIES_SPEWPA_SANDSTORM,
    SPECIES_SPEWPA_RIVER, SPECIES_SPEWPA_MONSOON, SPECIES_SPEWPA_SAVANNA,
    SPECIES_SPEWPA_SUN, SPECIES_SPEWPA_OCEAN, SPECIES_SPEWPA_JUNGLE,
    SPECIES_SPEWPA_FANCY, SPECIES_SPEWPA_POKEBALL,
};

static const u16 sVivillonForms[] = {
    SPECIES_VIVILLON_ICY_SNOW, SPECIES_VIVILLON_POLAR, SPECIES_VIVILLON_TUNDRA,
    SPECIES_VIVILLON_CONTINENTAL, SPECIES_VIVILLON_GARDEN, SPECIES_VIVILLON_ELEGANT,
    SPECIES_VIVILLON_MEADOW, SPECIES_VIVILLON_MODERN, SPECIES_VIVILLON_MARINE,
    SPECIES_VIVILLON_ARCHIPELAGO, SPECIES_VIVILLON_HIGH_PLAINS, SPECIES_VIVILLON_SANDSTORM,
    SPECIES_VIVILLON_RIVER, SPECIES_VIVILLON_MONSOON, SPECIES_VIVILLON_SAVANNA,
    SPECIES_VIVILLON_SUN, SPECIES_VIVILLON_OCEAN, SPECIES_VIVILLON_JUNGLE,
    SPECIES_VIVILLON_FANCY, SPECIES_VIVILLON_POKEBALL,
};

static const u16 sFurfrouForms[] = {
    SPECIES_FURFROU_NATURAL, SPECIES_FURFROU_HEART, SPECIES_FURFROU_STAR, SPECIES_FURFROU_DIAMOND,
    SPECIES_FURFROU_DEBUTANTE, SPECIES_FURFROU_MATRON, SPECIES_FURFROU_DANDY, SPECIES_FURFROU_LA_REINE,
    SPECIES_FURFROU_KABUKI, SPECIES_FURFROU_PHARAOH,
};

static const u16 sFlabebeForms[] = {
    SPECIES_FLABEBE_RED, SPECIES_FLABEBE_YELLOW, SPECIES_FLABEBE_ORANGE, SPECIES_FLABEBE_BLUE, SPECIES_FLABEBE_WHITE,
};
static const u16 sFloetteForms[] = {
    SPECIES_FLOETTE_RED, SPECIES_FLOETTE_YELLOW, SPECIES_FLOETTE_ORANGE, SPECIES_FLOETTE_BLUE, SPECIES_FLOETTE_WHITE,
};
static const u16 sFlorgesForms[] = {
    SPECIES_FLORGES_RED, SPECIES_FLORGES_YELLOW, SPECIES_FLORGES_ORANGE, SPECIES_FLORGES_BLUE, SPECIES_FLORGES_WHITE,
};

static const u16 sDeerlingForms[] = {
    SPECIES_DEERLING_SPRING, SPECIES_DEERLING_SUMMER, SPECIES_DEERLING_AUTUMN, SPECIES_DEERLING_WINTER,
};
static const u16 sSawsbuckForms[] = {
    SPECIES_SAWSBUCK_SPRING, SPECIES_SAWSBUCK_SUMMER, SPECIES_SAWSBUCK_AUTUMN, SPECIES_SAWSBUCK_WINTER,
};

static const u16 sBurmyForms[] = { SPECIES_BURMY_PLANT, SPECIES_BURMY_SANDY, SPECIES_BURMY_TRASH };
static const u16 sMothimForms[] = { SPECIES_MOTHIM_PLANT, SPECIES_MOTHIM_SANDY, SPECIES_MOTHIM_TRASH };

static const u16 sGenesectForms[] = {
    SPECIES_GENESECT, SPECIES_GENESECT_DOUSE, SPECIES_GENESECT_SHOCK, SPECIES_GENESECT_BURN, SPECIES_GENESECT_CHILL,
};

static const u16 sShellosForms[] = { SPECIES_SHELLOS_WEST, SPECIES_SHELLOS_EAST };
static const u16 sGastrodonForms[] = { SPECIES_GASTRODON_WEST, SPECIES_GASTRODON_EAST };

static const u16 sTatsugiriForms[] = { SPECIES_TATSUGIRI_CURLY, SPECIES_TATSUGIRI_DROOPY, SPECIES_TATSUGIRI_STRETCHY };

static const u16 sMiniorMeteorForms[] = {
    SPECIES_MINIOR_METEOR_RED, SPECIES_MINIOR_METEOR_ORANGE, SPECIES_MINIOR_METEOR_YELLOW,
    SPECIES_MINIOR_METEOR_GREEN, SPECIES_MINIOR_METEOR_BLUE, SPECIES_MINIOR_METEOR_INDIGO,
    SPECIES_MINIOR_METEOR_VIOLET,
};
static const u16 sMiniorCoreForms[] = {
    SPECIES_MINIOR_CORE_RED, SPECIES_MINIOR_CORE_ORANGE, SPECIES_MINIOR_CORE_YELLOW,
    SPECIES_MINIOR_CORE_GREEN, SPECIES_MINIOR_CORE_BLUE, SPECIES_MINIOR_CORE_INDIGO,
    SPECIES_MINIOR_CORE_VIOLET,
};

static const u16 sPumpkabooForms[] = {
    SPECIES_PUMPKABOO_AVERAGE, SPECIES_PUMPKABOO_SMALL, SPECIES_PUMPKABOO_LARGE, SPECIES_PUMPKABOO_SUPER,
};
static const u16 sGourgeistForms[] = {
    SPECIES_GOURGEIST_AVERAGE, SPECIES_GOURGEIST_SMALL, SPECIES_GOURGEIST_LARGE, SPECIES_GOURGEIST_SUPER,
};

static const struct CosmeticFormFamily sCosmeticFormFamilies[] =
{
    { sAlcremieForms, ARRAY_COUNT(sAlcremieForms) },
    { sUnownForms, ARRAY_COUNT(sUnownForms) },
    { sScatterbugForms, ARRAY_COUNT(sScatterbugForms) },
    { sSpewpaForms, ARRAY_COUNT(sSpewpaForms) },
    { sVivillonForms, ARRAY_COUNT(sVivillonForms) },
    { sFurfrouForms, ARRAY_COUNT(sFurfrouForms) },
    { sFlabebeForms, ARRAY_COUNT(sFlabebeForms) },
    { sFloetteForms, ARRAY_COUNT(sFloetteForms) },
    { sFlorgesForms, ARRAY_COUNT(sFlorgesForms) },
    { sDeerlingForms, ARRAY_COUNT(sDeerlingForms) },
    { sSawsbuckForms, ARRAY_COUNT(sSawsbuckForms) },
    { sBurmyForms, ARRAY_COUNT(sBurmyForms) },
    { sMothimForms, ARRAY_COUNT(sMothimForms) },
    { sGenesectForms, ARRAY_COUNT(sGenesectForms) },
    { sShellosForms, ARRAY_COUNT(sShellosForms) },
    { sGastrodonForms, ARRAY_COUNT(sGastrodonForms) },
    { sTatsugiriForms, ARRAY_COUNT(sTatsugiriForms) },
    { sMiniorMeteorForms, ARRAY_COUNT(sMiniorMeteorForms) },
    { sMiniorCoreForms, ARRAY_COUNT(sMiniorCoreForms) },
    { sPumpkabooForms, ARRAY_COUNT(sPumpkabooForms) },
    { sGourgeistForms, ARRAY_COUNT(sGourgeistForms) },
};

static const struct CosmeticFormFamily *GetCosmeticFormFamily(u16 species)
{
    u32 i, j;
    for (i = 0; i < ARRAY_COUNT(sCosmeticFormFamilies); i++)
    {
        const struct CosmeticFormFamily *family = &sCosmeticFormFamilies[i];
        for (j = 0; j < family->count; j++)
        {
            if (family->members[j] == species)
                return family;
        }
    }
    return NULL;
}

static bool32 IsCosmeticFormDuplicate(u16 species)
{
    const struct CosmeticFormFamily *family = GetCosmeticFormFamily(species);
    return family != NULL && family->members[0] != species;
}

static const u16 sExcludedAltForms[] =
{
    SPECIES_MIMIKYU_BUSTED, SPECIES_MIMIKYU_BUSTED_TOTEM, SPECIES_MIMIKYU_TOTEM_DISGUISED,
    SPECIES_CRAMORANT_GULPING, SPECIES_CRAMORANT_GORGING,
    SPECIES_CHERRIM_SUNSHINE,
    SPECIES_KELDEO_RESOLUTE,
    SPECIES_XERNEAS_ACTIVE,
    SPECIES_ZARUDE_DADA,
    SPECIES_MAGEARNA_ORIGINAL,
    SPECIES_PICHU_SPIKY_EARED,
    SPECIES_PIKACHU_ORIGINAL, SPECIES_PIKACHU_HOENN, SPECIES_PIKACHU_SINNOH, SPECIES_PIKACHU_UNOVA,
    SPECIES_PIKACHU_KALOS, SPECIES_PIKACHU_ALOLA, SPECIES_PIKACHU_PARTNER, SPECIES_PIKACHU_WORLD,
    SPECIES_PIKACHU_COSPLAY, SPECIES_PIKACHU_ROCK_STAR, SPECIES_PIKACHU_BELLE, SPECIES_PIKACHU_POP_STAR,
    SPECIES_PIKACHU_PHD, SPECIES_PIKACHU_LIBRE,
};

static bool32 IsExcludedAltForm(u16 species)
{
    u32 i;
    for (i = 0; i < ARRAY_COUNT(sExcludedAltForms); i++)
    {
        if (sExcludedAltForms[i] == species)
            return TRUE;
    }
    return FALSE;
}

static bool32 IsSpeciesEligibleForRandomFeatures(u16 species)
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

bool32 IsSpeciesRandomizable(u16 species)
{
    if (!IsSpeciesEligibleForRandomFeatures(species))
        return FALSE;

    if (species != SPECIES_SILVALLY_NORMAL && GET_BASE_SPECIES_ID(species) == SPECIES_SILVALLY_NORMAL)
        return FALSE;

    if (IsCosmeticFormDuplicate(species) || IsExcludedAltForm(species))
        return FALSE;

    return TRUE;
}

static rng_value_t GetRandomizerSeed(enum RandomizerContext context, u32 data1, u32 data2)
{
    u32 seed = GetTrainerId(gSaveBlock2Ptr->playerTrainerId);
    seed = seed * 2654435761u + (u32)context;
    seed = (seed ^ data1) * 2654435761u;
    seed ^= data2;
    return LocalRandomSeed(seed);
}

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
    u16 chosenSpecies;
    const struct CosmeticFormFamily *family;

    if (!IsSpeciesRandomizable(species) && !IsRestrictedLegendary(species))
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
    chosenSpecies = sBstTable[rangeStart + (LocalRandom32(&rng) % rangeCount)].species;

    family = GetCosmeticFormFamily(chosenSpecies);
    if (family != NULL)
        chosenSpecies = family->members[LocalRandom32(&rng) % family->count];

    return chosenSpecies;
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

struct MegaCapableSpecies
{
    u16 species;
    u16 item;
};

#define MAX_MEGA_CAPABLE_SPECIES 100

EWRAM_DATA static struct MegaCapableSpecies sMegaCapableSpecies[MAX_MEGA_CAPABLE_SPECIES] = {0};
EWRAM_DATA static u16 sMegaCapableSpeciesCount = 0;
EWRAM_DATA static bool8 sMegaCapableSpeciesBuilt = FALSE;

static void BuildMegaCapableSpeciesList(void)
{
    u16 species;

    sMegaCapableSpeciesCount = 0;
    for (species = 1; species < NUM_SPECIES && sMegaCapableSpeciesCount < MAX_MEGA_CAPABLE_SPECIES; species++)
    {
        const struct FormChange *formChanges;
        u32 i;

        if (!IsSpeciesRandomizable(species))
            continue;

        formChanges = GetSpeciesFormChanges(species);
        if (formChanges == NULL)
            continue;

        for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
        {
            if (formChanges[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM)
            {
                sMegaCapableSpecies[sMegaCapableSpeciesCount].species = species;
                sMegaCapableSpecies[sMegaCapableSpeciesCount].item = formChanges[i].param1;
                sMegaCapableSpeciesCount++;
                break;
            }
        }
    }

    sMegaCapableSpeciesBuilt = TRUE;
}

u16 RandomizeEliteFourAce(const void *trainerIdentity, u32 slot, u16 originalSpecies, u16 *outHeldItem)
{
    rng_value_t rng;
    u32 seedData1, pick;

    *outHeldItem = ITEM_NONE;

    if (!IsTrainerRandomizerEnabled())
        return originalSpecies;

    if (!sMegaCapableSpeciesBuilt)
        BuildMegaCapableSpeciesList();

    if (sMegaCapableSpeciesCount == 0)
        return originalSpecies;

    seedData1 = (u32)(uintptr_t)trainerIdentity;
    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_ELITE_FOUR_ACE, seedData1, slot);
    pick = LocalRandom32(&rng) % sMegaCapableSpeciesCount;

    *outHeldItem = sMegaCapableSpecies[pick].item;
    return sMegaCapableSpecies[pick].species;
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

#define TM_POOL_SIZE (ITEM_TM50 - ITEM_TM01 + 1)
#define COMBINED_ITEM_POOL_SIZE (ITEM_WHITELIST_SIZE + TM_POOL_SIZE + RANDOMIZER_MEGA_STONE_POOL_SIZE)

EWRAM_DATA static u16 sMegaStoneSubset[RANDOMIZER_MEGA_STONE_POOL_SIZE] = {0};
EWRAM_DATA static bool8 sMegaStoneSubsetBuilt = FALSE;

static bool32 IsMegaStoneForBannedSpecies(u16 item)
{
    u16 species;

    for (species = 1; species < NUM_SPECIES; species++)
    {
        const struct FormChange *formChanges = GetSpeciesFormChanges(species);
        u32 i;

        if (formChanges == NULL)
            continue;

        for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
        {
            if (formChanges[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM
             && formChanges[i].param1 == item)
                return IsRestrictedLegendary(species);
        }
    }
    return FALSE;
}

static void BuildMegaStoneSubset(void)
{
    rng_value_t rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_MEGA_STONE_SUBSET, 0, 0);
    u32 count = 0;
    u16 item;

    for (item = 1; item < ITEMS_COUNT; item++)
    {
        if (GetItemHoldEffect(item) != HOLD_EFFECT_MEGA_STONE)
            continue;

        if (IsMegaStoneForBannedSpecies(item))
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

static bool32 IsAbilityRandomizable(u16 ability)
{
    switch (ability)
    {
    case ABILITY_NONE:
    case ABILITY_314:
    case ABILITY_317:
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
    case ABILITY_TERA_SHELL:
    case ABILITY_TERAFORM_ZERO:
    case ABILITY_ZERO_TO_HERO:
    case ABILITY_FIRE_MANE:
    case ABILITY_EELEVATE:
    case ABILITY_ANTICIPATION:
    case ABILITY_BALL_FETCH:
    case ABILITY_DEFEATIST:
    case ABILITY_DISGUISE:
    case ABILITY_FORECAST:
    case ABILITY_FOREWARN:
    case ABILITY_FRISK:
    case ABILITY_GORILLA_TACTICS:
    case ABILITY_GRASS_PELT:
    case ABILITY_HONEY_GATHER:
    case ABILITY_HUGE_POWER:
    case ABILITY_ILLUMINATE:
    case ABILITY_KLUTZ:
    case ABILITY_MINUS:
    case ABILITY_PLUS:
    case ABILITY_PICKUP:
    case ABILITY_PURE_POWER:
    case ABILITY_SLOW_START:
    case ABILITY_STICKY_HOLD:
    case ABILITY_TRUANT:
    case ABILITY_STALL:
    case ABILITY_WIMP_OUT:
    case ABILITY_RUN_AWAY:
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
    ABILITY_ADAPTABILITY, ABILITY_PROTEAN, ABILITY_DESOLATE_LAND, ABILITY_PRIMORDIAL_SEA,
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
    ABILITY_MEGA_SOL, ABILITY_SPICY_SPRAY, ABILITY_AIR_LOCK, ABILITY_DELTA_STREAM,
    ABILITY_ICE_SCALES
};

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

static bool32 IsMoveEligible(u16 move)
{
    switch (move)
    {
        case MOVE_NONE:
        case MOVE_STRUGGLE:
        case MOVE_SPORE:
        case MOVE_SHEER_COLD:
        case MOVE_FISSURE:
        case MOVE_HORN_DRILL:
        case MOVE_GUILLOTINE:
        case MOVE_VEEVEE_VOLLEY:
        case MOVE_DRAGON_RAGE:
        case MOVE_DARK_VOID:
        case MOVE_SPIT_UP:
        case MOVE_SWALLOW:
            return FALSE;
        default:
            return TRUE;
    }
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
        } while (!IsMoveEligible(candidate)
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

    for (i = 0; i < NUM_TECHNICAL_MACHINES; i++)
    {
        if (sRandomizedTmMoves[i] == move)
            return ITEM_TM01 + i;
    }

    return originalItem;
}

static const u8 sMovesetLevelCurve[RANDOMIZER_MOVESET_SIZE] =
{
    1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 32, 35, 38, 41, 44, 47, 50, 53, 56, 59, 63,
};

#define MOVESET_CATEGORY_SIZE (RANDOMIZER_MOVESET_SIZE / 3)

#define MOVESET_STATUS_TYPE_MATCH_CAP 5

struct MoveReservoir
{
    u16 moves[MOVESET_CATEGORY_SIZE];
    u32 count;
};

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

static u8 GetMoveMinimumLearnLevel(u16 move)
{
    if (
        move == MOVE_RETURN || 
        move == MOVE_FRUSTRATION ||
        move == MOVE_CRUSH_GRIP ||
        move == MOVE_WATER_SPOUT ||
        move == MOVE_ERUPTION ||
        move == MOVE_DRAGON_ENERGY ||
        move == MOVE_HARD_PRESS ||
        move == MOVE_WRING_OUT ||
        move == MOVE_PIKA_PAPOW
    )
        return 40;
    return 1;
}

static void EnforceReservoirMinLevels(struct MoveReservoir *reservoir, u8 categorySlotStart)
{
    u32 count = (reservoir->count < MOVESET_CATEGORY_SIZE) ? reservoir->count : MOVESET_CATEGORY_SIZE;
    u32 i;

    for (i = 0; i < count; i++)
    {
        u8 minLevel = GetMoveMinimumLearnLevel(reservoir->moves[i]);
        u8 curLevel = sMovesetLevelCurve[categorySlotStart + i * 3];

        if (minLevel > curLevel)
        {
            u16 move = reservoir->moves[i];
            u32 k = i + 1;
            while (k < count && minLevel > sMovesetLevelCurve[categorySlotStart + k * 3])
                k++;
            if (k < count)
            {
                u32 j;
                for (j = i; j < k; j++)
                    reservoir->moves[j] = reservoir->moves[j + 1];
                reservoir->moves[k] = move;
            }
        }
    }
}

EWRAM_DATA static struct LevelUpMove sRandomizedLearnsetBuffer[RANDOMIZER_MOVESET_SIZE + 1] = {0};

const struct LevelUpMove *RandomizeLevelUpLearnset(u16 species, const struct LevelUpMove *originalLearnset)
{
    rng_value_t rng;
    struct MoveReservoir stabMoves = {0};
    struct MoveReservoir nonStabMoves = {0};
    struct MoveReservoir statusMatchingMoves = {0};
    struct MoveReservoir statusOtherMoves = {0};
    enum Type type1, type2;
    u16 move;
    u32 stabIndex = 0, nonStabIndex = 0, statusMatchingIndex = 0, statusOtherIndex = 0;
    u32 slot;

    if (!IsMovesetRandomizerEnabled() || !IsSpeciesEligibleForRandomFeatures(species))
        return originalLearnset;

    type1 = GetSpeciesType(species, 0);
    type2 = GetSpeciesType(species, 1);

    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_MOVESET, species, 0);

    for (move = 1; move < MOVES_COUNT; move++)
    {
        if (!IsMoveEligible(move))
            continue;

        if (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS)
        {
            if (GetMoveType(move) == type1 || GetMoveType(move) == type2)
                ReservoirAdd(&statusMatchingMoves, move, &rng);
            else
                ReservoirAdd(&statusOtherMoves, move, &rng);
        }
        else if (GetMoveType(move) == type1 || GetMoveType(move) == type2)
            ReservoirAdd(&stabMoves, move, &rng);
        else
            ReservoirAdd(&nonStabMoves, move, &rng);
    }

    SortReservoirByPower(&stabMoves);
    SortReservoirByPower(&nonStabMoves);
    EnforceReservoirMinLevels(&stabMoves, 0);
    EnforceReservoirMinLevels(&nonStabMoves, 1);

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
            if (statusMatchingIndex < statusMatchingMoves.count && statusMatchingIndex < MOVESET_STATUS_TYPE_MATCH_CAP)
                chosenMove = statusMatchingMoves.moves[statusMatchingIndex++];
            else if (statusOtherIndex < statusOtherMoves.count && statusOtherIndex < MOVESET_CATEGORY_SIZE)
                chosenMove = statusOtherMoves.moves[statusOtherIndex++];
            else
                chosenMove = MOVE_NONE;
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

    do
    {
        type = TYPE_NORMAL + (LocalRandom32(rng) % (TYPE_FAIRY - TYPE_NORMAL + 1));
    } while (type == TYPE_MYSTERY);

    return type;
}

u16 RandomizeSpeciesType(u16 species, u8 slot, u16 originalType)
{
    rng_value_t rng;

    if (!IsAbilityRandomizerEnabled() || species != SPECIES_SHEDINJA)
        return originalType;

    rng = GetRandomizerSeed(RANDOMIZER_CONTEXT_SPECIES_TYPE, species, slot);
    return PickRandomMonType(&rng);
}

#endif // RANDOMIZER_AVAILABLE
