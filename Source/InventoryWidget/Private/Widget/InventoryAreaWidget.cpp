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