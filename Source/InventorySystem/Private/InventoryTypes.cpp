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

UInventoryItemArea* UInventoryItemArea::Make(UObject* WorldContextObject, FName Area, FIntPoint InLayout)
{
	auto World = WorldContextObject->GetWorld();
	check(World);

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
	AreaObject->Items.SetNum(InLayout.X * InLayout.Y);
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


UInventoryItem* UInventoryItemArea::MakeItem(UInventoryItemInfo* Info, FIntPoint InLocation, bool bExecDelegate)
{
	if (!IsCanAcceptTypes(Info->Types))return nullptr;
	bool OutBound = false;
	if (GetUnderItems(InLocation, Info->Size, OutBound).Num() > 0 || OutBound) {
		return nullptr;
	}

	auto Id = Info->GetFName();

	FName FullName = FName(*FString::Printf(L"InventoryItem%s", *Id.ToString()), 0);
	FullName = MakeUniqueObjectName(this, UInventoryItemArea::StaticClass(), FullName);
	auto Obj = NewObject<UInventoryItem>(this, UInventoryItem::StaticClass(), FullName);
	Obj->Area = this;
	Obj->Info = Info;

	AddItem(Obj, InLocation, bExecDelegate);
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
			if (Items.IsValidIndex(Index) && Items[Index]) Temp.Add(Items[Index]);
		}
	}
	bOutBound = Location.X < 0 || Location.Y < 0 || (Location.X + Size.X > Layout.X) || (Location.Y + Size.Y > Layout.Y);

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
			if(x + Size.X > Layout.X || y + Size.Y > Layout.Y) continue;
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


void UInventoryItemArea::AddItem(UInventoryItem* Item, const FIntPoint& InLocation, bool bExecDelegate)
{
	for (int32 y = InLocation.Y; y < InLocation.Y + Item->Info->Size.Y; y++)
	{
		for (int32 x = InLocation.X; x < InLocation.X + Item->Info->Size.X; x++)
		{
			Items[y * Layout.X + x] = Item;
		}
	}
	Item->Area = this;
	if (bExecDelegate) {
		FInventoryAreaChangeParam Params;
		Params.Params.Add("Item", Item);
		OnChanged.Broadcast(EInventoryAreaChangeType::Add, Params);
	}
}

bool UInventoryItemArea::RemoveItem(UInventoryItem* Item, bool bExecDelegate)
{
	if (!Items.Contains(Item))return false;
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i] == Item)Items[i] = nullptr;
	}
	Item->Area = nullptr;
	if (bExecDelegate) {
		FInventoryAreaChangeParam Params;
		Params.Params.Add("Item", Item);
		OnChanged.Broadcast(EInventoryAreaChangeType::Remove, Params);
	}
	return true;
}

bool UInventoryItemArea::MoveItem(UInventoryItem* Item, const FIntPoint& ToLocation, bool bExecDelegate)
{
	if (!Items.Contains(Item))return false;

	FIntPoint PreLcation;
	bool bInitialLocation = false;
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i] == Item) {
			Items[i] = nullptr;

			if (!bInitialLocation) {
				bInitialLocation = true;
				PreLcation = FIntPoint(i % Layout.X, i / Layout.X);
			}
		}
	}
	for (int32 y = ToLocation.Y; y < ToLocation.Y + Item->Info->Size.Y; y++)
	{
		for (int32 x = ToLocation.X; x < ToLocation.X + Item->Info->Size.X; x++)
		{
			Items[y * Layout.X + x] = Item;
		}
	}

	if (bExecDelegate) {
		FInventoryAreaChangeParam Params;
		Params.Params.Add("Item", Item);
		Params.Params.Add("FromLocation", PreLcation);
		Params.Params.Add("ToLocation", ToLocation);
		OnChanged.Broadcast(EInventoryAreaChangeType::Remove, Params);
	}
	return true;
}


void UInventoryItemArea::SortItem(bool bExecDelegate)
{
	TSet<UInventoryItem*> Temp;
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i]) {
			Temp.Add(Items[i]);
		}
		Items[i] = nullptr;
	}

	if (Temp.Num() == 0)return;

	Temp.Sort([](const UInventoryItem& A, const UInventoryItem& B) {
		if (A.Info->Size.Y == B.Info->Size.Y) {
			return A.Info->Size.X > B.Info->Size.X;
		}
		return A.Info->Size.Y > B.Info->Size.Y;
	});

	for (auto& i : Temp)
	{
		FIntPoint Location;
		if (FindLocation(i->Info->Size, Location)) {
			AddItem(i, Location, false);
		}
	}

	if (bExecDelegate)OnChanged.Broadcast(EInventoryAreaChangeType::Refresh, FInventoryAreaChangeParam());
}


#undef LOCTEXT_NAMESPACE