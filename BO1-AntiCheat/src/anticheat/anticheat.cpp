#include <SFML/Graphics.hpp>

#include "constants.h"
#include "statuses.h"

#include "game/game.hpp"
#include "game/process.hpp"
#include "game/zone.hpp"

#include "game/config.hpp"
#include "game/engine.hpp"
#include "game/dvars.hpp"

#include "utils/memory.hpp"
#include "utils/strings.hpp"
#include "internals/internals.hpp"
#include "integrity/integrity.hpp"
#include "integrity/processes.hpp"

#include <vector>

bool initialized = false;
bool performed_zone_check = false;
bool cheating_detected = false;
bool notified_cheats_detected = false;
bool check_during_game = false;

bool has_loaded_into_game = false; // tracker for when they load into the main menu the first time

const char* current_map = "";
const char* last_map = "";

std::vector<std::string> cheats_found;

std::string main_status = Statuses::GAME_NOT_CONNECTED;
std::string info_status = "";

namespace anticheat {
    void Initialize()
    {
        // initialize we need to look for in zone
        game::zone::InitializeZoneQueue();

        // set which dvars to check
        game::dvars::InitDvarQueue();

        // set which config binds are not allowed
        game::config::InitializeConfigQueue();
    }

    // make sure the map is actually a map, excluding the main menu map
    bool IsMapValid(const char* map_name)
    {
        return map_name != nullptr 
            && std::strcmp(map_name, "") != 0 
            && std::strcmp(map_name, "frontend") != 0;
    }

    void OnGameOpened()
    {
        if (!internals::Inject())
        {
            MessageBoxA(NULL, "Couldn't initialize internals.", "BO1 Anti Cheat (Error)", MB_OK | MB_ICONERROR);
            ExitProcess(0);
            return;
        }

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
        performed_zone_check = false;
        game::dvars::Cleanup();
        game::process::Cleanup();
    }

    // adds a cheating method to a list, this will be shown in a second window
    void AddCheatFound(std::string cheating_method)
    {
        cheats_found.push_back(cheating_method);
    }

    // pops up the second window that explains what was found
    // this is especially good for players who may accidentally leave something in their files
    void NotifyCheatsDetected(bool show_detections = true)
    {
        // notify the player
        cheating_detected = true;
        main_status = Statuses::CHEATING_DETECTED;
        info_status = Statuses::MORE_INFO_WINDOW;
        notified_cheats_detected = true;

        // stop the integrity checks thread
        //integrity::StopChecksThread();

        // show the window, but in certain situations don't
        if (show_detections)
        {
            std::string cheats = "The following cheating methods were detected:\n";
            for (std::string cheat_found : cheats_found)
            {
                cheats += "\n- " + cheat_found;
            }

            MessageBoxA(NULL, cheats.c_str(), "BO1 Anti Cheat (Detections)", MB_OK | MB_ICONERROR);
        }
    }

