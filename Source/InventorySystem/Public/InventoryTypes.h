#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealAny.h"
#include "InventoryTypes.generated.h"

UENUM(BlueprintType)
enum class EInventoryAreaChangeType : uint8
{
	Add,
	Remove,
	Move,
};

USTRUCT(Blueprintable)
struct INVENTORYSYSTEM_API FInventoryAreaChangeParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FAny> Params;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryAreaItemChanged, UInventoryItem*, Item, EInventoryAreaChangeType, Type, const FInventoryAreaChangeParam&, Params);


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

	//物品背景框颜色
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor BackgroundColor = FLinearColor::Transparent;

	//物品其他需要显示的属性
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FText> Descript;

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

	UPROPERTY(BlueprintAssignable)
	FOnInventoryAreaItemChanged OnChanged;

	//查找或创建物品区域对象
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Make Item Area"), Category = Item)
	static UInventoryItemArea* Make(UObject* WorldContextObject, FName Area, FIntPoint InLayout);

	//查找区域
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", DisplayName = "Find Item Area"), Category = Item)
	static UInventoryItemArea* Find(UObject* WorldContextObject, FName Area);

	//创建物品对象
	UFUNCTION(BlueprintCallable, Category = Item)
	UInventoryItem* MakeItem(UInventoryItemInfo* Info, FIntPoint InLocation);

	//获取一定范围内的所有物品对象
	UFUNCTION(BlueprintPure, Category = Item)
	const TArray<UInventoryItem*> GetUnderItems(FIntPoint Location, FIntPoint Size, bool& bOutBound) const;

	//查找合适的可放入物品的位置，创建物品时会经常使用
	UFUNCTION(BlueprintPure, Category = Item)
	bool FindLocation(FIntPoint Size, FIntPoint& OutLocation) const;

	//获取区域内的所有物品对象
	UFUNCTION(BlueprintPure, Category = Item)
	const TMap<UInventoryItem*, FIntPoint> GetItems() const;

	//向区域内添加物品，添加前请使用IsCanAcceptTypes判断此区域是否接受物品类型
	UFUNCTION(BlueprintCallable, Category = Item)
	void AddItem(UInventoryItem* Item, const FIntPoint& InLocation);

	//移除物品
	UFUNCTION(BlueprintCallable, Category = Item)
	bool RemoveItem(UInventoryItem* Item);

	//同区域内移动物品
	UFUNCTION(BlueprintCallable, Category = Item)
	bool MoveItem(UInventoryItem* Item, const FIntPoint& ToLocation);

	//判断此区域是否接受传入的物品类型
	UFUNCTION(BlueprintPure, Category = Item)
	bool IsCanAcceptTypes(const TSet<FName>& Other) const;
private:
	UPROPERTY(Transient)
	TArray<UInventoryItem*> Items;
};
