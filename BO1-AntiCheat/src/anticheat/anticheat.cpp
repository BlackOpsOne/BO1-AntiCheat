#include <SFML/Graphics.hpp>

#include "../constants.h"

#include "../game/game.hpp"

#include "../game/process.hpp"

#include "integrity/zone.hpp"

#include "../statuses.h"

#include "../utils/memory.hpp"

#include "integrity/config.hpp"

#include "integrity/engine.hpp"

#include "helper/helper.hpp"

#include "integrity/dvars.hpp"

#include "../utils/strings.hpp"

using namespace std;

bool initialized = false;
bool performed_integrity_check = false;
bool cheating_detected = false;
bool notified_cheats_detected = false;
bool integrity_check_override = false;

const char* current_map = "";
const char* last_map = "";

vector<std::string> cheats_found;

std::string main_status = Statuses::GAME_NOT_CONNECTED;
std::string info_status = "";

namespace anticheat {
    void Initialize()
    {
        // initialize we need to look for in zone
        integrity::zone::InitializeZoneQueue();

        // set which config binds are not allowed
        integrity::config::InitializeConfigQueue();
    }

    // make sure the map is actually a map, excluding the main menu map
    bool IsMapValid(const char* map_name)
    {
        return map_name != nullptr && std::strcmp(map_name, "") != 0 && std::strcmp(map_name, "frontend") != 0;
    }

    // displays the "Game not connected." message
    void OnGameClosed()
    {
        main_status = Statuses::GAME_NOT_CONNECTED;
        info_status = Statuses::WAITING_FOR_GAME_TO_OPEN;
        initialized = false;
        performed_integrity_check = false;
    }

    // adds a cheating method to a list, this will be shown in a second window
    void AddCheatFound(string cheating_method)
    {
        cheats_found.push_back(cheating_method);
    }

    // pops up the second window that explains what was found
    // this is especially good for players who may accidentally leave something in their files
    void NotifyCheatsDetected()
    {
        cheating_detected = true;

        // notify the player
        main_status = Statuses::CHEATING_DETECTED;
        info_status = Statuses::MORE_INFO_WINDOW;
        notified_cheats_detected = true;

        string cheats = "The following cheating methods were detected:\n";
        for (string cheat_found : cheats_found)
        {
            cheats += "\n- " + cheat_found;
        }

        MessageBoxA(NULL, cheats.c_str(), "BO1 Anti Cheat (Detections)", MB_OK | MB_ICONERROR);
    }

