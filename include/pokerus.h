#ifndef GUARD_POKERUS_H
#define GUARD_POKERUS_H

u32 GetDaysLeftBasedOnStrain(u32 strain);
void RandomlyGivePartyPokerus(void);
bool32 IsPokerusInParty(void);
bool32 CheckMonPokerus(struct Pokemon *mon);
bool32 CheckMonHasHadPokerus(struct Pokemon *mon);
bool32 ShouldPokemonShowActivePokerus(struct Pokemon *mon);
bool32 ShouldPokemonShowCuredPokerus(struct Pokemon *mon);
void UpdatePartyPokerusTime(u32 days);
void PartySpreadPokerus(void);

// Pokérus is disabled (see config/pokerus.h); its per-mon storage byte (MON_DATA_POKERUS) is
// repurposed here as a permanent-faint marker for the nuzlocke "faint = dead" tracker.
bool32 IsMonPermanentlyFainted(struct Pokemon *mon);
void MarkMonPermanentlyFainted(struct Pokemon *mon);

#endif // GUARD_POKERUS_H
