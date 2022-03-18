// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SInventoryAreaWidget.h"
#include "SlateOptMacros.h"
#include "Layout/LayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SInventoryAreaWidget::SInventoryAreaWidget() : Children(this)
{
}


void SInventoryAreaWidget::Construct(const FArguments& InArgs)
{
	ItemSize = InArgs._ItemSize;
	Layout = InArgs._Layout;
	LineColor = InArgs._LineColor;

	Children.Reserve(InArgs.Slots.Num());
	for (int32 ChildIndex = 0; ChildIndex < InArgs.Slots.Num(); ChildIndex++)
	{
		FSlot* ChildSlot = InArgs.Slots[ChildIndex];
		Children.Add(ChildSlot);
	}
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
	return PaintArrangedChildren(Args, ArrangedChildren, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FVector2D SInventoryAreaWidget::ComputeDesiredSize(float) const
{
	return FVector2D(ItemSize.X * Layout.X, ItemSize.Y * Layout.Y);
}

SInventoryAreaWidget::FSlot& SInventoryAreaWidget::AddSlot(FIntPoint InLocation, FIntPoint InSize)
{
	FSlot& NewSlot = *(new FSlot(InLocation, InSize));
	Children.Add(&NewSlot);
	return NewSlot;
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


END_SLATE_FUNCTION_BUILD_OPTIMIZATION