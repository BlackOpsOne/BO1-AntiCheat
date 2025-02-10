#include "dvars.hpp"

#include <windows.h>
#include <iostream>
#include <filesystem>

#include "../../utils/files.hpp"
#include "../../utils/memory.hpp"

#include "../../game/process.hpp"
#include "../../game/game.hpp"

#include "checksums.h"
#include "../../constants.h"

#include "../../utils/strings.hpp"

#include <vector>

std::vector<DvarValueEntry> dvar_int_queue;
std::vector<DvarValueEntry> dvar_str_queue;
std::vector<DvarValueEntry> dvar_double_entry;

bool zombie_taunt_initalized = false;
bool zombie_reachin_initalized = false;

namespace anticheat {
	namespace integrity {
		namespace dvars {
			void InitDvarQueue()
			{
				dvar_int_queue = {
					{ "cl_noprint", "0" },
					{ "magic_chest_movable", "1" },
					{ "ai_disableSpawn", "0" },
					{ "zombie_reachin_freq", "50" },
					{ "zombie_taunt_freq", "5" },
					{ "cg_hudDamageIconTime", "2000" },
					{ "developer", "0" },
					{ "revive_trigger_radius", "75" },
					{ "player_reviveTriggerRadius", "64" },
					{ "g_speed", "190" },
					{ "ai_badPathSpam", "0" },
					{ "sv_fps", "20" }
				};

				dvar_str_queue = {
					{ "currentDifficulty", "normal" },
					{ "scr_force_weapon", "" },
					{ "scr_force_quantum_bomb_result", "" }
				};

				dvar_double_entry = {
					{ "g_banzai_player_fov_buffer", "0.200000" }
				};
			}

			void Cleanup()
			{
				zombie_taunt_initalized = false;
				zombie_reachin_initalized = false;
			}

			std::string GetModifiedDvars()
			{
				if (!game::process::IsGameOpen())
				{
					return "";
				}
				vector<string> modified_dvars;

				// check the integer based dvars
				for (DvarValueEntry int_entry : dvar_int_queue)
				{
					if (!game::process::IsGameOpen())
					{
						return "";
					}

					const char* dvar_name = int_entry.dvar_name;
					const char* expected_value = int_entry.expected_value;

					const char* game_value = GetDvarInt(dvar_name);
					const char* map_name = game::GetMapName();

					// special logic for the zombie_reachin_freq and zombie_taunt_freq dvars
					// these are only initialized when a zombie reaches through a window, or breaks off a part.

					if (!zombie_reachin_initalized)
					{
						if (utils::strings::CompareConstChar(dvar_name, "zombie_reachin_freq"))
						{
							if (utils::strings::CompareConstChar(game_value, "0"))
							{
								continue;
							}
							else
							{
								zombie_reachin_initalized = true;
							}
						}
					}

					if (!zombie_taunt_initalized)
					{
						if (utils::strings::CompareConstChar(dvar_name, "zombie_taunt_freq"))
						{
							if (utils::strings::CompareConstChar(game_value, "0"))
							{
								continue;
							}
							else
							{
								zombie_taunt_initalized = true;
							}
						}
					}

					// special check for "magic_chest_movable" on nacht and doa
					if (
						utils::strings::CompareConstChar(dvar_name, "magic_chest_movable") &&
						(utils::strings::CompareConstChar(map_name, "zombie_cod5_prototype") ||
							utils::strings::CompareConstChar(map_name, "zombietron"))
						)
					{
						continue;
					}

					// special logic for specific uninitialized dvars on doa
					// these don't actually matter on the map
					// they arent initialized on doa either so they can cause some false flags
					if (
						utils::strings::CompareConstChar(map_name, "zombietron")
						&& (
							utils::strings::CompareConstChar(dvar_name, "zombie_reachin_freq") ||
							utils::strings::CompareConstChar(dvar_name, "zombie_taunt_freq") ||
							utils::strings::CompareConstChar(dvar_name, "revive_trigger_radius")
							)
						)
					{
						continue;
					}

					if (!utils::strings::CompareConstChar(game_value, expected_value))
					{
						std::string name_str(dvar_name);
						modified_dvars.push_back(name_str);
					}
				}

				// check the string based dvars
				for (DvarValueEntry string_entry : dvar_str_queue)
				{
					if (!game::process::IsGameOpen())
					{
						return "";
					}

					const char* dvar_name = string_entry.dvar_name;
					const char* expected_value = string_entry.expected_value;
					const char* game_value = GetDvarString(dvar_name);

					// this can happen when the dvar isnt initialized
					if (game_value == nullptr)
					{
						continue;
					}

					if (!utils::strings::CompareConstChar(game_value, expected_value))
					{
						std::string name_str(dvar_name);
						modified_dvars.push_back(name_str);
					}
				}

				// check double entries
				for (DvarValueEntry double_entry : dvar_double_entry)
				{
					if (!game::process::IsGameOpen())
					{
						return "";
					}

					const char* dvar_name = double_entry.dvar_name;
					const char* expected_value = double_entry.expected_value;
					const char* game_value = GetDvarDouble(dvar_name);

					// this can happen when the dvar isnt initialized
					if (game_value == nullptr)
					{
						continue;
					}

					if (!utils::strings::CompareConstChar(game_value, expected_value))
					{
						std::string name_str(dvar_name);
						modified_dvars.push_back(name_str);
					}
				}

				return utils::strings::FormatVector(modified_dvars);
			}

