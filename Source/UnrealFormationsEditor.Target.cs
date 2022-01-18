// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealFormationsEditorTarget : TargetRules
{
	public UnrealFormationsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("UnrealFormations");
		ExtraModuleNames.Add("Steering");
	}
}
