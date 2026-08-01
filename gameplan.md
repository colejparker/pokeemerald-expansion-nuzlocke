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
    - [x] Every Pokemon can learn every TM and every HM (`src/pokemon.c` `CanLearnTeachableMove` — any move that currently maps to a TM or HM item via `GetTMHMItemIdFromMoveId` is universally learnable regardless of species; Move Tutor moves stay species-gated by the existing per-species `teachableLearnset` list. Deliberately keyed off "is this move taught by some TM/HM slot" rather than a hardcoded move list, so it stays correct once TM-to-move mappings get shuffled per-run)
- [x] Remove move tutors (the 10 vanilla Hoenn move-tutor NPCs — Swagger/Slateport Fan Club, Rollout/Mauville, Fury Cutter/Verdanturf, Mimic/Lavaridge, Metronome/Fallarbor, Sleep Talk/Fortree, Substitute/Lilycove Rooftop, Dynamic Punch/Mossdeep, Double-Edge/Sootopolis, Explosion/Pacifidlog — deleted their `object_events` entries from the 8 affected maps' `map.json` files (some NPCs share a map). `data/scripts/move_tutors.inc` and `data/text/move_tutors.inc` pruned down to just the shared "choose a box mon and teach it a move" framework (`MoveTutor_EventScript_OpenPartyMenu/OpenBox/CanOnlyBeLearnedOnce`, `MoveTutor_AfterChooseBoxMon`), since `src/chooseboxmon.c`'s `SELECT_PC_MON_MOVE_TUTOR` table entry still references `MoveTutor_AfterChooseBoxMon` at the C level regardless of whether any script calls it — the per-NPC dialogue/teach scripts and their text were deleted since nothing else referenced them. Left the FRLG-side move tutors (`move_tutors_frlg.inc`) and the `FLAG_MOVE_TUTOR_TAUGHT_*` flag IDs alone — the former are unreachable Kanto-only content in this Hoenn-only hack, the latter are harmless unused flag reservations not worth renumbering)
- Items are randomized
    - Any item given to the player is randomized from a list, with the exception of HM's and the key items needed for story progression
    - All berry trees are replaced by items a player can pick-up
    - The TM's a player can get are randomized, HM's are kept the same
    - Marts do not have the items they can sell randomized
- [x] Pokemon are level capped
    - They cannot level pas the following caps:
        - 0 badges earned: 15
        - 1 badges earned: 19
        - 2 badges earned: 24
        - 3 badges earned: 29
        - 4 badges earned: 31
        - 5 badges earned: 33
        - 6 badges earned: 42
        - 7 badges earned: 50
        - 8 badges earned: 63
    - [ ] Trainers are adjusted for the new level caps (not yet done — trainer parties still use vanilla levels)
    - Tested via debug menu (badge-flag toggling across all 9 tiers)
- Players are given key items for QOL
    - When leaving Littleroot for the first time when players would normally receive sneakers, they instead receive the following key items:
        - An Infinite Repel
        - A porta heal that heals all of their Pokemon to full HP, PP and status
        - An endless Rare Candy
        - A cap candy that instantly brings a Pokemon to the level cap
            - It does not skip learning moves or evolving the Pokemon
    - Players still start with Sneakers
    - Whiting out removes these key items
- [x] Wild pokemon and trainers pokemon do not give EVs (`src/pokemon.c` `MonGainEVs` — added an early-return gate: EVs are only awarded when `BATTLE_TYPE_TRAINER` is set, the opponent isn't a special/link trainer (`IsSpecialTrainer`), and `GetTrainerClassFromId` on the opponent returns `TRAINER_CLASS_LEADER`. This is the single choke point both post-battle EV-gain callsites in `battle_script_commands.c` already route through, so gating it here covers wild battles and every non-Gym trainer at once. "Gym statics" in the original bullet turned out to just mean the Gym Leader trainer battle itself — Hoenn's 8 gym leaders (Roxanne through Juan/Wallace, including rematches) are the only trainers using `TRAINER_CLASS_LEADER`, confirmed via `src/data/trainers.h`. Vitamins/feathers are a separate item-effect code path entirely and were untouched by this change)
    - [x] The static encounters of gyms DO give EVs (this refers to the Gym Leader trainer battle itself, not a literal wild "static encounter" — see above)
    - [x] Pokemon can get EV's only through vitamins, feathers and Gym Battles
    - Tested via debug menu (Gym Leader battle vs. regular trainer/wild battle, "Check EVs" before/after)
