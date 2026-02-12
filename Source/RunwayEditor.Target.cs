using UnrealBuildTool;

public class RunwayEditorTarget : TargetRules {
	public RunwayEditorTarget(TargetInfo Target) : base(Target) {
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[] {
			"Runway",
		});
	}
}
