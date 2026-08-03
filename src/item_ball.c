#include "global.h"
#include "item_ball.h"
#include "event_data.h"
#include "randomizer.h"
#include "constants/event_objects.h"
#include "constants/items.h"

static u32 GetItemBallAmountFromTemplate(u32);
static u32 GetItemBallIdFromTemplate(u32);

static u32 GetItemBallAmountFromTemplate(u32 itemBallId)
{
    u32 amount = gMapHeader.events->objectEvents[itemBallId].movementRangeX;

    if (amount > MAX_BAG_ITEM_CAPACITY)
        return MAX_BAG_ITEM_CAPACITY;

    return (amount == 0) ? 1 : amount;
}

static u32 GetItemBallIdFromTemplate(u32 itemBallId)
{
    enum Item itemId = gMapHeader.events->objectEvents[itemBallId].trainerRange_berryTreeId;

    if (itemId >= ITEMS_COUNT)
        return ITEM_NONE + 1;

    return RandomizeFieldItem(itemId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, itemBallId);
}

void GetItemBallIdAndAmountFromTemplate(void)
{
    u32 itemBallId = (gSpecialVar_LastTalked - 1);
    gSpecialVar_Result = GetItemBallIdFromTemplate(itemBallId);
    gSpecialVar_0x8009 = GetItemBallAmountFromTemplate(itemBallId);
}

// Called by Std_ObtainItem (the `giveitem` script command) before the item
// actually gets added to the bag, so NPCs that hand the player an item
// directly get the same randomization as item balls/hidden items.
void RandomizeGivenItem(void)
{
    gSpecialVar_0x8000 = RandomizeNpcGiftItem(gSpecialVar_0x8000, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
}