- [x] Starter Pokemon always start with 5 perfect IV's (`src/battle_setup.c` `CB2_GiveStarter` — builds the starter mon itself instead of delegating to `ScriptGiveMon`, then calls `SetBoxMonPerfectIVs(&mon.box, 5)` before handing it to the player. Deliberately done at the give-site rather than via the species' `.perfectIVCount` field, since that field is shared with wild/gift encounters of the same species and would leak the guarantee outside the starter pick). Tested via a fresh New Game + "Check IVs" on the received starter
- [x] Bag is disabled in trainer battles only (still usable in wild battles)
- [x] When a Pokemon faints, it is marked permanently (cosmetic marker only — a permanently-fainted mon still shows its usual status icon everywhere, e.g. party menu list, summary screen, but can still technically be selected/used if the player chooses to. Implemented by co-opting Pokérus's storage rather than adding a new save field: `P_POKERUS_ENABLED` set `FALSE` in `include/config/pokerus.h` (kills all vanilla infection/spread/EV-double behavior, but leaves the underlying per-mon byte, `MON_DATA_POKERUS`, fully readable/writable — that byte survives evolution/trade/egg-hatch/link-battle-sync for free since that plumbing already existed). Added `IsMonPermanentlyFainted`/`MarkMonPermanentlyFainted` in `src/pokerus.c` reading/writing that byte directly (bypassing the now-dead `CheckMonPokerus`/`CheckMonHasHadPokerus` gated helpers). Hooked the mark into `SetValuesOnFaint` in `src/battle_util.c` (the single choke point both player- and opponent-faint handling already route through) — only the player-side branch marks, via `MarkMonPermanentlyFainted(GetBattlerMon(battler))`, so it fires on every player faint in every battle type (wild included). For the visual marker, reused the existing "FNT" (currently-fainted, 0 HP) status icon instead of drawing new art — `GetMonAilment` in `src/party_menu.c` now returns `AILMENT_FNT` whenever `IsMonPermanentlyFainted` is true, regardless of current HP, so a permanently-fainted mon keeps showing as fainted forever even after being healed at a Pokémon Center. `pokemon_summary_screen.c` picks this up for free since it calls the same `GetMonAilment`. The debug menu's existing "Give/Clear/Set Pokérus" tools still directly touch the same raw byte — left as-is, doubles as a handy manual way to flag/unflag a mon for testing). Tested via debug menu ("Faint Pokemon" then "Heal party", confirmed the FNT icon persists in party list, summary screen, and PC box)
- [x] Regirock, Registeel, and Regice become available in their unlocked caves after Rayquaza sends Kyogre and Groudon away in Sootopolis (right before the 8th gym). `VAR_SOOTOPOLIS_CITY_STATE` reaches 5 exactly at that story beat (set in `data/maps/SkyPillar_Top/scripts.inc` when Rayquaza is summoned and sends Kyogre/Groudon away — confirmed via the full state timeline: 1 at Seafloor Cavern awakening, 2 back in Sootopolis, 3 entering Cave of Origin, 4 at Sky Pillar's base, 5 at the top with Rayquaza, 6 only after beating the Gym). Added a `VAR_SOOTOPOLIS_CITY_STATE < 5` check to the cave-entrance-interaction scripts in `data/maps/DesertRuins`, `AncientTomb`, and `IslandCave` (both the middle and side entrance triggers in each) that blocks the braille puzzle with a new shared message (`gText_RegiEntranceSealed` in `data/event_scripts.s`) until that story point is reached
    - [ ] They are still randomized (depends on the not-yet-built species randomizer)
- [x] ~~New item called Trade Cable that evolves all Pokemon that would otherwise need to be traded to evolve~~ Not needed — pokeemerald-expansion already ships `ITEM_LINKING_CORD` for exactly this (Kadabra, Machoke, Graveler/Graveler-Alola, Haunter, Boldore, Gurdurr, Phantump, Pumpkaboo forms already evolve on it). Use Linking Cord instead of building a new item.
- [x] All Pokemon that need to evolve by trading a specific item instead evolve when given that item — already true by default for Onix (Metal Coat) and Feebas/Milotic before this hack's Milotic override; no engine work needed
- [x] Pokemon that would normally not be able to evolve without specific conditions are instead changed to level evolutions or easier evolution methods. Implemented across `src/data/pokemon/species_info/gen_*_families.h`. Random-split evolutions use the codebase's existing PID-modulo-100 mechanism (a fixed, per-individual "random" split, same trick the vanilla data already uses for Maushold/Dudunsparce) — even odds for the splits below unless noted otherwise. These Pokemon are:
    - Leafeon (Leaf Stone) — moved onto Eevee's main item list, dropped the Petalburg Woods map-location alternative
    - Glaceon (Ice Stone) — same, dropped the Shoal Cave map-location alternative
    - Kingambit (Level 63) — dropped the "defeat 3 Leader's Crest Bisharp" requirement; found and fixed a second, duplicate "Kingambit (Level 50)" entry later in this same list (removed per your call, since Level 63 already covers it)
    - Mamoswine (Level 47)
    - Lickilicky (Level 43)
    - Tangrowth (Level 43)
    - Ambipom (Level 43)
    - Yanmega (Level 43)
    - Tsareena (Level 36)
    - Naganadel (Level 50)
    - Grapploct (Level 36)
    - Overqwil (Level 43)
    - Wyrdeer (Level 43)
    - Annihilape (Level 50)
    - Fargiraf (Level 32)
    - Appletun/Flapple/Dipplin (Level 29, random 3-way even split) — dropped Tart/Sweet/Syrupy Apple requirement entirely
    - Hydrapple (Level 43)
    - Dudunsparce (Level 32) — kept the existing 99/1 real-game-rarity PID split for Two-Segment/Three-Segment, just moved it off the Hyper Drill move requirement onto a flat level
    - Sirfetch'd (Level 36)
    - Milotic (Level 36) — dropped the beauty/Prism-Scale-trade requirement
    - Palafin (Level 38) — already matched, no change needed
    - Gholdengo (Level 47) — both Gimmighoul forms (Chest and Roaming) updated, dropped the 999-coin requirement
    - Malamar (Level 30) — already matched, no change needed
    - Escavalier (Linking Cord) — was the one non-default trade evolution (partner-species-gated, not item-gated like Kadabra/Machoke/etc.); now `EVO_ITEM, ITEM_LINKING_CORD`
    - Accelgor (Linking Cord) — same fix
    - Mr. Mime/Galarian Mr. Mime (Level 30) — dropped the "must know Mimic" requirement, kept the region check so the regional-variant split still works correctly
    - Mantine (Level 21) — dropped the "Remoraid in party" requirement
    - Urshifu Single Strike (Level 47 while knowing a Dark move) — `IF_KNOWS_MOVE_TYPE, TYPE_DARK`, replacing the Scroll of Darkness item
    - Urshifu Rapid Style Strike (Level 47 while knowing a Water move) — `IF_KNOWS_MOVE_TYPE, TYPE_WATER`, replacing the Scroll of Waters item
    - Pikachu (Level 15) — both regular and Alolan Raichu branches updated, region check preserved
    - Togepi (Level 15)
    - Clefairy (Level 15) — dropped Moon Stone requirement
    - Marill (Level 15)
    - Roselia (Level 15)
    - Chingling (Level 15)
    - Sudowoodo (Level 21)
    - Chansey (Level 15) — dropped the Oval-Stone/daytime requirement
    - Snorlax (Level 24)
    - Jigglypuff (Level 15)
    - Blissey (Level 43)
    - Lucario (Level 29)
    - Crobat (Level 36)
    - Espeon (Moon Stone) — moved onto Eevee's item list, replacing the friendship+daytime condition
    - Umbreon (Sun Stone) — same, replacing friendship+nighttime
    - Sylveon (Shiny Stone) — same, replacing friendship+Fairy-move-known
    - Lopunny (Level 21)
    - Swoobat (Level 22)
    - Leavanny (Level 36)
    - Alolan Persian (Level 28)
    - Silvally (Level 43)
    - Frosmoth (Level 29)
    - Rabsca (Level 29) — dropped the 1000-overworld-steps requirement
    - Goodra (Level 50) — both weather-conditioned branches (rain/fog) collapsed into one, since they both led to the same species anyway
    - Galarian Slowking/Slowking (King's Rock) — both now use King's Rock directly. Also revisited Galarian Slowbro while here: it no longer needs Galarica Cuff either, now a plain Level 37 evolution (same level as regular Slowbro) since Galarian Slowpoke is its own distinct species from regular Slowpoke — Galarica Cuff dropped from the Lilycove shop as fully unused
    - Kleavor/Scizor (Randomly at 43, even split) — dropped the Metal Coat/Black Augurite item requirements
        - [x] Tested va debug and its working
    - Ursaluna/Blood Moon Ursaluna (Random at Level 50, even split) — dropped the Peat Block/Hisui-region/nighttime requirement
    - Armarouge/Ceruledge (Random at Level 36, even split) — dropped the Auspicious/Malicious Armor item requirement
    - Polteageist (Level 36) — both Phony and Antique Sinistea variants keep evolving into their respective Polteageist form, just via level instead of Cracked/Chipped Pot
    - Sinistcha (Level 36) — same treatment for Poltchageist's Counterfeit/Artisan variants and the Unremarkable/Masterpiece Teacup items
    - Archaludon (Level 63) — dropped Metal Alloy requirement
    - Runerigus (Level 34) — dropped the curse-damage-threshold requirement
    - Alcremie (Level 30, random form) — this one needed a scope correction: the codebase actually implements 63 distinct Alcremie species (7 sweets × 9 cream/swirl patterns), not 9. Per your call, narrowed the random pool to the 7 "Vanilla Cream" flavor-base forms (one per sweet), 7-way near-even split (~14-15% each), dropping the hold-item/spin-direction/time-of-day mechanic entirely
    - Basculegion (Level 50) — kept the Male/Female split (that's a real distinct-species mechanic, not a difficulty gate), dropped the recoil-damage requirement
    - Melmetal (Level 63) — Meltan had no evolution data at all by default; added a new `.evolutions` field
    - Hydreigon (Level 63) — was already Level 64, adjusted down by 1
    - Solgaleo/Lunala (Randomly at 53, even split) — dropped the day/night requirement
    - Lycanroc forms (Randomly at 25, 3-way even split) — unified onto the regular Rockruff (previously Dusk form required a separate rare "Own Tempo" Rockruff variant, which would have been awkward to guarantee access to under this hack's design); left the Own Tempo variant's dedicated block as a harmless bonus guaranteed-Dusk path
- [x] All pokemon that could evolve into a regional form, the evolution is determined randomly. Found the root issue: this game is Hoenn-only, so the "player's current region" is always Hoenn — any evolution branch gated on `IF_REGION, REGION_ALOLA/GALAR/HISUI` could never trigger, and the paired `IF_NOT_REGION` branch always won by default, making 12 regional forms completely unreachable (Raichu-Alola, Exeggutor-Alola, Marowak-Alola, Weezing-Galar, Mr. Mime-Galar, Typhlosion-Hisui, Samurott-Hisui, Lilligant-Hisui, Braviary-Hisui, Decidueye-Hisui, Sliggoo-Hisui, Avalugg-Hisui). Fixed by swapping the region conditions for the same PID-modulo even-split mechanism used elsewhere in this pass, preserving any other conditions those branches had (Marowak-Alola keeps its nighttime requirement, Decidueye-Hisui keeps its different level-36-vs-34 requirement). Separately audited every regional-form species itself (Vulpix-Alola, Growlithe-Hisui, Sliggoo-Hisui, etc.) to confirm none evolve into a base/non-regional form — all 17 checked out clean, no bugs found there, so "regional forms only evolve into regional forms" was already true
- [x] Shop in Lilycove that sells all the evolution items (new clerk NPC added to the Department Store 5F, `LilycoveCity_DepartmentStore_5F_EventScript_ClerkEvolutionItems`; sells all stones + special evolution items still needed by any species, including Linking Cord; NPC placed at an inferred-clear tile (11,2) — verify in-game and reposition if blocked)
- Side content removed to streamline playthrough
- [x] Set-up moves have their PP reduced (`src/data/moves_info.h` — `.pp` set directly on each move, replacing any vanilla ternary based on `B_UPDATED_MOVE_DATA`)
    - Acupressure 3
    - Agility 2
    - Autotomize 2
    - Bulk Up 2
    - Calm Mind 2
    - Coil 2
    - Curse 3
    - Dragon Dance 2
    - Growth 2
    - Howl 2
    - Hone Claws 2
    - Meditate 2
    - Nasty Plot 2
    - No Retreat 2
    - Quiver Dance 2
    - Rock Polish 2
    - Shell Smash 2
    - Shift Gear 2
    - Sword Dance 2
    - Take Heart 2
    - Tail Glow 2
    - Tidy Up 2
    - Victory Dance 2
- Restricted legendaries are removed from the pool
    - Arceus
    - Kyurem
    - Zacian
    - Zamazenta
    - Eternatus
    - Mewtwo
    - Lugia
    - Ho-Oh
    - Rayqauza
    - Dialga
    - Palkia
    - Giratina
    - Reshirom
    - Zekrom
    - Solgaleo
    - Luala
    - Hoopa Unbound
    - Necrozma
    - Calyrex Rider Forms
    - Kyogre
    - Groudon
    - Regigigas
    - Koraidon
    - Miraidon
- Spore as a move is removed from the pool
- [x] Innards Out reveals itself as the ability when a Pokemon is sent out with it (`src/battle_util.c` `AbilityBattleEffects` — added a new `ABILITY_INNARDS_OUT` case under `ABILITYEFFECT_ON_SWITCHIN` that just calls `BattleScriptCall(BattleScript_AbilityPopUp)`, following the same `gEffectBattler = gBattlerAbility = battler;` pop-up idiom used by every other switch-in ability reveal in that switch. Previously it only revealed itself when its damage-back effect actually triggered on faint, under `ABILITYEFFECT_MOVE_END`, which is untouched and still fires normally)
- [x] Types are shown next to the Pokemon's name when Battle is selected during a fight. (`include/config/battle.h` — `B_SHOW_TYPES` flipped from `SHOW_TYPES_NEVER` to `SHOW_TYPES_ALWAYS`)

## Implementation Audit

### Already built-in (just flip a flag / one-line edit)

| Feature | Where | Status |
|---|---|---|
| Bag disabled in trainer battles | `include/config/battle.h` — `B_VAR_NO_BAG_USE` now points at `VAR_NO_BAG_USE` (renamed from `VAR_UNUSED_0x404E`); set to `NO_BAG_AGAINST_TRAINER` (wild battles keep the bag) in `src/new_game.c` on new game and re-asserted in `src/overworld.c` `Overworld_ResetStateAfterWhiteOut` after whiteout | Done |
| Level caps | `include/config/caps.h` — `B_LEVEL_CAP_TYPE = LEVEL_CAP_FLAG_LIST`, `B_EXP_CAP_TYPE = EXP_CAP_HARD`, `B_RARE_CANDY_CAP = TRUE`; `sLevelCapFlagMap` in `src/caps.c` updated to the 8 badge thresholds (14/21/24/29/36/43/47/50) with a 63 cap through the Champion | Done |
| Starting money | Not a config flag, but hardcoded to one call: `src/new_game.c` `SetMoney(&gSaveBlock1Ptr->money, 750000);` (lowered from 999,999 for money-cap headroom) | Done |
| Lilycove evolution-item shop | New clerk NPC + `pokemart` item list added to `data/maps/LilycoveCity_DepartmentStore_5F/` (map.json object event + scripts.inc), rather than a one-line edit — no pre-existing accessible mart slot for it. List settled at 24 items (10 stones, King's Rock, Metal Coat, Dragon Scale, Upgrade, Protector, Electirizer, Magmarizer, Dubious Disc, Reaper Cloth, Whipped Dream, Sachet, Deep Sea Tooth, Deep Sea Scale, Linking Cord) — every other special evolution item (Leader's Crest, all 7 Sweets, Black Augurite, Prism Scale, Oval Stone, Cracked/Chipped Pot, Galarica Cuff/Wreath, Tart/Sweet/Syrupy Apple, both Teacups, Metal Alloy, both Scrolls, both Armors, Peat Block) is fully unused now that every consuming species is on a level evolution. Galarica Cuff briefly went back and forth (removed, re-added for Galarian Slowbro, then removed again once Galarian Slowbro also moved to a plain level evolution) before landing here | Done |

### Data edits (existing systems, no new engine, but real work)

| Feature | Notes |
|---|---|
| ~60 species evolution-method overrides | Done. All entries across `src/data/pokemon/species_info/gen_*_families.h` — see the checked-off list above for per-species details. Random-split evolutions use the codebase's existing PID-modulo-100 mechanism (same trick vanilla Maushold/Dudunsparce already use), even splits per your call. Alcremie's scope was corrected mid-implementation (63 actual species, not 9 as originally assumed) and narrowed to a 7-way split per your call. Melmetal needed a net-new `.evolutions` field since Meltan had none. Also fixed a duplicate Kingambit entry and a mislabeled Escavalier/Accelgor "Trade Cable" reference found along the way. |
| Regi caves unlock timing | Done. Gated on `VAR_SOOTOPOLIS_CITY_STATE >= 5` in `data/maps/{DesertRuins,AncientTomb,IslandCave}/scripts.inc` |
| Every Pokémon learns every TM and every HM | Done. Not a bitfield in this codebase — TM/HM/tutor compatibility is an explicit per-species move list (`teachableLearnset`). Fixed in the check function (`CanLearnTeachableMove`) instead of editing every species' list, so it applies uniformly and stays correct regardless of future TM/move changes |

### New systems (no existing hook — real feature work)

| Feature | Why it's new |
|---|---|
| Full species randomizer (wild/starters/trainers, ±10% stat variance, ability whitelist, Shedinja/Ditto special cases, gym/E4/champion exclusion) | No randomizer engine exists in this codebase at all |
| Moveset randomizer (21 moves, STAB/non-STAB/status split, power curve to Lv63) | Same — would replace the per-species level-up learnset arrays |
| Gift/TM item randomization | No flag; needs a hook into give-item scripts and TM pickup tables |
| EVs blocked outside vitamins/feathers/gym statics | No flag; requires gating the EV-award path in `battle_util.c` by trainer class |
| Starter 5-perfect-IV guarantee | Done. `SetBoxMonPerfectIVs` (the same helper `.perfectIVCount`/`P_LEGENDARY_PERFECT_IVS` uses under the hood) called directly in `CB2_GiveStarter` rather than through the species field, so it's scoped to the starter pick only |
| QOL key-item kit replacing/augmenting the Running Shoes pickup, stripped on whiteout | New items + edit to that event script + a whiteout hook |
| Permanent-faint marking (nuzlocke tracker) | New system — worth checking published decomp nuzlocke patches before writing from scratch |
