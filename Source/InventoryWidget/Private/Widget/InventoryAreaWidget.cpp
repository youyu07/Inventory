#include "InventoryAreaWidget.h"
#include "Slate/SInventoryItemWidget.h"
#include "Slate/SInventoryAreaWidget.h"
#include "Components/RichTextBlock.h"


UInventoryAreaWidget::UInventoryAreaWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DefaultTipTextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
}


TSharedRef<SWidget> UInventoryAreaWidget::RebuildWidget()
{
    AreaObject = UInventoryItemArea::Make(this, Area, Layout);
    MyWidget = SNew(SInventoryAreaWidget, AreaObject).ItemSize(ItemSize).Layout(Layout);

    TipStyleSet = MakeShareable(new FSlateStyleSet("InventoryItemTipStyleSet"));
	if (TipTextStyleSet && TipTextStyleSet->GetRowStruct()->IsChildOf(FRichTextStyleRow::StaticStruct()))
	{
		for (const auto& Entry : TipTextStyleSet->GetRowMap())
		{
			FName SubStyleName = Entry.Key;
			FRichTextStyleRow* RichTextStyle = (FRichTextStyleRow*)Entry.Value;

			if (SubStyleName == FName(TEXT("Default")))
			{
				DefaultTipTextStyle = RichTextStyle->TextStyle;
			}
			TipStyleSet->Set(SubStyleName, RichTextStyle->TextStyle);
		}
	}

#if WITH_EDITOR
    if (IsDesignTime()) {}
    else
#endif
    {
        Update();
        if (AreaObject) {
            AreaObject->OnChanged.AddDynamic(this, &UInventoryAreaWidget::OnAreaChanged);
        }
    }

    return MyWidget.ToSharedRef();
}

void UInventoryAreaWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	MyWidget->SetItemSize(ItemSize);
	MyWidget->SetLayout(Layout);
	MyWidget->SetLineColor(LineColor);
}

void UInventoryAreaWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MyWidget.Reset();
}


void UInventoryAreaWidget::OnAreaChanged(const FInventoryAreaChangedEvent& Event)
{
    Update();
}

void UInventoryAreaWidget::Update()
{
    MyWidget->ClearChildren();

    if (AreaObject) {
        auto Items = AreaObject->GetItems();
        for (auto& It : Items)
        {
            auto Info = It.Value->Info;
            MyWidget->AddSlot(It.Key, Info->Size)[
                SNew(SInventoryItemWidget, It.Value)
            ];
        }
    }
}