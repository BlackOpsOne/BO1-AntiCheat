#include "processes.hpp"

#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>

#include "../utils/strings.hpp"

namespace anticheat {
	namespace integrity {
		namespace processes {
			// Store known cheat indicators
			std::vector<CheatingProcess> array_1;
			std::vector<CheatingProcess> array_2;
			std::vector<std::string> detected_programs;

			bool Initialize()
			{
				array_1 = {
					{ utils::strings::CasperTheGhost({ 0x43, 0x68, 0x65, 0x61, 0x74, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65 }), // "Cheat Engine"
					  utils::strings::CasperTheGhost({ 0x43, 0x68, 0x65, 0x61, 0x74, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65 }) },  // "Cheat Engine"
					{ utils::strings::CasperTheGhost({ 0x5A, 0x6F, 0x6C, 0x66, 0x65, 0x72, 0x6E, 0x6F, 0x73, 0x20, 0x42, 0x4F, 0x31, 0x20, 0x54, 0x72, 0x61, 0x69, 0x6E, 0x65 }), // "Zolfernos BO1 Trainer"
					  utils::strings::CasperTheGhost({ 0x5A, 0x6F, 0x6C, 0x66, 0x65, 0x72, 0x6E, 0x6F, 0x73 }) }, // "Zolfernos"
					{ utils::strings::CasperTheGhost({ 0x5A, 0x6F, 0x6C, 0x66, 0x65, 0x72, 0x6E, 0x6F, 0x73, 0x27, 0x73 }) , // "Zolferno's"
					  utils::strings::CasperTheGhost({ 0x5A, 0x6F, 0x6C, 0x66, 0x65, 0x72, 0x6E, 0x6F, 0x73 }) } // "Zolfernos"
				};

				array_2 = {
					{ utils::strings::CasperTheGhost({ 0x43, 0x68, 0x65, 0x61, 0x74, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65 }), // "Cheat Engine"
					  utils::strings::CasperTheGhost({ 0x63, 0x68, 0x65, 0x61, 0x74, 0x65, 0x6E, 0x67, 0x69, 0x6E }) }, // "cheatengine"
					{ utils::strings::CasperTheGhost({ 0x5A, 0x6F, 0x6C, 0x66, 0x65, 0x72, 0x6E, 0x6F, 0x73, 0x20, 0x42, 0x4F, 0x31, 0x20, 0x54, 0x72, 0x61, 0x69, 0x6E, 0x65 }), // "Zolfernos BO1 Trainer"
					  utils::strings::CasperTheGhost({ 0x5A, 0x6F, 0x6C, 0x66, 0x65, 0x72, 0x6E, 0x6F, 0x73 }) } // "Zolfernos"
				};

				return true;
			}

			void FilterWindowTitles()
			{
				HWND hwnd = GetTopWindow(nullptr); // start with the first window

				while (hwnd)
				{
					char title[256] = { 0 };

					if (GetWindowTextA(hwnd, title, sizeof(title)) > 0)
					{
						// loop through the window titles we know of
						for (CheatingProcess target : array_1)
						{
							// if we have a match and it doesnt already exist in the queue, add it
							if (utils::strings::ConstCharContains(title, target.value.c_str())
								&& !AlreadyDetected(target.cheat))
							{
								detected_programs.push_back(target.cheat);
								break;
							}
						}
					}

					// head to the next window
					hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
				}
			}

			void FilterProcessNames()
			{
				// Get all process IDs
				DWORD processes[1024], needed;
				if (!EnumProcesses(processes, sizeof(processes), &needed))
				{
					return;
				}

				// go through each process id
				for (unsigned int i = 0; i < needed / sizeof(DWORD); ++i)
				{
					DWORD pid = processes[i];
					if (pid == 0) continue;

					// get process name
					HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
					if (process_handle)
					{
						char process_name[MAX_PATH] = { 0 };
						if (GetModuleFileNameExA(process_handle, NULL, process_name, sizeof(process_name)) > 0)
						{
							// get the file name instead of the full path
							std::string process_name_str = process_name;
							size_t pos = process_name_str.find_last_of("\\/");
							if (pos != std::string::npos)
							{
								process_name_str = process_name_str.substr(pos + 1);
							}

							// check if the process name matches any target
							for (CheatingProcess target : array_2)
							{
								// if we have a match and it doesnt already exist in the queue, add it
								if (utils::strings::ConstCharContains(process_name_str.c_str(), target.value.c_str())
									&& !AlreadyDetected(target.cheat))
								{
									detected_programs.push_back(target.cheat);
									break;
								}
							}
						}

						CloseHandle(process_handle);
					}
				}
			}

			// loops through the list of already detected cheats to see if we already picked up on it
			bool AlreadyDetected(std::string target_cheat)
			{
				for (std::string cheat : detected_programs)
				{
					if (target_cheat == cheat)
					{
						return true;
					}
				}
				return false;
			}

			// returns the list of cheats
			std::string GetBlacklistedPrograms()
			{
				if (array_1.empty() || array_2.empty()) {
					Initialize();
				}

				FilterWindowTitles();
				FilterProcessNames();

				return utils::strings::FormatVector(detected_programs);
			}
		}
	}
}