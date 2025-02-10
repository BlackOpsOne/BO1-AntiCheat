#include <windows.h>

#include <string>

#include <vector>

#include "process.hpp"

#include "../checksums.h"

#include "../utils/memory.hpp"

#include "../constants.h"

#include "../utils/files.hpp"

#include "game.hpp"

#include "dvars.hpp"

bool game_mod_loaded = false;
bool custom_fx_loaded = false;

namespace fs = std::filesystem;

namespace anticheat {
	namespace game {

		// reads the map id from memory, Furret's box tracker uses this same method for setting up the weapons list
		int GetMapId()
		{
			HANDLE handle = process::GetBlackOpsHandle();
			if (handle == NULL || handle == INVALID_HANDLE_VALUE) {
				return -1;
			}

			int mapId = utils::memory::ReadInt(handle, Constants::C_MAPADDRESS);
			return mapId;
		}

		const char* GetMapName()
		{
			const char* map_name = anticheat::game::dvars::GetDvarString("mapname");

			// this only happens when the game is closing
			if (map_name == nullptr)
			{
				return "";
			}

			return map_name;
		}

		// gets the total game time
		int GetGameTime()
		{
			HANDLE handle = process::GetBlackOpsHandle();
			if (handle == NULL || handle == INVALID_HANDLE_VALUE) {
				return -1;
			}

			int gameTime = utils::memory::ReadInt(handle, Constants::C_TIMEADDRESS);
			return gameTime;
		}

		bool IsInMap()
		{
			int map_id = GetMapId();
			return map_id != Constants::INVALID_MAP && map_id != Constants::NO_MAP;
		}

		// gets the path to zone\\Common
		std::string GetZoneCommon()
		{
			return GetGamePath() + "\\zone\\Common\\";
		}

		// gets the path to the current langauge folder
		std::string GetZoneLanguage()
		{
			return GetGamePath() + "\\zone\\" + GetLanguageZoneName() + "\\";
		}

		// gets the path to the executable thats being run
		std::string GetGamePath()
		{
			std::string blackOpsPath = process::GetPathToExe();
			std::string::size_type pos = blackOpsPath.find_last_of("\\/");
			return blackOpsPath.substr(0, pos);
		}

		// acts as an initializer for the game_mod_loaded boolean
		void CheckForAllowedTools()
		{
			game_mod_loaded = IsGameModPresent();
			custom_fx_loaded = IsCustomFxToolPresent();
		}

		// just a getter for the value of game_mod_loaded
		bool IsGameModLoaded()
		{
			return game_mod_loaded;
		}

		// checks for the game_mod.dll file in the modules list of bo1
		bool IsGameModPresent()
		{
			HANDLE handle = process::GetBlackOpsHandle();
			if (handle == NULL || handle == INVALID_HANDLE_VALUE) {
				return false;
			}

			std::vector<HMODULE> hMods(1024);
			DWORD cbNeeded;

			if (EnumProcessModulesEx(handle, hMods.data(), hMods.size() * sizeof(HMODULE), &cbNeeded, LIST_MODULES_ALL)) {
				size_t moduleCount = cbNeeded / sizeof(HMODULE);
				for (size_t i = 0; i < moduleCount; i++) {
					TCHAR szModName[MAX_PATH];
					MODULEINFO modInfo;

					if (GetModuleFileNameEx(handle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
						if (GetModuleInformation(handle, hMods[i], &modInfo, sizeof(modInfo))) {
							char modulePath[MAX_PATH];
							size_t convertedChars = 0;
							wcstombs_s(&convertedChars, modulePath, szModName, sizeof(modulePath));
							modulePath[sizeof(modulePath) - 1] = '\0';
							std::string dllPath = std::string(modulePath);

							// if we have a match then game mod is present
							if (utils::files::GetMD5(dllPath) == Checksums::GAME_MOD_DLL)
							{
								return true;
							}
						}
					}
				}
			}

			return false;
		}

		// this checks if the custom fx tool is present in the game
		bool IsCustomFxToolPresent()
		{
			HANDLE handle = process::GetBlackOpsHandle();
			if (!process::IsGameOpen()) {
				return false;
			}

			std::string game_folder = game::GetGamePath();
			std::string steam_api = game_folder + "\\steam_api.dll";
			std::string fx_tool = game_folder + "\\blackops-fx.dll";

			// check for a match on both steam_api.dll and blackops-fx.dll
			// every install of black ops requires a steam_api.dll file
			// so we can just check those hashes and see if they match up
			if (fs::exists(steam_api) && fs::exists(fx_tool))
			{
				std::string steam_api_hash = utils::files::GetMD5(steam_api);
				std::string fx_dll_hash = utils::files::GetMD5(fx_tool);

				if (steam_api_hash == Checksums::CUSTOM_FX_STEAM_API_DLL 
					&& fx_dll_hash == Checksums::CUSTOM_FX_DLL)
				{
					return true;
				}
			}

			return false;
		}

		bool IsCustomFxToolLoaded()
		{
			return custom_fx_loaded;
		}

		std::string GetLanguageZoneName()
		{
			std::string language = GetGameLanguage();

			if (language == "english")
			{
				return "English";
			}
			else if (language == "japanese")
			{
				return "Japanese";
			}
			else if (language == "french")
			{
				return "French";
			}
			else if (language == "austrian")
			{
				return "German";
			}
			else if (language == "italian")
			{
				return "Italian";
			}
			else if (language == "spanish")
			{
				return "Spanish";
			}

			return "English";
		}

		std::string GetGameLanguage()
		{
			HANDLE handle = process::GetBlackOpsHandle();
			if (handle == NULL || handle == INVALID_HANDLE_VALUE) {
				return "";
			}

			std::string lang = utils::memory::ReadString(handle, Constants::C_LANGADDRESS);
			return lang;
		}
	} // game
} // anticheat