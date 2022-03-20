// Fill out your copyright notice in the Description page of Project Settings.


#include "SInventoryItemWidget.h"
#include "SlateOptMacros.h"
#include "Slate.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryItemWidget::Construct(const FArguments& InArgs, UInventoryItem* InItem)
{
	Item = InItem;
	Background.TintColor = InItem->Info->BackgroundColor;
	check(Item);
	auto Panel = SNew(SScaleBox)
		.StretchDirection(EStretchDirection::DownOnly)
		.Stretch(EStretch::ScaleToFit)[
			SNew(SImage).Image(&Item->Info->Icon)
		];

	ChildSlot.Padding(1.0f)
	[
		SNew(SBorder).Padding(0.0f)
		.BorderImage(&Background)[
			Panel
		]
	];
}

FReply SInventoryItemWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto Reply = SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
		Reply = FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}
	return Reply;
}
FReply SInventoryItemWidget::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		auto Operation = FInventoryItemDragDropOperation::New(SharedThis(this), MyGeometry.GetAbsoluteSize(), MouseEvent.GetPointerIndex());
		return FReply::Handled().BeginDragDrop(Operation);
	}

	return FReply::Unhandled();
}

FInventoryItemDragDropOperation::FInventoryItemDragDropOperation()
{
	
}

TSharedRef<FInventoryItemDragDropOperation> FInventoryItemDragDropOperation::New(TSharedPtr<SInventoryItemWidget> InItem, const FVector2D& InItemSize, const int32 InPointerIndex)
{
	// Create the drag-drop op containing the key
	TSharedRef<FInventoryItemDragDropOperation> Operation = MakeShareable(new FInventoryItemDragDropOperation);
	Operation->Item = InItem;
	Operation->ItemSize = InItemSize;
	Operation->PointerIndex = InPointerIndex;

	InItem->SetVisibility(EVisibility::Hidden);
	Operation->Construct();
	return Operation;
}


TSharedPtr<SWidget> FInventoryItemDragDropOperation::GetDefaultDecorator() const
{
	return SNew(SBox).WidthOverride(GetSize().X).HeightOverride(GetSize().Y)[
		SNew(SScaleBox)
			.StretchDirection(EStretchDirection::DownOnly)
			.Stretch(EStretch::ScaleToFit)[
				SNew(SImage).Image(&GetItem()->Info->Icon)
			]
	];
}


void FInventoryItemDragDropOperation::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetPointerIndex() == PointerIndex) {
		Item->SetVisibility(EVisibility::Visible);
	}
}


void FInventoryItemDragDropOperation::OnDragged(const FDragDropEvent& DragDropEvent)
{
	if (DragDropEvent.GetPointerIndex() == PointerIndex)
	{
		FVector2D Position = DragDropEvent.GetScreenSpacePosition();
		Position -= GetSize() * FVector2D(0.5f, 0.5f);

		DecoratorPosition = Position;
	}
}

UInventoryItem* FInventoryItemDragDropOperation::GetItem() const { return Item->Item; }
FVector2D FInventoryItemDragDropOperation::GetSize() const { return ItemSize; }

END_SLATE_FUNCTION_BUILD_OPTIMIZATION