// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SPanel.h"

/**
 * 
 */
class INVENTORYWIDGET_API SInventoryAreaWidget : public SPanel
{
public:
	struct FSlot : public TSlotBase<FSlot>
	{
		FSlot(FIntPoint InLocation, FIntPoint InSize)
			: TSlotBase<FSlot>()
			, Location(InLocation)
			, Size(InSize)
		{
		}

		FIntPoint Location;
		FIntPoint Size;
	};

	SInventoryAreaWidget();

	static FSlot& Slot(FIntPoint InLocation, FIntPoint InSize)
	{
		return *(new FSlot(InLocation, InSize));
	}

	SLATE_BEGIN_ARGS(SInventoryAreaWidget)
		: _ItemSize(32.0f)
		, _Layout(2)
		, _LineColor(FLinearColor::White)
	{}
	SLATE_SLOT_ARGUMENT(FSlot, Slots)
	SLATE_ARGUMENT(FVector2D, ItemSize)
	SLATE_ARGUMENT(FIntPoint, Layout)
	SLATE_ARGUMENT(FLinearColor, LineColor)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, class UInventoryItemArea* InArea);

	void SetItemSize(const FVector2D& InItemSize)
	{
		ItemSize = InItemSize;
	}

	void SetLayout(const FIntPoint& InLayout)
	{
		Layout = InLayout;
	}

	void SetLineColor(const FLinearColor& InColor)
	{
		LineColor = InColor;
	}

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

	FScopedWidgetSlotArguments AddSlot(FIntPoint InLocation, FIntPoint InSize);
	bool RemoveAt(FIntPoint InLocation);
	void ClearChildren();

	//~ Begin SPanel Interface	
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FChildren* GetChildren() override;
	//~ End SPanel Interface

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;


	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
protected:

	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.
private:

	class UInventoryItemArea* Area = nullptr;
	FIntPoint Layout;
	FVector2D ItemSize;
	TAttribute<FMargin> SlotPadding;
	TPanelChildren<FSlot> Children;

	FLinearColor LineColor;

	bool bShowDragBox = false;
	FBox2D DragBox;
	FSlateBrush DragBoxBrush;
};
