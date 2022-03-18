// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "InventoryTypes.h"

/**
 * 
 */
class SInventoryItemWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryItemWidget)
	{}
	SLATE_ARGUMENT(UInventoryItem*, Item)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FSlateBrush Background;
};
