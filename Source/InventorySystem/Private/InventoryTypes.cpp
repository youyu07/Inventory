#include "InventoryTypes.h"

#define LOCTEXT_NAMESPACE "Inventory"

TMap<FName, UInventoryItemInfo*> UInventoryItemInfo::Map = TMap<FName, UInventoryItemInfo*>();

UInventoryItemInfo::UInventoryItemInfo(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	
}

UInventoryItemInfo* UInventoryItemInfo::Find(FName InId)
{
	if (Map.Contains(InId)) return Map[InId];
	return nullptr;
}

void UInventoryItem::Delete()
{
	Area->RemoveItem(this);
}


UInventoryItemArea* UInventoryItemArea::Make(UObject* WorldContextObject, FName Area, FIntPoint InLayout)
{
	auto World = WorldContextObject->GetWorld();
	if(!World) return nullptr;

	auto AreaObject = Find(WorldContextObject, Area);
	if (!AreaObject) {
		FName FullName = FName(*FString::Printf(L"InventoryItemArea_%s", *Area.ToString()), 0);
		FullName = MakeUniqueObjectName(WorldContextObject, UInventoryItemArea::StaticClass(), FullName);
		AreaObject = NewObject<UInventoryItemArea>(World, FullName);
		World->ExtraReferencedObjects.Add(AreaObject);
	}
	else {
#if WITH_EDITOR
		if (InLayout != AreaObject->Layout) {
			FMessageLog AssetCheckLog("AssetCheck");
			const FText Message = FText::Format(LOCTEXT("AreaLayoutCheck", "Area({0}) Layout Rewritten!"), FText::FromName(Area));
			auto TolenedMessage = AssetCheckLog.Warning(Message);
			AssetCheckLog.Notify(Message, EMessageSeverity::Warning, true);
		}
#endif
	}
	AreaObject->Layout = InLayout;
	return AreaObject;
}

UInventoryItemArea* UInventoryItemArea::Find(UObject* WorldContextObject, FName Area)
{
	check(WorldContextObject->GetWorld());

	FName FullName = FName(*FString::Printf(L"InventoryItemArea_%s", *Area.ToString()), 0);
	for (TObjectIterator<UInventoryItemArea> It; It; ++It)
	{
		if (It->GetFName().IsEqual(FullName, ENameCase::CaseSensitive, false) && It->GetWorld() == WorldContextObject->GetWorld()) return *It;
	}
	return nullptr;
}


UInventoryItem* UInventoryItemArea::MakeItem(UInventoryItemInfo* Info)
{
	auto Id = Info->GetFName();
	FName FullName = FName(*FString::Printf(L"InventoryItem%s", *Id.ToString()), 0);
	FullName = MakeUniqueObjectName(this, UInventoryItem::StaticClass(), FullName);
	auto Obj = NewObject<UInventoryItem>(this, UInventoryItem::StaticClass(), FullName);
	Obj->Area = this;
	Obj->Info = Info;
	return Obj;
}

const TArray<UInventoryItem*> UInventoryItemArea::GetUnderItems(FIntPoint Location, FIntPoint Size) const
{
	TSet<UInventoryItem*> Temp;

	auto Y = FMath::Min(Location.Y + Size.Y, Layout.Y);
	auto X = FMath::Min(Location.X + Size.X, Layout.X);

	for (int32 y = Location.Y; y < Y; y++)
	{
		for (int32 x = Location.X; x < X; x++)
		{
			auto Key = FIntPoint(x, y);
			if (ItemMap.Contains(Key)) {
				Temp.Add(ItemMap[Key]);
			}
		}
	}
	return Temp.Array();
}


bool UInventoryItemArea::FindLocation(FIntPoint Size, FIntPoint& OutLocation) const
{
	return FindLocation(ItemMap, Size, OutLocation);
}


bool UInventoryItemArea::IsCanAcceptTypes(const TSet<FName>& Other) const
{
	if (AcceptTypes.Num() == 0) return true;

	bool Accept = false;
	for (auto& Type : AcceptTypes)
	{
		if (Other.Contains(Type)) {
			Accept = true;
			break;
		}
	}
	return Accept;
}


void UInventoryItemArea::AddItem(UInventoryItem* Item, const FIntPoint& InLocation)
{
	SingleItemMap.Add(InLocation, Item);
	Fill(ItemMap, Item, InLocation);
	Item->Area = this;

	FInventoryAreaChangedEvent Event = {
		EInventoryAreaChangeType::Add,
		Item
	};
	OnChanged.Broadcast(Event);
}

