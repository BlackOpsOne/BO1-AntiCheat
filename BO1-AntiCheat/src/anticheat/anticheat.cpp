#include <SFML/Graphics.hpp>

#include "../constants.h"

#include "../game/game.hpp"

#include "../game/process.hpp"

#include "integrity/zone.hpp"

#include "../statuses.h"

#include "../utils/memory.hpp"

#include "integrity/config.hpp"

#include "integrity/engine.hpp"

#include "integrity/dvars.hpp"

#include "../utils/strings.hpp"

using namespace std;

bool initialized = false;
bool performed_integrity_check = false;
bool cheating_detected = false;
bool notified_cheats_detected = false;
bool check_during_game = false;

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

        // set which dvars to check
        integrity::dvars::InitDvarQueue();

        // set which config binds are not allowed
        integrity::config::InitializeConfigQueue();
    }

    // make sure the map is actually a map, excluding the main menu map
    bool IsMapValid(const char* map_name)
    {
        return map_name != nullptr && std::strcmp(map_name, "") != 0 && std::strcmp(map_name, "frontend") != 0;
    }

    void OnGameOpened()
    {
        const char* map_name = game::GetMapName();
        if (IsMapValid(map_name))
        {
            check_during_game = true;
            main_status = Statuses::CHECKING_FOR_PATCHES;
            info_status = "This may take a moment";
        }
        else
        {
            main_status = Statuses::GAME_CONNECTED;
            info_status = Statuses::WAITING_FOR_MAP_LOAD_QUIT;
        }

        game::CheckForAllowedTools();
    }

    // displays the "Game not connected." message
    void OnGameClosed()
    {
        main_status = Statuses::GAME_NOT_CONNECTED;
        info_status = Statuses::WAITING_FOR_GAME_TO_OPEN;
        initialized = false;
        performed_integrity_check = false;
        integrity::dvars::Cleanup();
    }

    // adds a cheating method to a list, this will be shown in a second window
    void AddCheatFound(std::string cheating_method)
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

        std::string cheats = "The following cheating methods were detected:\n";
        for (std::string cheat_found : cheats_found)
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

        const char* map_name = integrity::dvars::GetDvarString("mapname");
        if (map_name == nullptr)
        {
            return;
        }

        last_map = current_map;
        current_map = map_name;

        bool map_changed = std::strcmp(current_map, last_map) != 0;
        if (map_changed)
        {
            performed_integrity_check = false;
        }

        // only check when the map is being loaded/quit
        if (!performed_integrity_check)
        {
            if (IsMapValid(current_map) || check_during_game)
            {
                main_status = Statuses::CHECKING_FOR_PATCHES;
                info_status = "This may take a moment";
                performed_integrity_check = true;

                Sleep(1000); // puts us in the loading screen so they cant edit the files

                // check for modification of actual engine functions
                std::string modified_functions = integrity::engine::ModifiedEngineFunctions();
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
                std::string extra_zones = integrity::zone::CheckForExtraItemsInZone();
                if (extra_zones != "")
                {
                    AddCheatFound("Unknown items in zone folder: " + extra_zones);
                }

                // check for any extra files, they should not be there
                std::string extra_common_files = integrity::zone::GetExtraFilesInZone("Common");
                if (extra_common_files != "")
                {
                    AddCheatFound("Extra files found in zone\\Common, could be a stealth patch: " + extra_common_files);
                }

                std::string lang_zone = game::GetLanguageZoneName();
                std::string extra_lang_files = integrity::zone::GetExtraFilesInZone(lang_zone);
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
                std::string map_name_str(current_map);
                std::string modified_fastfiles = integrity::zone::GetModifiedFastFiles(map_name_str);
                if (modified_fastfiles != "")
                {
                    AddCheatFound("Modified fastfiles: " + modified_fastfiles);
                }

                // if theres any cheats detected, make it show on the display, this way people know something wasnt right
                if (cheats_found.size() > 0)
                {
                    NotifyCheatsDetected();
                }
                else
                {
                    // otherwise tell them they're good, but with some special conditions
                    main_status = Statuses::NO_PATCHING_DETECTED;
                    info_status = check_during_game ? Statuses::PATCHES_CHECKED_AFTER_MAP_LOAD : Statuses::WILL_CONTINUE_SEARCH;
                    check_during_game = false;

                    // we have to disable engine checks for custom-fx
                    if (game::IsCustomFxToolLoaded())
                    {
                        info_status = Statuses::ENGINE_CHECKS_DISABLED;
                    }
                }
            }
        }

        // check game values such as godmode, box movable, etc.
        // only do this when the map id is set, thats how we know they're in the map
        if (IsMapValid(current_map) && game::IsInMap())
        {
            std::string playerStates = integrity::engine::GetModifiedPlayerStates();
            if (playerStates != "")
            {
                AddCheatFound(playerStates);
                NotifyCheatsDetected();
                return;
            }

            /*const char* dvar_name = "revive_trigger_radius";
            std::cout << dvar_name << " value: " << integrity::dvars::GetDvarInt(dvar_name) << "\n";*/

            std::string dvars = integrity::dvars::GetModifiedDvars();
            if (dvars != "")
            {
                AddCheatFound("Modified Dvars: " + dvars);
                NotifyCheatsDetected();
                return;
            }

            // check config but only if its been modified
            std::string cheatingCommands = integrity::config::GetCheatingCommands();
            if (cheatingCommands != "")
            {
                AddCheatFound("Disallowed commands in the config: " + cheatingCommands);
                NotifyCheatsDetected();
                return;
            }
        }

        if (std::strcmp(map_name, "frontend") == 0)
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
            OnGameOpened();
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