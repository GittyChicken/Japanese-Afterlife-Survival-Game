// Copyright (c) 2026 Yomi Survival. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class YomiSurvivalEditorTarget : TargetRules
{
	public YomiSurvivalEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.AddRange(new string[] { "YomiSurvival" });
	}
}
