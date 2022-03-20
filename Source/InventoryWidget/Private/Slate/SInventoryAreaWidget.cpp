// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SInventoryAreaWidget.h"
#include "SlateOptMacros.h"
#include "Layout/LayoutUtils.h"
#include "SInventoryItemWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SInventoryAreaWidget::SInventoryAreaWidget() : Children(this)
{
}


void SInventoryAreaWidget::Construct(const FArguments& InArgs, UInventoryItemArea* InArea)
{
	Area = InArea;
	ItemSize = InArgs._ItemSize;
	Layout = InArgs._Layout;
	LineColor = InArgs._LineColor;

	DragBoxBrush.TintColor = FLinearColor::Green * FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);

	Children.AddSlots(MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots)));
}
void SInventoryAreaWidget::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	if (Children.Num() > 0)
	{
		for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
		{
			const FSlot& Child = Children[ChildIndex];
			const EVisibility ChildVisibility = Child.GetWidget()->GetVisibility();
			if (ArrangedChildren.Accepts(ChildVisibility))
			{
				const FVector2D Offset = FVector2D(ItemSize.X * Child.Location.X, ItemSize.Y * Child.Location.Y);
				const FVector2D Size = FVector2D(ItemSize.X * Child.Size.X, ItemSize.Y * Child.Size.Y);

				ArrangedChildren.AddWidget(ChildVisibility, AllottedGeometry.MakeChild(Child.GetWidget(), Offset, Size));
			}
		}
	}
}

FChildren* SInventoryAreaWidget::GetChildren()
{
	return &Children;
}

int32 SInventoryAreaWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto Size = ComputeDesiredSize(1.0f);
	
	for (int32 x = 0; x <= Layout.X; x++)
	{
		TArray<FVector2D> Points;
		Points.Emplace(ItemSize.X * x, 0.0f);
		Points.Emplace(ItemSize.X * x, Size.Y);
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, LineColor, true);
	}
	for (int32 y = 0; y <= Layout.Y; y++)
	{
		TArray<FVector2D> Points;
		Points.Emplace(0.0f, ItemSize.Y * y);
		Points.Emplace(Size.Y, ItemSize.Y * y);
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, LineColor, true);
	}

	

	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	ArrangeChildren(AllottedGeometry, ArrangedChildren);
	LayerId = PaintArrangedChildren(Args, ArrangedChildren, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (bShowDragBox) {
		auto Geometry = AllottedGeometry.ToPaintGeometry(DragBox.Min, DragBox.GetSize(), 1.0f);
		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, Geometry, &DragBoxBrush, ESlateDrawEffect::None, DragBoxBrush.TintColor.GetSpecifiedColor());
	}

	return LayerId;
}

FVector2D SInventoryAreaWidget::ComputeDesiredSize(float) const
{
	return FVector2D(ItemSize.X * Layout.X, ItemSize.Y * Layout.Y);
}

SInventoryAreaWidget::FScopedWidgetSlotArguments SInventoryAreaWidget::AddSlot(FIntPoint InLocation, FIntPoint InSize)
{
	return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(InLocation, InSize), Children, INDEX_NONE };
}


bool SInventoryAreaWidget::RemoveAt(FIntPoint InLocation)
{
	for (int32 SlotIdx = 0; SlotIdx < Children.Num(); ++SlotIdx)
	{
		if (Children[SlotIdx].Location == InLocation)
		{
			Children.RemoveAt(SlotIdx);
			return true;
		}
	}

	return false;
}

void SInventoryAreaWidget::ClearChildren()
{
	Children.Empty();
}


static bool GetCanReplaceItems(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, const FVector2D& ItemSize, 
	UInventoryItem*& SourceItem, UInventoryItemArea* CurArea, FIntPoint& OutLoc)
{
	if (auto Op = DragDropEvent.GetOperationAs<FInventoryItemDragDropOperation>()) {
		auto Local = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
		SourceItem = Op->GetItem();

		if (!CurArea->IsCanAcceptTypes(SourceItem->Info->Types))return false;

		auto StartLoction = Local - FVector2D(SourceItem->Info->Size) * ItemSize * 0.5f;
		OutLoc = FIntPoint(FMath::RoundToInt(StartLoction.X / ItemSize.X), FMath::RoundToInt(StartLoction.Y / ItemSize.Y));

		bool OutBound = false;
		auto Items = CurArea->GetUnderItems(OutLoc, SourceItem->Info->Size, OutBound);
		if (!OutBound) {
			if (Items.Num() == 0) { return true; }
			if (Items.Num() == 1 && Items[0] == SourceItem)return true;
		}
	}

	return false;
}


FReply SInventoryAreaWidget::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	FIntPoint Loc;
	UInventoryItem *SourceItem;
	bShowDragBox = GetCanReplaceItems(MyGeometry, DragDropEvent, ItemSize, SourceItem, Area, Loc);
	if (bShowDragBox) {
		auto Start = FVector2D(Loc) * ItemSize;
		auto End = FVector2D(SourceItem->Info->Size) * ItemSize + Start;
		DragBox = FBox2D(Start, End);
	}
	return FReply::Unhandled();
}

void SInventoryAreaWidget::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	bShowDragBox = false;
}

FReply SInventoryAreaWidget::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	FIntPoint Loc;
	UInventoryItem* SourceItem;
	if (GetCanReplaceItems(MyGeometry, DragDropEvent, ItemSize, SourceItem, Area, Loc)) {
		if (SourceItem->Area == Area) {
			Area->MoveItem(SourceItem, Loc);
		}
		else {
			SourceItem->Area->RemoveItem(SourceItem);
			Area->AddItem(SourceItem, Loc);
		}
	}
	bShowDragBox = false;
	return FReply::Handled();
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION