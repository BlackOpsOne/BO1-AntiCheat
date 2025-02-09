using std::string;

namespace game {
	int GetMapId();
	const char* GetMapName();
	int GetGameTime();
	bool IsInMap();
	std::string GetZoneCommon();
	std::string GetZoneLanguage();
	std::string GetGamePath();

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