// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MovieSceneToolsPrivatePCH.h"
#include "ColorPropertyTrackEditor.h"
#include "ColorPropertySection.h"

TSharedRef<FMovieSceneTrackEditor> FColorPropertyTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> InSequencer )
{
	return MakeShareable( new FColorPropertyTrackEditor( InSequencer ) );
}

TSharedRef<ISequencerSection> FColorPropertyTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack* Track )
{
	return MakeShareable(new FColorPropertySection( SectionObject, Track->GetTrackName(), GetSequencer().Get(), Track ));
}

bool FColorPropertyTrackEditor::TryGenerateKeyFromPropertyChanged( const FPropertyChangedParams& PropertyChangedParams, FColorKey& OutKey )
{
	bool bIsFColor = false, bIsFLinearColor = false, bIsSlateColor = false;
	const UStructProperty* StructProp = Cast<const UStructProperty>(PropertyChangedParams.PropertyPath.Last());
	if (StructProp && StructProp->Struct)
	{
		FName StructName = StructProp->Struct->GetFName();

		bIsFColor = StructName == NAME_Color;
		bIsFLinearColor = StructName == NAME_LinearColor;
		bIsSlateColor = StructName == FName("SlateColor");
	}

	if (!bIsFColor && !bIsFLinearColor && !bIsSlateColor) 
	{
		return false;
	}

	FName PropertyName = PropertyChangedParams.PropertyPath.Last()->GetFName();

	FLinearColor ColorValue;
	if (bIsFColor)
	{
		ColorValue = PropertyChangedParams.GetPropertyValue<FColor>()->ReinterpretAsLinear();
	}
	else
	{
		ColorValue = *PropertyChangedParams.GetPropertyValue<FLinearColor>();
	}

	if( StructProp->HasMetaData("HideAlphaChannel") )
	{
		ColorValue.A = 1;
	}

	OutKey.Value = ColorValue;
	OutKey.CurveName = PropertyChangedParams.StructPropertyNameToKey;
	OutKey.bIsSlateColor = bIsSlateColor;

	return true;
}
