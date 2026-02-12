using UnrealBuildTool;

public class RunwayGameTarget : TargetRules {
	public RunwayGameTarget(TargetInfo Target) : base(Target) {
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[] {
			"Runway",
		});
	}
}
