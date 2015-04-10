// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnrealEd.h"
#include "PropertyEditing.h"
#include "ProceduralFoliageComponentDetails.h"
#include "ProceduralFoliageSpawner.h"
#include "ProceduralFoliageComponent.h"
#include "InstancedFoliage.h"
#include "FoliageEdMode.h"
#include "ScopedTransaction.h"
#include "SNotificationList.h"
#include "NotificationManager.h"

#define LOCTEXT_NAMESPACE "ProceduralFoliageComponentDetails"

TSharedRef<IDetailCustomization> FProceduralFoliageComponentDetails::MakeInstance()
{
	return MakeShareable( new FProceduralFoliageComponentDetails() );

}

void FProceduralFoliageComponentDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	const FName ProceduralFoliageCategoryName("ProceduralFoliage");
	IDetailCategoryBuilder& ProceduralFoliageCategory = DetailBuilder.EditCategory(ProceduralFoliageCategoryName);

	const FText ResimulateText = LOCTEXT("ResimulateButtonText", "Resimulate" );

	TArray< TWeakObjectPtr<UObject> > ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized( ObjectsBeingCustomized );

	for( TWeakObjectPtr<UObject>& Object : ObjectsBeingCustomized )
	{
		UProceduralFoliageComponent* Component = Cast<UProceduralFoliageComponent>( Object.Get() );
		if( ensure( Component ) )
		{
			SelectedComponents.Add( Component );
		}
	}

	TArray<TSharedRef<IPropertyHandle>> AllProperties;
	bool bSimpleProperties = true;
	bool bAdvancedProperties = false;
	// Add all properties in the category in order
	ProceduralFoliageCategory.GetDefaultProperties(AllProperties, bSimpleProperties, bAdvancedProperties);
	for( auto& Property : AllProperties )
	{
		ProceduralFoliageCategory.AddProperty(Property);
	}

	FDetailWidgetRow& NewRow =  ProceduralFoliageCategory.AddCustomRow( ResimulateText );

	NewRow.ValueContent()
	.MaxDesiredWidth(120.f)
	[
		SNew(SButton)
		.OnClicked( this, &FProceduralFoliageComponentDetails::OnResimulateClicked )
		.ToolTipText( this, &FProceduralFoliageComponentDetails::GetResimulateTooltipText )
		.IsEnabled( this, &FProceduralFoliageComponentDetails::IsResimulateEnabled )
		[
			SNew( STextBlock )
			.Font( IDetailLayoutBuilder::GetDetailFont() )
			.Text( ResimulateText )
		]
	];
}

FReply FProceduralFoliageComponentDetails::OnResimulateClicked()
{
	TSet<UProceduralFoliageSpawner*> UniqueFoliageSpawners;
	for( TWeakObjectPtr<UProceduralFoliageComponent>& Component : SelectedComponents )
	{
		if( Component.IsValid() && Component->FoliageSpawner )
		{
			if( !UniqueFoliageSpawners.Contains( Component->FoliageSpawner ) )
			{
				UniqueFoliageSpawners.Add(Component->FoliageSpawner);
			}

			FScopedTransaction Transaction(LOCTEXT("Resimulate_Transaction", "Procedural Foliage Simulation"));
			TArray <FDesiredFoliageInstance> DesiredFoliageInstances;
			if (Component->GenerateProceduralContent(DesiredFoliageInstances))
			{
				FEdModeFoliage::AddInstances(Component->GetWorld(), DesiredFoliageInstances);

				// If no instances were spawned, inform the user
				if (!Component->HasSpawnedAnyInstances())
				{
					FNotificationInfo Info(LOCTEXT("NothingSpawned_Notification", "Unable to spawn instances. Ensure a large enough surface exists within the volume."));
					Info.bUseLargeFont = false;
					Info.bFireAndForget = true;
					Info.bUseThrobber = false;
					Info.bUseSuccessFailIcons = true;
					FSlateNotificationManager::Get().AddNotification(Info);
				}
			}
		}
	}
	return FReply::Handled();
}

bool FProceduralFoliageComponentDetails::IsResimulateEnabled() const
{
	for(const TWeakObjectPtr<UProceduralFoliageComponent>& Component : SelectedComponents)
	{
		if(Component.IsValid() && Component->FoliageSpawner && Component->FoliageSpawner->GetFoliageTypes().Num() > 0)
		{
			return true;
		}
	}

	return false;
}

FText FProceduralFoliageComponentDetails::GetResimulateTooltipText() const
{
	FText TooltipText;

	for (const TWeakObjectPtr<UProceduralFoliageComponent>& Component : SelectedComponents)
	{
		if (Component.IsValid())
		{
			if (!Component->FoliageSpawner)
			{
				TooltipText = LOCTEXT("Resimulate_Tooltip_NeedSpawner", "Cannot generate foliage: Assign a Procedural Foliage Spawner to run the procedural foliage simulation");
				break;
			}
			else if (Component->FoliageSpawner->GetFoliageTypes().Num() == 0)
			{
				TooltipText = LOCTEXT("Resimulate_Tooltip_EmptySpawner", "Cannot generate foliage: The assigned Procedural Foliage Spawner does not contain any foliage types to spawn.");
				break;
			}
		}
	}

	if (TooltipText.IsEmpty())
	{
		TooltipText = LOCTEXT("Resimulate_Tooltip", "Runs the procedural foliage spawner simulation. Replaces any existing instances spawned by a previous simulation.");
	}

	return TooltipText;
}

#undef LOCTEXT_NAMESPACE