bool UInventoryItemArea::RemoveItem(UInventoryItem* Item)
{
	bool bHas = false;
	for (auto It = SingleItemMap.CreateIterator(); It; ++It)
	{
		if (It.Value() == Item) {
			It.RemoveCurrent();
			bHas = true;
		}
	}
	for (auto It = ItemMap.CreateIterator(); It; ++It)
	{
		if (It.Value() == Item) {
			It.RemoveCurrent();
		}
	}
	if (bHas) {
		Item->Area = nullptr;
		FInventoryAreaChangedEvent Event = {
			EInventoryAreaChangeType::Remove,
			Item
		};
		OnChanged.Broadcast(Event);
	}
	return bHas;
}

bool UInventoryItemArea::MoveItem(UInventoryItem* Item, const FIntPoint& ToLocation)
{
	FIntPoint PreLcation = -1;
	for (auto It = SingleItemMap.CreateIterator(); It; ++It)
	{
		if (It.Value() == Item) {
			It.RemoveCurrent();
			PreLcation = It.Key();
		}
	}
	if (PreLcation != -1) {
		for (auto It = ItemMap.CreateIterator(); It; ++It)
		{
			if (It.Value() == Item) {
				It.RemoveCurrent();
			}
		}
	}

	SingleItemMap.Add(ToLocation, Item);
	Fill(ItemMap, Item, ToLocation);

	FInventoryAreaChangedEvent Event = {
		EInventoryAreaChangeType::Move,
		Item
	};

	/*
	"FromLocation", PreLcation;
	"ToLocation", ToLocation;
	*/

	OnChanged.Broadcast(Event);
	return true;
}


void UInventoryItemArea::Sort()
{
	/*TSet<UInventoryItem*> TempItems;
	for (size_t i = 0; i < Items.Num(); i++)
	{
		TempItems.Add(Items[i]);
	}

	TArray<UInventoryItem*> NewItems;
	NewItems.SetNum(Items.Num());

	FIntPoint Location;

	for (auto& Item : TempItems)
	{
		if (Item == nullptr) continue;
		auto Size = Item->Info->Size;
		if (FindLocation(NewItems, Size, Location)) {
			Fill(NewItems, Item, Location);
		}
	}
	
	Items = NewItems;

	FInventoryAreaChangedEvent Event = {
		EInventoryAreaChangeType::Sort,
		nullptr
	};
	OnChanged.Broadcast(Event);*/
}

bool UInventoryItemArea::OutofBound(const FIntPoint& Location, const FIntPoint& Size) const
{
	bool bInBound = Location.X >= 0 && Location.Y >= 0;
	bInBound = bInBound && (Location.X + Size.X <= Layout.X);
	bInBound = bInBound && (Location.Y + Size.Y <= Layout.Y);
	return !bInBound;
}

bool UInventoryItemArea::IsBlankWithRange(const TMap<FIntPoint, UInventoryItem*>& InMap, const FIntPoint& InLocation, const FIntPoint& InSize) const
{
	for (int32 y = 0; y < InSize.Y; y++)
	{
		for (int32 x = 0; x < InSize.X; x++)
		{
			auto Key = FIntPoint(InLocation.X + x, InLocation.Y + y);
			if (InMap.Contains(Key)) return false;
		}
	}
	return true;
}

bool UInventoryItemArea::FindLocation(const TMap<FIntPoint, UInventoryItem*>& InMap, const FIntPoint& InSize, FIntPoint& OutLocation) const
{
	OutLocation = FIntPoint(-1);
	check(InSize.X > 0 && InSize.Y > 0);

	for (int32 y = 0; y < Layout.Y; y++)
	{
		for (int32 x = 0; x < Layout.X; x++)
		{
			auto Key = FIntPoint(x, y);

			if (InMap.Contains(Key)) continue;
			if (OutofBound(Key, InSize)) continue;

			if (IsBlankWithRange(InMap, Key, InSize)) {
				OutLocation = Key;
				return true;
			}
		}
	}

	return false;
}

void UInventoryItemArea::Fill(TMap<FIntPoint, UInventoryItem*>& InMap, UInventoryItem* InItem, const FIntPoint& InLocation)
{
	auto Size = InItem->Info->Size;

	for (int32 y = 0; y < Size.Y; y++)
	{
		for (int32 x = 0; x < Size.X; x++)
		{
			InMap.Add(FIntPoint(InLocation.X + x, InLocation.Y + y), InItem);
		}
	}
}


#undef LOCTEXT_NAMESPACE