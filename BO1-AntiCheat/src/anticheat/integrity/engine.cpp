#include "engine.hpp"

#include "../../game/process.hpp"

#include "../../game/game.hpp"

#include "../../utils/files.hpp"

#include "../../utils/memory.hpp"

#include "../../utils/strings.hpp"

#include "dvars.hpp"

namespace anticheat {
	namespace integrity {
		namespace engine {
			// debugging function
			/*void PrintOpcodes(const std::vector<BYTE>& opcodes) {
				std::cout << "Opcodes: ";
				for (size_t i = 0; i < opcodes.size(); ++i) {
					std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)opcodes[i] << " ";
				}
				std::cout << std::dec << std::endl;
			}*/

			static bool CheckEngineFunction(std::vector<BYTE> expected, std::vector<BYTE> gotten)
			{
				if (!gotten.empty() && game::process::IsGameOpen() && !utils::memory::DoOpcodesMatch(gotten, expected))
				{
					return false;
				}
				return true;
			}

			std::string ModifiedEngineFunctions()
			{
				vector<string> modified_functions;

				// check "Com_LoadLevelFastFiles"
				if (!game::IsCustomFxToolLoaded())
				{
					bool func = CheckEngineFunction(
						{ 0x83, 0xec, 0x7c, 0x53, 0x55 },
						utils::memory::GetFunctionOpcodes((void*)0x004C8890)
					);
					if (!func)
					{
						modified_functions.push_back("Com_LoadLevelFastFiles");
					}
				}

				// check "Dvar_GetString"
				bool check_Dvar_GetString = CheckEngineFunction(
					{ 0x8b, 0x44, 0x24, 0x04, 0x85 },
					utils::memory::GetFunctionOpcodes((void*)0x57FF80)
				);
				if (!check_Dvar_GetString)
				{
					modified_functions.push_back("Dvar_GetString");
				}

				// check "Dvar_GetBool"
				bool check_Dvar_GetBool = CheckEngineFunction(
					{ 0x8b, 0x44, 0x24, 0x04, 0x85 },
					utils::memory::GetFunctionOpcodes((void*)0x68B030)
				);
				if (!check_Dvar_GetBool)
				{
					modified_functions.push_back("Dvar_GetBool");
				}

				// check "Dvar_GetInt"
				bool check_Dvar_GetInt = CheckEngineFunction(
					{ 0x8b, 0x44, 0x24, 0x04, 0x85 },
					utils::memory::GetFunctionOpcodes((void*)0x636670)
				);
				if (!check_Dvar_GetInt)
				{
					modified_functions.push_back("Dvar_GetInt");
				}

				return utils::strings::FormatVector(modified_functions);
			}

			// makes sure that the game values like god mode, no target, etc. are not modified
			// i believe for this to properly work in coop, it will need to be run by the host
			std::string GetModifiedPlayerStates()
			{
				HANDLE handle = game::process::GetBlackOpsProcess();
				if (handle == NULL || handle == INVALID_HANDLE_VALUE)
				{
					return "";
				}

				vector<string> modified_player_states;

				const char* corpse_count_str = dvars::GetDvarInt("ai_corpseCount");
				bool corpse_count_converted;
				int corpse_count = utils::strings::ToInt(corpse_count_str, corpse_count_converted);

				if (corpse_count_converted)
				{
					if (corpse_count < 3)
					{
						modified_player_states.push_back("Modified Tiny Corpses (less than the normal count)");
					}
				}

				vector<int> player_state_addresses = { 0x01A79868, 0x01A79BB4, 0x01A79F00, 0x01A7A24C };

				// checks the player states for all 4
				for (int i = 0; i <= 3; i++)
				{
					int demi_god_mode = utils::memory::ReadInt(handle, player_state_addresses[i]) & 2;
					int god_mode = utils::memory::ReadInt(handle, player_state_addresses[i]) & 1;
					int no_target = utils::memory::ReadInt(handle, player_state_addresses[i]) & 4;

					if (!game::process::IsGameOpen())
					{
						return "";
					}

					std::string player_num = to_string(i + 1);

					if (demi_god_mode == 2)
					{
						modified_player_states.push_back("Demi God Mode (Player " + player_num + ")");
					}

					if (god_mode == 1)
					{
						modified_player_states.push_back("God Mode (Player " + player_num + ")");
					}

					if (no_target == 4)
					{
						modified_player_states.push_back("No Target (Player " + player_num + ")");
					}
				}

				return utils::strings::FormatVector(modified_player_states);
			}

			// checks for specific DLL files that should not be injected into bo1
			// this could range from a stealth patch to other files too
			bool IsStealthPatchInjected()
			{
				if (!game::process::IsGameOpen())
				{
					return false;
				}

				HANDLE handle = game::process::GetBlackOpsProcess();
				if (handle == NULL) {
					return false;
				}

				std::vector<HMODULE> hMods(1024);
				DWORD cbNeeded;
				bool result = false;

				if (EnumProcessModulesEx(handle, hMods.data(), hMods.size() * sizeof(HMODULE), &cbNeeded, LIST_MODULES_ALL)) {
					size_t moduleCount = cbNeeded / sizeof(HMODULE);
					for (size_t i = 0; i < moduleCount; i++) {
						// could cause a problem mid check if the game is closed suddenly
						if (!game::process::IsGameOpen())
						{
							return false;
						}

						if (IsSuspiciousModule(hMods[i], handle)) {
							result = true;
							break;
						}
					}
				}

				return result;
			}

			// makes sure a module isnt something like a stealth patch dll or something
			bool IsSuspiciousModule(HMODULE hModule, HANDLE hProcess) {
				TCHAR szModName[MAX_PATH];
				MODULEINFO modInfo;

				if (GetModuleFileNameEx(hProcess, hModule, szModName, sizeof(szModName) / sizeof(TCHAR))) {
					if (GetModuleInformation(hProcess, hModule, &modInfo, sizeof(modInfo))) {
						char modulePath[MAX_PATH];
						size_t convertedChars = 0;
						wcstombs_s(&convertedChars, modulePath, szModName, sizeof(modulePath));
						modulePath[sizeof(modulePath) - 1] = '\0';
						std::string dllPath = std::string(modulePath);

						// Check against suspicious_modules
						if (utils::files::GetMD5(dllPath) == "c3ceab590eb62f593a9cbdd59d588243") {
							return true;
						}

						// Check for suspicious module names
						if (dllPath.find("stealth_patch") != std::string::npos) {
							return true;
						}
					}
				}

				return false;
			}
		}
	}
}