    // handles all checks to ensure a bo1 game is not cheated
    void AttemptIntegrityCheck()
    {
        if (!game::process::IsGameOpen() || cheating_detected)
        {
            return;
        }

        last_map = current_map;
        current_map = integrity::dvars::CallGetDvarString("mapname");

        bool map_changed = std::strcmp(current_map, last_map) != 0;
        if (map_changed)
        {
            performed_integrity_check = false;
        }

        bool ff_checking_after_map_load = integrity_check_override && info_status == Statuses::PATCHES_CHECKED_AFTER_MAP_LOAD;

        // only check when the map is being loaded/quit
        if (!performed_integrity_check)
        {
            if (IsMapValid(current_map) || integrity_check_override)
            {
                main_status = Statuses::CHECKING_FOR_PATCHES;
                if (!ff_checking_after_map_load)
                {
                    info_status = "This may take a moment";
                }

                integrity_check_override = false;
                performed_integrity_check = true;

                Sleep(1000); // puts us in the loading screen so they cant edit the files

                // check for modification of actual engine functions
                string modified_functions = integrity::engine::ModifiedEngineFunctions();
                if (modified_functions != "")
                {
                    AddCheatFound("Modified engine functions: " + modified_functions);
                }

                // check for game_mod
                if (game::IsGameModLoaded())
                {
                    AddCheatFound("A known Game Mod dll was injected.");
                }

                // check for any unknown zones, can be used to cheat
                string extra_zones = integrity::zone::CheckForExtraItemsInZone();
                if (extra_zones != "")
                {
                    AddCheatFound("Unknown items in zone folder: " + extra_zones);
                }

                // check for any extra files, they should not be there
                string extra_common_files = integrity::zone::GetExtraFilesInZone("Common");
                if (extra_common_files != "")
                {
                    AddCheatFound("Extra files found in zone\\Common, could be a stealth patch: " + extra_common_files);
                }

                string lang_zone = game::GetLanguageZoneName();
                string extra_lang_files = integrity::zone::GetExtraFilesInZone(lang_zone);
                if (extra_lang_files != "")
                {
                    AddCheatFound("Extra files found in zone\\Common" + lang_zone + ", could be a stealth patch: " + extra_lang_files);
                }

                // check for any known stealth patch injections
                if (integrity::engine::IsStealthPatchInjected())
                {
                    AddCheatFound("A known stealth patch DLL was injected.");
                }

                // list off modified common files
                string modified_fastfiles = integrity::zone::GetModifiedFastFiles();
                if (modified_fastfiles != "")
                {
                    AddCheatFound("Modified fastfiles: " + modified_fastfiles);
                }

                // if theres any cheats detected, notify them and crash bo1
                if (cheats_found.size() > 0)
                {
                    NotifyCheatsDetected();
                }
                else
                {
                    // otherwise tell them they're good
                    main_status = Statuses::NO_PATCHING_DETECTED;

                    // if we're not checking after map load then notify of extra checks during the game
                    if (!ff_checking_after_map_load)
                    {
                        info_status = Statuses::WILL_CONTINUE_SEARCH;
                    }
                    else
                    {
                        // notify viewers patches are checked after a map load
                        info_status = Statuses::PATCHES_CHECKED_AFTER_MAP_LOAD;
                    }

                    // we have to disable engine checks for custom-fx
                    if (game::IsCustomFxToolLoaded())
                    {
                        info_status = Statuses::ENGINE_CHECKS_DISABLED;
                    }
                }
            }
        }

        // check game values such as godmode, box movable, etc.
        if (IsMapValid(current_map))
        {
            string playerStates = integrity::engine::GetModifiedPlayerStates();
            if (playerStates != "")
            {
                AddCheatFound(playerStates);
                NotifyCheatsDetected();
                return;
            }

            // check config but only if its been modified
            string cheatingCommands = integrity::config::GetCheatingCommands();
            if (cheatingCommands != "")
            {
                AddCheatFound("Disallowed commands in the config: " + cheatingCommands);
                NotifyCheatsDetected();
                return;
            }
        }

        if (game::GetMapId() == Constants::MAIN_MENU_ID)
        {
            main_status = Statuses::GAME_CONNECTED;
            info_status = Statuses::WAITING_FOR_MAP_LOAD_QUIT;
            performed_integrity_check = false;
        }
    }

    // waits for the game to be opened before we run any checks
    void WaitForBlackOpsProcess()
    {
        if (cheating_detected)
        {
            return;
        }

        if (!helper::CheckHelperIntegrity() && !cheating_detected)
        {
            AddCheatFound(Constants::HELPER_NAME + " is required to run the anticheat.");
            NotifyCheatsDetected();
            return;
        }

        if (!game::process::IsGameOpen() && !cheating_detected)
        {
            OnGameClosed();
            main_status = Statuses::GAME_NOT_CONNECTED;
            info_status = Statuses::WAITING_FOR_GAME_TO_OPEN;
            game::CheckForAllowedTools();
            return;
        }

        // this checks for certain things once, think of it as an "OnGameOpen"
        if (!initialized)
        {
            if (current_map != "frontend")
            {
                info_status = "Not in a map";
                main_status = Statuses::GAME_CONNECTED;

                // check for patching methods mid game
                if (IsMapValid(current_map))
                {
                    integrity_check_override = true;
                    info_status = Statuses::PATCHES_CHECKED_AFTER_MAP_LOAD;
                }
            }

            if (!helper::InjectHelper())
            {
                MessageBoxA(NULL, "Could not initialize helper dll.", "BO1 Anti Cheat (Error)", MB_OK | MB_ICONERROR);
                ExitProcess(0);
                return;
            }

            game::CheckForAllowedTools();
            initialized = true;
        }

        AttemptIntegrityCheck();
        Sleep(50);
    }

    bool CheatingDetected()
    {
        return cheating_detected;
    }

    std::string GetMainStatus()
    {
        return main_status;
    }

    std::string GetInfoStatus()
    {
        return info_status;
    }
}