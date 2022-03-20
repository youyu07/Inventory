#include "InventoryAreaWidget.h"
#include "Slate/SInventoryItemWidget.h"
#include "Slate/SInventoryAreaWidget.h"

TSharedRef<SWidget> UInventoryAreaWidget::RebuildWidget()
{
    AreaObject = UInventoryItemArea::Make(this, Area, Layout);
    MyWidget = SNew(SInventoryAreaWidget, AreaObject).ItemSize(ItemSize).Layout(Layout);

#if WITH_EDITOR
    if (IsDesignTime()) {}
    else
#endif
    {
        Update();
        AreaObject->OnChanged.AddDynamic(this, &UInventoryAreaWidget::OnAreaChanged);
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


void UInventoryAreaWidget::OnAreaChanged(UInventoryItem* Item, EInventoryAreaChangeType Type, const FInventoryAreaChangeParam& Params)
{
    Update();
}

void UInventoryAreaWidget::Update()
{
    MyWidget->ClearChildren();

    auto Items = AreaObject->GetItems();
    for (auto& It : Items)
    {
        auto Info = It.Key->Info;
        MyWidget->AddSlot(It.Value, Info->Size)[
            SNew(SInventoryItemWidget, It.Key)
        ];
    }
}