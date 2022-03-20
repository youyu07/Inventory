// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "InventoryTypes.h"
#include "InventoryAreaWidget.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryAreaWidget : public UWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FName Area;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FIntPoint Layout = FIntPoint(9);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FVector2D ItemSize = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FLinearColor LineColor = FLinearColor::White;
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

private:
	UFUNCTION()
	void OnAreaChanged(UInventoryItem* Item, EInventoryAreaChangeType Type, const FInventoryAreaChangeParam& Params);

	void Update();
private:
	TSharedPtr<class SInventoryAreaWidget> MyWidget;
	UInventoryItemArea* AreaObject = nullptr;
};
