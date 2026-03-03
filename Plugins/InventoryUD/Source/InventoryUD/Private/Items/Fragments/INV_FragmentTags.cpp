
#include "Items/Fragments/INV_FragmentTags.h"


namespace Fragments
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Grid, "Fragments.Grid", "Grid Fragment Tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Icon, "Fragments.Icon", "Icon Fragment Tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stackable, "Fragments.Stackable", "Stackable Fragment Tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Consumable, "Fragments.Consumable", "Consumable Fragment Tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ItemName, "Fragments.ItemName", "ItemName Fragment Tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PrimaryStat, "Fragments.PrimaryStat", "PrimaryStat Fragment Tag")

		namespace StatMod
	{
		UE_DEFINE_GAMEPLAY_TAG(StatMod_1, "Fragments.StatMod.1")
			UE_DEFINE_GAMEPLAY_TAG(StatMod_2, "Fragments.StatMod.2")
			UE_DEFINE_GAMEPLAY_TAG(StatMod_3, "Fragments.StatMod.3")
	}
}
