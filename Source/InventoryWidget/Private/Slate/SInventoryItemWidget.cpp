// Fill out your copyright notice in the Description page of Project Settings.


#include "SInventoryItemWidget.h"
#include "SlateOptMacros.h"
#include "Slate.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SInventoryItemWidget::Construct(const FArguments& InArgs)
{
	Background.TintColor = FLinearColor::MakeRandomColor() * FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);

	auto Item = InArgs._Item;
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
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
