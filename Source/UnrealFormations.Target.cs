// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealFormationsTarget : TargetRules
{
	public UnrealFormationsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("UnrealFormations");
		ExtraModuleNames.Add("Steering");
	}
}
