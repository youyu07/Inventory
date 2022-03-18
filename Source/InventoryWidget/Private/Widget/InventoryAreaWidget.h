// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "InventoryAreaWidget.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryAreaWidget : public UWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	FName Area;

	UPROPERTY(EditAnywhere)
	FIntPoint Layout = FIntPoint(9);

	UPROPERTY(EditAnywhere)
	FVector2D ItemSize = 32.0f;

	UPROPERTY(EditAnywhere)
	FLinearColor LineColor = FLinearColor::White;

	UFUNCTION(BlueprintCallable)
	void Update();
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

private:
	TSharedPtr<class SInventoryAreaWidget> MyWidget;
};
