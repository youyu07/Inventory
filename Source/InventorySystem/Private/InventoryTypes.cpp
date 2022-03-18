#include "InventoryTypes.h"

#define LOCTEXT_NAMESPACE "Inventory"

TMap<FName, UInventoryItemInfo*> UInventoryItemInfo::Map = TMap<FName, UInventoryItemInfo*>();

UInventoryItemInfo::UInventoryItemInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

UInventoryItemInfo* UInventoryItemInfo::Find(FName InId)
{
	if (Map.Contains(InId)) return Map[InId];
	return nullptr;
}


bool UInventoryItem::Move(FIntPoint ToLocation, UInventoryItem*& BelowItem, UInventoryItemArea* ToArea, bool bTest)
{
	auto NextArea = ToArea ? ToArea : Area;
	if (!NextArea->IsCanAcceptTypes(Info->Types))return false;

	bool OutBound = false;
	auto Items = NextArea->GetUnderItems(ToLocation, Info->Size, OutBound);
	if (Items.Num() > 1 || OutBound) {
		return false;
	}
	BelowItem = Items[0];
	if (!bTest) {
		Area->RemoveItem(this);
		Area = ToArea;
		Area->AddItem(this, ToLocation);
	}

	return true;
}

void UInventoryItem::Delete()
{
	Area->RemoveItem(this);
}


UInventoryItemArea* UInventoryItemArea::Make(UObject* WorldContextObject, FName Area, FIntPoint InLayout)
{
	auto World = WorldContextObject->GetWorld();
	check(World);

	auto AreaObject = Find(WorldContextObject, Area);
	if (!AreaObject) {
		FName FullName = *FString::Printf(L"InventoryItemArea_%s", *Area.ToString());
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
	AreaObject->Items.SetNum(InLayout.X * InLayout.Y);
	return AreaObject;
}

UInventoryItemArea* UInventoryItemArea::Find(UObject* WorldContextObject, FName Area)
{
	check(WorldContextObject->GetWorld());

	FName FullName = *FString::Printf(L"InventoryItemArea_%s", *Area.ToString());
	for (TObjectIterator<UInventoryItemArea> It; It; ++It)
	{
		if (It->GetFName().IsEqual(FullName, ENameCase::CaseSensitive, false) && It->GetWorld() == WorldContextObject->GetWorld()) return *It;
	}
	return nullptr;
}


UInventoryItem* UInventoryItemArea::MakeItem(UInventoryItemInfo* Info, FIntPoint InLocation)
{
	if (!IsCanAcceptTypes(Info->Types))return nullptr;
	bool OutBound = false;
	if (GetUnderItems(InLocation, Info->Size, OutBound).Num() > 0 || OutBound) {
		return nullptr;
	}

	auto Id = Info->GetFName();

	FName FullName = *FString::Printf(L"InventoryItem%s", *Id.ToString());
	FullName = MakeUniqueObjectName(this, UInventoryItemArea::StaticClass(), FullName);
	auto Obj = NewObject<UInventoryItem>(this, UInventoryItem::StaticClass(), FullName);
	Obj->Area = this;
	Obj->Info = Info;

	AddItem(Obj, InLocation);
	return Obj;
}

const TArray<UInventoryItem*> UInventoryItemArea::GetUnderItems(FIntPoint Location, FIntPoint Size, bool& bOutBound) const
{
	TSet<UInventoryItem*> Temp;
	for (int32 y = Location.Y; y < Location.Y + Size.Y; y++)
	{
		for (int32 x = Location.X; x < Location.X + Size.X; x++)
		{
			auto Index = y * Layout.X + x;
			if (Items[Index]) Temp.Add(Items[Index]);
		}
	}
	bOutBound = (Location.X + Size.X > Layout.X) || (Location.Y + Size.Y > Layout.Y);

	return Temp.Array();
}


bool UInventoryItemArea::FindLocation(FIntPoint Size, FIntPoint& OutLocation) const
{
	auto IsBlank = [&](FIntPoint Location) {
		for (int32 y = Location.Y; y < Location.Y + Size.Y; y++)
		{
			for (int32 x = Location.X; x < Location.X + Size.X; x++)
			{
				auto Index = y * Layout.X + x;
				if (Items[Index]) return false;
			}
		}
		return true;
	};

	for (int32 y = 0; y < Layout.Y; y++)
	{
		for (int32 x = 0; x < Layout.X; x++)
		{
			auto Loc = FIntPoint(x, y);
			if (IsBlank(Loc)) {
				OutLocation = Loc;
				return true;
			}
		}
	}
	
	return false;
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


const TMap<UInventoryItem*, FIntPoint> UInventoryItemArea::GetItems() const
{
	TMap<UInventoryItem*, FIntPoint> Map;
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!Items[i])continue;
		auto Y = i / Layout.X;
		auto X = i % Layout.X;

		if (!Map.Contains(Items[i])) {
			Map.Add(Items[i], FIntPoint(X, Y));
		}
	}
	return MoveTemp(Map);
}


void UInventoryItemArea::AddItem(UInventoryItem* Item, const FIntPoint& InLocation)
{
	for (int32 y = InLocation.Y; y < InLocation.Y + Item->Info->Size.Y; y++)
	{
		for (int32 x = InLocation.X; x < InLocation.X + Item->Info->Size.X; x++)
		{
			Items[y * Layout.X + x] = Item;
		}
	}
}

void UInventoryItemArea::RemoveItem(UInventoryItem* Item)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i] == Item)Items[i] = nullptr;
	}
}


#undef LOCTEXT_NAMESPACE