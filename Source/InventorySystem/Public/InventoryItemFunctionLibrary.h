// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryItemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryItemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//对区域内的所有物品进行合成操作，区域内的物品必须正好满足配方要求
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Combine Item"))
	static bool Combine(class UDataTable* InRecipes, class UInventoryItemArea* InArea);


	//对区域内的物品进行拆分操作，区域内必须有且仅有一个物品
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Split Item"))
	static bool Split(class UDataTable* InRecipes, class UInventoryItemArea* InArea);
};
