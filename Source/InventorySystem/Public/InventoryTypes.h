#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealAny.h"
#include "InventoryTypes.generated.h"


UCLASS()
class INVENTORYSYSTEM_API UInventoryItemInfo : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()
public:
	//物品所占格子大小, 最小为1，1
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntPoint Size = FIntPoint(1);

	//物品类型,可以设置部分区域仅接收部分物品类型
	//注意：物品必须有至少一个类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	TSet<FName> Types;

	//物品图标
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush Icon;

	//物品其他属性，可根据需要添加
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FString> Custom;

public:
	UFUNCTION(BlueprintCallable, Category = Item)
	static UInventoryItemInfo* Find(FName InId);


	static TMap<FName, UInventoryItemInfo*> Map;
};


UCLASS(Transient, BlueprintType)
class INVENTORYSYSTEM_API UInventoryItem : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient, BlueprintReadOnly)
	UInventoryItemInfo* Info;

	UPROPERTY(Transient, BlueprintReadOnly)
	UInventoryItemArea* Area = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FAny> Custom;

public:
	//移动物品位置，目标区域为空则只在当前区域移动，如果位置不可移动则返回false
	//规则：当位置内仅有一个物体时可以移动
	UFUNCTION(BlueprintCallable, Category = Item)
	bool Move(FIntPoint ToLocation, UInventoryItem*& BelowItem, UInventoryItemArea* ToArea = nullptr, bool bTest = false);

	UFUNCTION(BlueprintCallable, Category = Item)
	void Delete();
};


UCLASS(Transient, BlueprintType)
class INVENTORYSYSTEM_API UInventoryItemArea : public UObject
{
	GENERATED_BODY()

public:
	//区域大小
	UPROPERTY(BlueprintReadOnly)
	FIntPoint Layout;

	//可选，可接收的物品类型，为空表示接收所有类型
	UPROPERTY(BlueprintReadWrite)
	TSet<FName> AcceptTypes;

	//查找或创建物品区域对象
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Make Item Area"), Category = Item)
	static UInventoryItemArea* Make(UObject* WorldContextObject, FName Area, FIntPoint InLayout);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", DisplayName = "Find Item Area"), Category = Item)
	static UInventoryItemArea* Find(UObject* WorldContextObject, FName Area);

	UFUNCTION(BlueprintCallable, Category = Item)
	UInventoryItem* MakeItem(UInventoryItemInfo* Info, FIntPoint InLocation);

	UFUNCTION(BlueprintPure, Category = Item)
	const TArray<UInventoryItem*> GetUnderItems(FIntPoint Location, FIntPoint Size, bool& bOutBound) const;


	UFUNCTION(BlueprintPure, Category = Item)
	bool FindLocation(FIntPoint Size, FIntPoint& OutLocation) const;

	UFUNCTION(BlueprintPure, Category = Item)
	const TMap<UInventoryItem*, FIntPoint> GetItems() const;

private:
	UPROPERTY(Transient)
	TArray<UInventoryItem*> Items;

	bool IsCanAcceptTypes(const TSet<FName>& Other) const;
	void AddItem(UInventoryItem* Item, const FIntPoint& InLocation);
	void RemoveItem(UInventoryItem* Item);


	friend class UInventoryItem;
};