			const char* GetDvarString(const char* dvar_name)
			{
				return CallHelperFunction(dvar_name, "GetDvarString");
			}

			const char* GetDvarInt(const char* dvar_name)
			{
				return CallHelperFunction(dvar_name, "GetDvarInt");
			}

			const char* GetDvarBool(const char* dvar_name)
			{
				return CallHelperFunction(dvar_name, "GetDvarBool");
			}

			const char* GetDvarDouble(const char* dvar_name)
			{
				return CallHelperFunction(dvar_name, "GetDvarDouble");
			}

			const char* CallHelperFunction(const char* dvar_name, const char* function_name)
			{
				HANDLE handle = game::process::GetBlackOpsProcess();
				if (handle == NULL || handle == INVALID_HANDLE_VALUE)
				{
					return nullptr;
				}

				// Get the module of the helper DLL
				HMODULE helper_module = utils::memory::GetRemoteModuleHandle(handle, Constants::HELPER_NAME.c_str());
				if (!helper_module)
				{
					return nullptr;
				}

				// Get function address
				FARPROC func_address = utils::memory::GetRemoteProcAddress(handle, helper_module, function_name);
				if (!func_address)
				{
					return nullptr;
				}

				// Allocate memory in the remote process for the Dvar name
				size_t len = strlen(dvar_name) + 1;
				LPVOID dvar_name_alloc = VirtualAllocEx(handle, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
				if (!dvar_name_alloc)
				{
					return nullptr;
				}

				// Write the Dvar name to the remote process memory
				WriteProcessMemory(handle, dvar_name_alloc, dvar_name, len, nullptr);

				// Create thread in remote process to call the function
				HANDLE remote_thread = CreateRemoteThread(handle, nullptr, 0, (LPTHREAD_START_ROUTINE)func_address, dvar_name_alloc, 0, nullptr);
				if (!remote_thread)
				{
					VirtualFreeEx(handle, dvar_name_alloc, 0, MEM_RELEASE);
					return nullptr;
				}

				// Wait for the function call to complete
				WaitForSingleObject(remote_thread, INFINITE);

				// Get the return value (which is a pointer in remote memory)
				DWORD remote_ptr;
				GetExitCodeThread(remote_thread, &remote_ptr);

				if (!remote_ptr)  // Check if allocation in the remote process failed
				{
					VirtualFreeEx(handle, dvar_name_alloc, 0, MEM_RELEASE);
					CloseHandle(remote_thread);
					return nullptr;
				}

				// Read the string from remote memory
				char buffer[256] = { 0 };
				ReadProcessMemory(handle, (LPCVOID)remote_ptr, buffer, sizeof(buffer), nullptr);

				// Free allocated memory in remote process
				VirtualFreeEx(handle, dvar_name_alloc, 0, MEM_RELEASE);
				VirtualFreeEx(handle, (LPVOID)remote_ptr, 0, MEM_RELEASE); // Free the string memory
				CloseHandle(remote_thread);

				// Return a local copy
				return _strdup(buffer);
			}

		}
	}
}