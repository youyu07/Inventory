// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR


#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "InventoryAssetEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryAssetEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
};



#endif