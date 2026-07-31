- [x] Players start with 750,000$ (lowered from 999,999 to leave headroom before hitting the 999,999 money cap)
- Pokemon are randomized
    - This includes wild encounters, starter pokemon and random trainers.
    - This does not include gym battles, the Elite 4 and the Champion.
    - Pokemon are randomized to be within 10% up or down of the base stats of the Pokemon that is being randomized.
    - Abilities are randomized from a whitelisted list of abilities
        - Abilities are maintained across evolutions
        - Abilities of Mega Evolutions are not maintained across evolutions and are pulled from a separate, stronger list of abilities
        - Shedinja always has Wonder Guard, but its type is randomized
        - Ditto always has Imposter, but its stat spread is entirely randomized
- Movesets of Pokemon are randomized
    - Every Pokemon learns 21 moves of at the same levels, with the last move being learned at Level 63
    - The move sets always contain 7 STAB moves, 7 non-stab moves and 7 non-damaging moves
    - Moves become more powerful later into the learnset
    - Every Pokemon can learn every TM
- Items are randomized
    - Any item given to the player is randomized from a list
    - The TM's a player can get are randomized, HM's are kept the same
    - Marts do not have the items they can sell randomized
- [x] Pokemon are level capped
    - They cannot level pas the following caps:
        - 0 badges earned: 14
        - 1 badges earned: 21
        - 2 badges earned: 24
        - 3 badges earned: 29
        - 4 badges earned: 36
        - 5 badges earned: 43
        - 6 badges earned: 47
        - 7 badges earned: 50
        - 8 badges earned: 63
    - [ ] Trainers are adjusted for the new level caps (not yet done — trainer parties still use vanilla levels)
- Players are given key items for QOL
    - When leaving Littleroot for the first time when players would normally receive sneakers, they instead receive the following key items:
        - An Infinite Repel
        - A porta heal that heals all of their Pokemon to full HP, PP and status
        - An endless Rare Candy
        - A cap candy that instantly brings a Pokemon to the level cap
            - It does not skip learning moves or evolving the Pokemon
    - Players still start with Sneakers
    - Whiting out removes these key items
- Wild pokemon and trainers pokemon do not give EVs
    - The static encounters of gyms DO give EVs
    - Pokemon can get EV's only through vitamins, feathers and Gym Battles
- Starter Pokemon always start with 5 perfect IV's
- [x] Bag is disabled in trainer battles only (still usable in wild battles)
- When a Pokemon faints, it is marked permanently
- Regirock, Registeel, and Regice become available in their unlocked caves after Rayquaza sends Kyogre and Groudon away in Sootopolis (right before the 8th gym)
    - They are still randomized
