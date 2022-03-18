#include "InventoryAreaWidget.h"
#include "Slate/SInventoryAreaWidget.h"
#include "Slate/SInventoryItemWidget.h"


TSharedRef<SWidget> UInventoryAreaWidget::RebuildWidget()
{
    MyWidget = SNew(SInventoryAreaWidget).ItemSize(ItemSize).Layout(Layout);

#if WITH_EDITOR
    if (IsDesignTime()) {}
    else
#endif
    {
        Update();
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


void UInventoryAreaWidget::Update()
{
    auto AreaObject = UInventoryItemArea::Make(this, Area, Layout);

    if (MyWidget) {
        MyWidget->ClearChildren();
        auto Items = AreaObject->GetItems();

        for (auto& It : Items)
        {
            auto Info = It.Key->Info;
            MyWidget->AddSlot(It.Value, Info->Size)[
                SNew(SInventoryItemWidget).Item(It.Key)
            ];
        }
    }
}