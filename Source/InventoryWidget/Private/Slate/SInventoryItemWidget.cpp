// Fill out your copyright notice in the Description page of Project Settings.


#include "SInventoryItemWidget.h"
#include "SlateOptMacros.h"
#include "Slate.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryItemWidget::Construct(const FArguments& InArgs, UInventoryItem* InItem)
{
	Item = InItem;

	if (auto Color = InItem->Info->Attributes.Find("Color")) {
		Background.TintColor = Color->Get<FLinearColor>();
	}

	if (auto Icon = InItem->Info->Attributes.Find("Icon")) {
		IconBrush = Icon->Get<FSlateBrush>();
	}

	check(Item);
	TSharedRef<SWidget> Panel = SNew(SScaleBox)
		.StretchDirection(EStretchDirection::DownOnly)
		.Stretch(EStretch::ScaleToFit)[
			SNew(SImage).Image(&IconBrush)
		];


	auto Descript = InItem->Info->Descript;
	if (auto Tip = Descript.Find("Tip")) {
		auto TipWidget = SNew(SToolTip).BorderImage(TipBackground)[
			SNew(SRichTextBlock)
				.TextStyle(InArgs._TextStyle)
				.DecoratorStyleSet(InArgs._TipStyleSet)
				.Text(*Tip)
		];
		SetToolTip(TipWidget);
	}

	if (auto Name = Descript.Find("Name")) {
		Panel = SNew(SOverlay) + SOverlay::Slot()[
			Panel
		] + SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top)[
			SNew(SRichTextBlock)
				.TextStyle(InArgs._TextStyle)
				.DecoratorStyleSet(InArgs._TipStyleSet)
				.Text(*Name)
		];
	}

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

	InItem->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.2f));

	//InItem->SetVisibility(EVisibility::Hidden);
	Operation->Construct();
	return Operation;
}


TSharedPtr<SWidget> FInventoryItemDragDropOperation::GetDefaultDecorator() const
{
	return SNew(SBox).WidthOverride(GetSize().X).HeightOverride(GetSize().Y)[
		SNew(SScaleBox)
			.StretchDirection(EStretchDirection::DownOnly)
			.Stretch(EStretch::ScaleToFit)[
				SNew(SImage).Image(&Item->IconBrush)
			]
	];
}


void FInventoryItemDragDropOperation::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetPointerIndex() == PointerIndex) {
		Item->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

		//Item->SetVisibility(EVisibility::Visible);
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