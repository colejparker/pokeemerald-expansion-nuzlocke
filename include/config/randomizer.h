#ifndef GUARD_CONFIG_RANDOMIZER_H
#define GUARD_CONFIG_RANDOMIZER_H

// Master switch. If FALSE, none of the randomizer code is compiled in.
#define RANDOMIZER_AVAILABLE   TRUE

#if RANDOMIZER_AVAILABLE == TRUE

// Per-feature toggles. Each is a save-persistent flag so the randomizer can
// eventually be turned on/off from a menu; for now they're flipped via the
// overworld debug menu (hold R + START) while each phase is being built out.
#define RANDOMIZER_FLAG_WILD_MON               FLAG_RANDOMIZER_WILD_MON
#define RANDOMIZER_FLAG_TRAINER_MON            FLAG_RANDOMIZER_TRAINER_MON
#define RANDOMIZER_FLAG_STARTER_AND_GIFT_MON   FLAG_RANDOMIZER_STARTER_AND_GIFT_MON
#define RANDOMIZER_FLAG_FIELD_ITEMS            FLAG_RANDOMIZER_FIELD_ITEMS
#define RANDOMIZER_FLAG_ABILITIES              FLAG_RANDOMIZER_ABILITIES

// A randomized species must have a base stat total within this percent of the
// original species' BST (gameplan.md: "within 10% up or down of base stats").
#define RANDOMIZER_BST_LENIENCY_PERCENT    10

#endif // RANDOMIZER_AVAILABLE

#endif // GUARD_CONFIG_RANDOMIZER_H
