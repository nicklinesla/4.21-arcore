// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UMGPrivatePCH.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UWindowTitleBarArea

UWindowTitleBarArea::UWindowTitleBarArea(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = false;

	SWindowTitleBarArea::FArguments Defaults;
	Visiblity_DEPRECATED = Visibility = UWidget::ConvertRuntimeToSerializedVisibility(Defaults._Visibility.Get());

	bDoubleClickTogglesFullscreen = false;
}

void UWindowTitleBarArea::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyWindowTitleBarArea.Reset();

	if (WindowActionNotificationHandle.IsValid())
	{
		FSlateApplication::Get().UnregisterOnWindowActionNotification(WindowActionNotificationHandle);
		WindowActionNotificationHandle.Reset();
	}
}

TSharedRef<SWidget> UWindowTitleBarArea::RebuildWidget()
{
	MyWindowTitleBarArea = SNew(SWindowTitleBarArea);
	if (bDoubleClickTogglesFullscreen)
	{
		WindowActionNotificationHandle = FSlateApplication::Get().RegisterOnWindowActionNotification(BIND_UOBJECT_DELEGATE(FOnWindowAction, HandleWindowAction));
	}
	else if (WindowActionNotificationHandle.IsValid())
	{
		FSlateApplication::Get().UnregisterOnWindowActionNotification(WindowActionNotificationHandle);
		WindowActionNotificationHandle.Reset();
	}

	MyWindowTitleBarArea->SetOnDoubleClickCallback(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleMouseButtonDoubleClick));

	if (GetChildrenCount() > 0)
	{
		Cast<UWindowTitleBarAreaSlot>(GetContentSlot())->BuildSlot(MyWindowTitleBarArea.ToSharedRef());
	}

	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
	{
		MyWindowTitleBarArea->SetGameWindow(GEngine->GameViewport->GetWindow());
	}

	return BuildDesignTimeWidget(MyWindowTitleBarArea.ToSharedRef());
}

UClass* UWindowTitleBarArea::GetSlotClass() const
{
	return UWindowTitleBarAreaSlot::StaticClass();
}

void UWindowTitleBarArea::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live slot if it already exists
	if (MyWindowTitleBarArea.IsValid())
	{
		// Construct the underlying slot.
		UWindowTitleBarAreaSlot* WindowTitleBarAreaSlot = CastChecked<UWindowTitleBarAreaSlot>(InSlot);
		WindowTitleBarAreaSlot->BuildSlot(MyWindowTitleBarArea.ToSharedRef());
	}
}

void UWindowTitleBarArea::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyWindowTitleBarArea.IsValid())
	{
		MyWindowTitleBarArea->SetContent(SNullWidget::NullWidget);
	}
}

void UWindowTitleBarArea::SetPadding(FMargin InPadding)
{
	if (MyWindowTitleBarArea.IsValid())
	{
		MyWindowTitleBarArea->SetPadding(InPadding);
	}
}

void UWindowTitleBarArea::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	if (MyWindowTitleBarArea.IsValid())
	{
		MyWindowTitleBarArea->SetHAlign(InHorizontalAlignment);
	}
}

void UWindowTitleBarArea::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	if (MyWindowTitleBarArea.IsValid())
	{
		MyWindowTitleBarArea->SetVAlign(InVerticalAlignment);
	}
}

void UWindowTitleBarArea::PostLoad()
{
	Super::PostLoad();

	if (GetChildrenCount() > 0)
	{
		//TODO UMG Pre-Release Upgrade, now have slots of their own.  Convert existing slot to new slot.
		if (UPanelSlot* PanelSlot = GetContentSlot())
		{
			UWindowTitleBarAreaSlot* WindowTitleBarAreaSlot = Cast<UWindowTitleBarAreaSlot>(PanelSlot);
			if (WindowTitleBarAreaSlot == NULL)
			{
				WindowTitleBarAreaSlot = NewObject<UWindowTitleBarAreaSlot>(this);
				WindowTitleBarAreaSlot->Content = GetContentSlot()->Content;
				WindowTitleBarAreaSlot->Content->Slot = WindowTitleBarAreaSlot;
				Slots[0] = WindowTitleBarAreaSlot;
			}
		}
	}
}

bool UWindowTitleBarArea::HandleWindowAction(const TSharedRef<FGenericWindow>& PlatformWindow, EWindowAction::Type WindowAction)
{
	if (GEngine && (WindowAction == EWindowAction::Maximize || WindowAction == EWindowAction::Restore))
	{
		GEngine->DeferredCommands.Add(TEXT("TOGGLE_FULLSCREEN"));
		return true;
	}
	return false;
}

void UWindowTitleBarArea::HandleMouseButtonDoubleClick()
{
	// This is only called in fullscreen mode when user double clicks the title bar.
	if (GEngine)
	{
		GEngine->DeferredCommands.Add(TEXT("TOGGLE_FULLSCREEN"));
	}
}

#if WITH_EDITOR

const FText UWindowTitleBarArea::GetPaletteCategory()
{
	return LOCTEXT("Borderless Window", "Borderless Window");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