- [x] ~~New item called Trade Cable that evolves all Pokemon that would otherwise need to be traded to evolve~~ Not needed — pokeemerald-expansion already ships `ITEM_LINKING_CORD` for exactly this (Kadabra, Machoke, Graveler/Graveler-Alola, Haunter, Boldore, Gurdurr, Phantump, Pumpkaboo forms already evolve on it). Use Linking Cord instead of building a new item.
- [x] All Pokemon that need to evolve by trading a specific item instead evolve when given that item — already true by default for Onix (Metal Coat) and Feebas/Milotic before this hack's Milotic override; no engine work needed
- Pokemon that would normally not be able to evolve without specific conditions are instead changed to level evolutions or easier evolution methods. These Pokemon are:
    - Leafeon (Leaf Stone)
    - Glaceon (Ice Stone)
    - Kingambit (Level 63)
    - Mamoswine (Level 47)
    - Lickilicky (Level 43)
    - Tangrowth (Level 43)
    - Ambipom (Level 43)
    - Yanmega (Level 43)
    - Tsareena (Level 36)
    - Naganadel (Level 50)
    - Grapploct (Level 36)
    - Overwil (Level 43)
    - Wyrdeer (Level 43)
    - Annihilape (Level 50)
    - Fargiraf (Level 32)
    - Appletun/Flapple/Dipplin (Level 29 randomly)
    - Hydrapple (Level 43)
    - Dudunsparce (Level 32)
    - Sirfetch'd (Level 36)
    - Milotic (Level 36)
    - Palafin (Level 38)
    - Gholdengo (Level 47)
    - Malamar (Level 30)
    - Escavalier (Linking Cord) — currently `EVO_TRADE` gated on trading specifically for a Shelmet; default expansion data does not already give this an item alternative like it does for Kadabra/Machoke/etc., so this one still needs a data edit adding `EVO_ITEM, ITEM_LINKING_CORD`
    - Accelgor (Linking Cord) — same situation, gated on trading specifically for a Karrablast
    - Mr. Mime/Galarian Mr. Mime (Level 30)
    - Mantine (Level 21)
    - Urshifu Single Strike (Level 47 while knowing a Dark move)
    - Urshifu Rapid Style Strike (Level 47 while knowing a Water move)
    - Pikachu (Level 15)
    - Togepi (Level 15)
    - Clefairy (Level 15)
    - Marill (Level 15)
    - Roselia (Level 15)
    - Chingling (Level 15)
    - Sudowoodo (Level 21)
    - Chansey (Level 15)
    - Snorlax (Level 24)
    - Jigglypuff (Level 15)
    - Blissey (Level 43)
    - Lucario (Level 29)
    - Crobat (Level 36)
    - Espeon (Moon Stone)
    - Umbreon (Sun Stone)
    - Sylveon (Shiny Stone)
    - Lopunny (Level 21)
    - Swoobat (Level 22)
    - Leavanny (Level 36)
    - Alolan Persian (Level 28)
    - Silvally (Level 43)
    - Frosmoth (Level 29)
    - Rabsca (Level 29)
    - Goodra (Level 50)
    - Galarian Slowking/Slowking (King's Rock)
    - Kleavor/Scizor (Randomly at 43)
    - Ursaluna/Blood Moon Ursaluna (Random at Level 50)
    - Armarouge/Ceruledge (Random at Level 36)
    - Polteageist (Level 36)
    - Sinistcha (Level 36)
    - Archaludon (Level 63)
    - Runerigus (Level 34)
    - Alcremie (Level 30) (random form)
    - Basculegion (Level 50)
    - Melmetal (Level 63)
    - Hydreigon (Level 63)
    - Solgaleo/Lunala (Randomly at 53)
    - Lycanroc forms (Randomly at 25)
- All pokemon that could evolve into a regional form, the evolution is determined randomly
- [x] Shop in Lilycove that sells all the evolution items (new clerk NPC added to the Department Store 5F, `LilycoveCity_DepartmentStore_5F_EventScript_ClerkEvolutionItems`; sells all stones + special evolution items, excluding Linking Cord since that's superseded by the planned Trade Cable item; NPC placed at an inferred-clear tile (11,2) — verify in-game and reposition if blocked)

## Implementation Audit

### Already built-in (just flip a flag / one-line edit)

| Feature | Where | Status |
|---|---|---|
| Bag disabled in trainer battles | `include/config/battle.h` — `B_VAR_NO_BAG_USE` now points at `VAR_NO_BAG_USE` (renamed from `VAR_UNUSED_0x404E`); set to `NO_BAG_AGAINST_TRAINER` (wild battles keep the bag) in `src/new_game.c` on new game and re-asserted in `src/overworld.c` `Overworld_ResetStateAfterWhiteOut` after whiteout | Done |
| Level caps | `include/config/caps.h` — `B_LEVEL_CAP_TYPE = LEVEL_CAP_FLAG_LIST`, `B_EXP_CAP_TYPE = EXP_CAP_HARD`, `B_RARE_CANDY_CAP = TRUE`; `sLevelCapFlagMap` in `src/caps.c` updated to the 8 badge thresholds (14/21/24/29/36/43/47/50) with a 63 cap through the Champion | Done |
| Starting money | Not a config flag, but hardcoded to one call: `src/new_game.c` `SetMoney(&gSaveBlock1Ptr->money, 750000);` (lowered from 999,999 for money-cap headroom) | Done |
| Lilycove evolution-item shop | New clerk NPC + `pokemart` item list added to `data/maps/LilycoveCity_DepartmentStore_5F/` (map.json object event + scripts.inc), rather than a one-line edit — no pre-existing accessible mart slot for it. List reviewed against the evolution-override list below and pruned from 48 to 24 items (10 stones, King's Rock, Metal Coat, Dragon Scale, Upgrade, Protector, Electirizer, Magmarizer, Dubious Disc, Reaper Cloth, Whipped Dream, Sachet, Deep Sea Tooth, Deep Sea Scale, Linking Cord) — items whose only consuming species are being switched to level evolutions (Leader's Crest, all 7 Sweets, Black Augurite, Prism Scale, Oval Stone, Cracked/Chipped Pot, Galarica Cuff/Wreath, Tart/Sweet/Syrupy Apple, both Teacups, Metal Alloy, both Scrolls, both Armors, Peat Block) were dropped since they'd have no use left in the game | Done |

### Data edits (existing systems, no new engine, but real work)

| Feature | Notes |
|---|---|
| ~60 species evolution-method overrides | Evolution tables are per-species data (`EVO_LEVEL`, `EVO_ITEM`, `EVO_TRADE`, etc. in `include/constants/pokemon.h`, defined per-species in `src/data/pokemon/species_info/gen_*_families.h`). Purely mechanical. Escavalier/Accelgor need `EVO_ITEM, ITEM_LINKING_CORD` added (no new item required, see below). |
| Regi caves unlock timing | Standard map/event script edit around the existing Sootopolis Rayquaza cutscene flag |
| Every Pokémon learns every TM | TM compatibility is a per-species bitfield in data — can be scripted to set all bits, not intelligent design |

### New systems (no existing hook — real feature work)

| Feature | Why it's new |
|---|---|
| Full species randomizer (wild/starters/trainers, ±10% stat variance, ability whitelist, Shedinja/Ditto special cases, gym/E4/champion exclusion) | No randomizer engine exists in this codebase at all |
| Moveset randomizer (21 moves, STAB/non-STAB/status split, power curve to Lv63) | Same — would replace the per-species level-up learnset arrays |
| Gift/TM item randomization | No flag; needs a hook into give-item scripts and TM pickup tables |
| EVs blocked outside vitamins/feathers/gym statics | No flag; requires gating the EV-award path in `battle_util.c` by trainer class |
| Starter 5-perfect-IV guarantee | No flag today, but `P_LEGENDARY_PERFECT_IVS` (3 IVs for legendaries) in `pokemon.h` is the exact precedent to extend |
| QOL key-item kit replacing/augmenting the Running Shoes pickup, stripped on whiteout | New items + edit to that event script + a whiteout hook |
| Permanent-faint marking (nuzlocke tracker) | New system — worth checking published decomp nuzlocke patches before writing from scratch |
| Regional-form evolution randomization | No existing branch point at evolution time |
