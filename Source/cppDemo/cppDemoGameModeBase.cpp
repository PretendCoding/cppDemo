// Copyright Epic Games, Inc. All Rights Reserved.


#include "cppDemoGameModeBase.h"
#include "cppDemoGameStateBase.h"
#include "MyCharacter.h"

AcppDemoGameModeBase::AcppDemoGameModeBase() {
	GameStateClass = AcppDemoGameStateBase::StaticClass();
	DefaultPawnClass = AMyCharacter::StaticClass();
}