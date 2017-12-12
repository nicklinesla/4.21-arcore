// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

class UCryptoKeysSettings;

namespace CryptoKeysHelpers
{
	bool GenerateNewEncryptionKey(UCryptoKeysSettings* InSettings);
	bool GenerateNewSigningKeys(UCryptoKeysSettings* InSettings);
}