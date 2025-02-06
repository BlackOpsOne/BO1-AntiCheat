using std::string;

namespace game {
	int GetMapId();
	int GetGameTime();
	std::string GetZoneCommon();
	std::string GetZoneLanguage();
	std::string GetBlackOpsPath();

	void CheckForAllowedTools();

	// game mod
	bool IsGameModLoaded();
	bool IsGameModPresent();

	// custom fx tool
	bool IsCustomFxToolLoaded();
	bool IsCustomFxToolPresent();

	std::string GetLanguageZoneName();
	std::string GetGameLanguage();
}