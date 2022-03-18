#include "InventorySubsystem.h"
#include "InventoryTypes.h"
#include "AssetRegistryModule.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif


#define LOCTEXT_NAMESPACE "Inventory"


void UInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UInventoryItemInfo::Map.Empty();
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> Assets;
	AssetRegistryModule.Get().GetAssetsByClass(UInventoryItemInfo::StaticClass()->GetFName(), Assets, true);

#if WITH_EDITOR
	TSet<UObject*> DuplicateObject;
#endif

	for (auto& A : Assets)
	{
		auto InfoObject = A.GetAsset();

#if WITH_EDITOR
		if (UInventoryItemInfo::Map.Contains(A.AssetName)) {
			DuplicateObject.Add(UInventoryItemInfo::Map[A.AssetName]);
			DuplicateObject.Add(InfoObject);
		}
#endif
		UInventoryItemInfo::Map.Add(A.AssetName, Cast<UInventoryItemInfo>(InfoObject));
	}

#if WITH_EDITOR
	if (DuplicateObject.Num() > 0) {
		FMessageLog AssetCheckLog("AssetCheck");
		const FText Message = LOCTEXT("ItemInfoCheck", "Item Info has duplicate!");

		auto TolenedMessage = AssetCheckLog.Error(Message);
		for (auto& O : DuplicateObject)
		{
			TolenedMessage->AddToken(FUObjectToken::Create(O));
		}
		AssetCheckLog.Notify(Message, EMessageSeverity::Error, true);
	}
#endif
}

#undef LOCTEXT_NAMESPACE