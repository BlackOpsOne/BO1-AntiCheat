#include <Windows.h>

#include <string>

using namespace std;

#ifndef Statuses
#define Statuses

// main statuses
const static std::string COULDNT_CHECK_UPDATES = "There was a problem checking for updates.\nPlease try again later.";
const static std::string GAME_CONNECTED = "Game connected";
const static std::string GAME_NOT_CONNECTED = "Game not connected";
const static std::string CHEATING_DETECTED = "A known cheating method was detected";
const static std::string MORE_INFO_WINDOW = "More information can be found on the detections window";
const static std::string CHECKING_FOR_PATCHES = "Checking for known patching methods";
const static std::string NO_PATCHING_DETECTED = "No known patching methods detected";

// secondary statuses
const static std::string WAITING_FOR_MAP_LOAD_QUIT = "Waiting for the player to load a map";
const static std::string WAITING_FOR_GAME_TO_OPEN = "Please open a Steam or BGamer copy of the game";
const static std::string WILL_CONTINUE_SEARCH = "Still looking for other cheating methods";
const static std::string PATCHES_CHECKED_AFTER_MAP_LOAD = "Patches unreliably checked because a map was already loaded";
const static std::string ENGINE_CHECKS_DISABLED = "Some engine checks were ignored due to Custom-FX";

#endif