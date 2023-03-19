#include "InventoryItemFunctionLibrary.h"
#include "InventoryTypes.h"


bool UInventoryItemFunctionLibrary::Combine(UDataTable* InRecipes, UInventoryItemArea* InArea)
{
	check(InRecipes && InArea);
	auto Items = InArea->GetItems();
	if (Items.Num() > 1) {
		TArray<UInventoryItemInfo*> Keys;
		for (auto& it : Items)
		{
			Keys.Add(it.Key->Info);
		}

		Keys.Sort();

		TArray<FInventoryItemRecipeRow*> Rows;
		InRecipes->GetAllRows(L"", Rows);

		for (auto& R : Rows)
		{
			if (R->Source.Num() != Keys.Num()) continue;
			auto Infos = R->Source;
			Infos.Sort();

			if (Keys == Infos) {
				for (auto& it : Items)
				{
					InArea->RemoveItem(it.Key);
				}
				InArea->MakeItem(R->Destination, FIntPoint(0));
				return true;
			}
		}
	}

	return false;
}


bool UInventoryItemFunctionLibrary::Split(UDataTable* InRecipes, UInventoryItemArea* InArea)
{
	check(InRecipes && InArea);
	auto Items = InArea->GetItems();
	if (Items.Num() == 1) {

		TArray<UInventoryItem*> Keys;
		Items.GetKeys(Keys);

		TArray<FInventoryItemRecipeRow*> Rows;
		InRecipes->GetAllRows(L"", Rows);

		for (auto& R : Rows)
		{
			if (R->Destination == Keys[0]->Info) {
				InArea->RemoveItem(Keys[0]);

				for (auto& i : R->Source)
				{
					FIntPoint Location;
					if (InArea->FindLocation(i->Size, Location)) {
						InArea->MakeItem(i, Location);
					}
				}
				return true;
			}
		}
	}

	return false;
}