    // handles all checks to ensure a bo1 game is not cheated
    void AttemptIntegrityCheck()
    {
        if (!game::process::IsGameOpen() || cheating_detected)
        {
            return;
        }

        const char* map_name = game::dvars::GetDvarString("mapname");
        if (map_name == nullptr)
        {
            return;
        }

        // okay, they've gotten in the main menu
        if (!has_loaded_into_game && game::GetMapId() == Constants::MAIN_MENU_ID)
        {
            has_loaded_into_game = true;
        }

        // don't do anything if they havent gotten to the main menu for the first time
        if (!has_loaded_into_game)
        {
            return;
        }

        last_map = current_map;
        current_map = map_name;

        bool map_changed = std::strcmp(current_map, last_map) != 0;
        if (map_changed)
        {
            performed_zone_check = false;
        }

        // only check when the map is being loaded/quit
        if (!performed_zone_check)
        {
            if (IsMapValid(current_map) || check_during_game)
            {
                main_status = Statuses::CHECKING_FOR_PATCHES;
                info_status = "This may take a moment";
                performed_zone_check = true;

                Sleep(1000); // puts us in the loading screen so they cant edit the files

                // check for modification of actual engine functions
                std::string modified_functions = game::engine::ModifiedEngineFunctions();
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
                std::string extra_zones = game::zone::CheckForExtraItemsInZone();
                if (extra_zones != "")
                {
                    AddCheatFound("Unknown items in zone folder: " + extra_zones);
                }

                // check for any extra files, they should not be there
                std::string extra_common_files = game::zone::GetExtraFilesInZone("Common");
                if (extra_common_files != "")
                {
                    AddCheatFound("Extra files found in zone\\Common, could be a stealth patch: " + extra_common_files);
                }

                std::string lang_zone = game::GetLanguageZoneName();
                std::string extra_lang_files = game::zone::GetExtraFilesInZone(lang_zone);
                if (extra_lang_files != "")
                {
                    AddCheatFound("Extra files found in zone\\Common" + lang_zone + ", could be a stealth patch: " + extra_lang_files);
                }

                // check for any known stealth patch injections
                if (game::engine::IsStealthPatchInjected())
                {
                    AddCheatFound("A known stealth patch DLL was injected.");
                }

                // list off modified common files
                std::string map_name_str(current_map);
                std::string modified_fastfiles = game::zone::GetModifiedFastFiles(map_name_str);
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
            std::string playerStates = game::engine::GetModifiedPlayerStates();
            if (playerStates != "")
            {
                AddCheatFound(playerStates);
                NotifyCheatsDetected();
                return;
            }

            std::string dvars = game::dvars::GetModifiedDvars();
            if (dvars != "")
            {
                AddCheatFound("Modified Dvars: " + dvars);
                NotifyCheatsDetected();
                return;
            }

            // check config but only if its been modified
            std::string cheatingCommands = game::config::GetCheatingCommands();
            if (cheatingCommands != "")
            {
                AddCheatFound("Disallowed commands in the config: " + cheatingCommands);
                NotifyCheatsDetected();
                return;
            }

            // check for filmtweaks on ascension
            const char* r_filmUseTweaks = game::dvars::GetDvarInt("r_filmUseTweaks");
            if (utils::strings::CompareConstChar(r_filmUseTweaks, "1") 
                && utils::strings::CompareConstChar(current_map, "zombie_cosmodrome"))
            {
                AddCheatFound("Film Tweaks are enabled on a map that disallows it.");
                NotifyCheatsDetected();
                return;
            }
        }

        // always check for developer_script
        const char* developer_script = game::dvars::GetDvarInt("developer_script");
        if (utils::strings::CompareConstChar(developer_script, "1"))
        {
            AddCheatFound("Developer Scripts were enabled.");
            NotifyCheatsDetected();
            return;
        }

        // fps checks
        const char* com_maxfps_str = game::dvars::GetDvarInt("com_maxfps");
        bool maxfps_converted = false;
        int com_maxfps = utils::strings::ToInt(com_maxfps_str, maxfps_converted);

        if (maxfps_converted)
        {
            // can not be above 250
            if (com_maxfps > 250)
            {
                AddCheatFound("Max FPS is set higher than 250");
                NotifyCheatsDetected();
                return;
            }

            // can not be below 24
            if (com_maxfps < 24)
            {
                AddCheatFound("Max FPS is set lower than 24");
                NotifyCheatsDetected();
                return;
            }
        }

        if (utils::strings::CompareConstChar(map_name, "frontend"))
        {
            main_status = Statuses::GAME_CONNECTED;
            info_status = Statuses::WAITING_FOR_MAP_LOAD_QUIT;
            performed_zone_check = false;
        }
    }

    // waits for the game to be opened before we run any checks
    void WaitForBlackOpsProcess()
    {
        if (cheating_detected)
        {
            return;
        }

        if (!internals::CheckDLL_Integrity() && !cheating_detected)
        {
            NotifyCheatsDetected(false);
            info_status = Constants::INTERNALS_NAME + " was modified or not found";
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

        // now if the game is open, run the main logic
        if (game::process::IsGameOpen())
        {
            // initialize
            if (!initialized)
            {
                OnGameOpened();
                initialized = true;
            }

            AttemptIntegrityCheck();
        }

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

    void Cleanup()
    {
        has_loaded_into_game = false;
        integrity::Cleanup();
    }
}