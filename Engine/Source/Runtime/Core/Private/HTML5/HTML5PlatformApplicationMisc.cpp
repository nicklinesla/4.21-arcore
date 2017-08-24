// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HTML5PlatformApplicationMisc.h"
#include "HTML5Application.h"

THIRD_PARTY_INCLUDES_START
#include <SDL.h>
THIRD_PARTY_INCLUDES_END

GenericApplication* FHTML5PlatformApplicationMisc::CreateApplication()
{
	return FHTML5Application::CreateHTML5Application();
}

uint32 FHTML5PlatformApplicationMisc::GetCharKeyMap(uint32* KeyCodes, FString* KeyNames, uint32 MaxMappings)
{
	return FGenericPlatformApplicationMisc::GetStandardPrintableKeyMap(KeyCodes, KeyNames, MaxMappings, false, true);
}

uint32 FHTML5PlatformApplicationMisc::GetKeyMap(uint32* KeyCodes, FString* KeyNames, uint32 MaxMappings)
{
#define ADDKEYMAP(KeyCode, KeyName)		if (NumMappings<MaxMappings) { KeyCodes[NumMappings]=KeyCode; KeyNames[NumMappings]=KeyName; ++NumMappings; };

	uint32 NumMappings = 0;

	if (KeyCodes && KeyNames && (MaxMappings > 0))
	{
		ADDKEYMAP(SDL_SCANCODE_BACKSPACE, TEXT("BackSpace"));
		ADDKEYMAP(SDL_SCANCODE_TAB, TEXT("Tab"));
		ADDKEYMAP(SDL_SCANCODE_RETURN, TEXT("Enter"));
		ADDKEYMAP(SDL_SCANCODE_RETURN2, TEXT("Enter"));
		ADDKEYMAP(SDL_SCANCODE_KP_ENTER, TEXT("Enter"));
		ADDKEYMAP(SDL_SCANCODE_PAUSE, TEXT("Pause"));

		ADDKEYMAP(SDL_SCANCODE_ESCAPE, TEXT("Escape"));
		ADDKEYMAP(SDL_SCANCODE_SPACE, TEXT("SpaceBar"));
		ADDKEYMAP(SDL_SCANCODE_PAGEUP, TEXT("PageUp"));
		ADDKEYMAP(SDL_SCANCODE_PAGEDOWN, TEXT("PageDown"));
		ADDKEYMAP(SDL_SCANCODE_END, TEXT("End"));
		ADDKEYMAP(SDL_SCANCODE_HOME, TEXT("Home"));

		ADDKEYMAP(SDL_SCANCODE_LEFT, TEXT("Left"));
		ADDKEYMAP(SDL_SCANCODE_UP, TEXT("Up"));
		ADDKEYMAP(SDL_SCANCODE_RIGHT, TEXT("Right"));
		ADDKEYMAP(SDL_SCANCODE_DOWN, TEXT("Down"));

		ADDKEYMAP(SDL_SCANCODE_INSERT, TEXT("Insert"));
		ADDKEYMAP(SDL_SCANCODE_DELETE, TEXT("Delete"));

		ADDKEYMAP(SDL_SCANCODE_F1, TEXT("F1"));
		ADDKEYMAP(SDL_SCANCODE_F2, TEXT("F2"));
		ADDKEYMAP(SDL_SCANCODE_F3, TEXT("F3"));
		ADDKEYMAP(SDL_SCANCODE_F4, TEXT("F4"));
		ADDKEYMAP(SDL_SCANCODE_F5, TEXT("F5"));
		ADDKEYMAP(SDL_SCANCODE_F6, TEXT("F6"));
		ADDKEYMAP(SDL_SCANCODE_F7, TEXT("F7"));
		ADDKEYMAP(SDL_SCANCODE_F8, TEXT("F8"));
		ADDKEYMAP(SDL_SCANCODE_F9, TEXT("F9"));
		ADDKEYMAP(SDL_SCANCODE_F10, TEXT("F10"));
		ADDKEYMAP(SDL_SCANCODE_F11, TEXT("F11"));
		ADDKEYMAP(SDL_SCANCODE_F12, TEXT("F12"));

		ADDKEYMAP(SDL_SCANCODE_CAPSLOCK, TEXT("CapsLock"));
		ADDKEYMAP(SDL_SCANCODE_LCTRL, TEXT("LeftControl"));
		ADDKEYMAP(SDL_SCANCODE_LSHIFT, TEXT("LeftShift"));
		ADDKEYMAP(SDL_SCANCODE_LALT, TEXT("LeftAlt"));
		ADDKEYMAP(SDL_SCANCODE_RCTRL, TEXT("RightControl"));
		ADDKEYMAP(SDL_SCANCODE_RSHIFT, TEXT("RightShift"));
		ADDKEYMAP(SDL_SCANCODE_RALT, TEXT("RightAlt"));
	}

	check(NumMappings < MaxMappings);
	return NumMappings;

	return NumMappings;
}
