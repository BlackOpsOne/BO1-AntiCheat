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

			std::string ModifiedEngineFunctions()
			{
				vector<string> modified_functions;

				// if custom fx is not present, we can check for this safely
				// we notify users that this check was not run if custom fx is present
				if (!game::IsCustomFxToolLoaded())
				{
					// check "Com_LoadLevelFastFiles", this is how people are stealth patching
					int Com_LoadLevelFastFiles = 0x004C8890;
					std::vector<BYTE> expected_ff_opcodes = { 0x83, 0xec, 0x7c, 0x53, 0x55 };
					std::vector<BYTE> gotten_ff_opcodes = utils::memory::GetFunctionOpcodes((void*)Com_LoadLevelFastFiles, expected_ff_opcodes.size());

					if (!gotten_ff_opcodes.empty() && !utils::memory::DoOpcodesMatch(gotten_ff_opcodes, expected_ff_opcodes))
					{
						if (game::process::IsGameOpen())
						{
							modified_functions.push_back("Com_LoadLevelFastFiles");
						}
					}
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

				vector<int> player_state_addresses = { 0x01A79868, 0x01A79BB4, 0x01A79F00, 0x01A7A24C };
				vector<string> modified_player_states;

				if (dvars::CallGetDvarBool("cl_noprint"))
				{
					modified_player_states.push_back("No Print");
				}

				// checks the player states for all 4
				for (int i = 0; i <= 3; i++)
				{
					int demi_god_mode = utils::memory::ReadInt(handle, player_state_addresses[i]) & 2;
					int god_mode = utils::memory::ReadInt(handle, player_state_addresses[i]) & 1;
					int no_target = utils::memory::ReadInt(handle, player_state_addresses[i]) & 4;

					string player = to_string(i + 1);

					if (demi_god_mode == 2)
					{
						modified_player_states.push_back("Demi God Mode (Player " + player + ")");
					}

					if (god_mode == 1)
					{
						modified_player_states.push_back("God Mode (Player " + player + ")");
					}

					if (no_target == 4)
					{
						modified_player_states.push_back("No Target (Player " + player + ")");
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