#include "InventoryAssetEditorSubsystem.h"


#if WITH_EDITOR

#include "AssetRegistry/IAssetRegistry.h"
#include "InventoryTypes.h"


void UInventoryAssetEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	IAssetRegistry::GetChecked().OnInMemoryAssetCreated().AddLambda([](const FAssetData& Asset) {
		if (auto Info = Cast<UInventoryItemInfo>(Asset.GetAsset())) {
			Info->Attributes.Add(FName("Name"), FAny(FText()));
			Info->Attributes.Add(FName("Icon"), FAny(FSlateBrush()));
			Info->Attributes.Add(FName("Color"), FAny(FLinearColor::Transparent));
		}
	});
}


#endif