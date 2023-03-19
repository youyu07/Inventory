// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "InventoryTypes.h"

class FInventoryItemDragDropOperation : public FGameDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FInventoryItemDragDropOperation, FGameDragDropOperation);

	static TSharedRef<FInventoryItemDragDropOperation> New(TSharedPtr<class SInventoryItemWidget> InItem, const FVector2D& InItemSize, const int32 InPointerIndex);
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	virtual void OnDragged(const class FDragDropEvent& DragDropEvent) override;

	UInventoryItem* GetItem() const;
	FVector2D GetSize() const;
protected:
	FInventoryItemDragDropOperation();

private:
	TSharedPtr<class SInventoryItemWidget> Item;
	FVector2D ItemSize;
	int32 PointerIndex = -1;
};


class SInventoryItemWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryItemWidget)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UInventoryItem* InItem);


public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	UInventoryItem* Item;
	FSlateBrush Background, IconBrush;

	friend class FInventoryItemDragDropOperation;
};
