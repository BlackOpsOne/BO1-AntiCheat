#include <string>

#ifndef Constants
#define Constants

const static std::string VERSION = "1.0.2";
const static std::string GUIDELINES = "- Show the BO1-AntiCheat.exe file properties. Right click -> Properties, the exact amount of bytes of \"Size on disk\" must be shown.\n- Show yourself opening the same tool.\n- Display the tool on your stream/recording.";

const static std::string INTERNALS_NAME = "BO1-AntiCheat.dll";
const static std::string INTERNALS_HASH = "979e9d80bbec6357708c8feb79a6fef8";

// addresses
const static int C_MAPADDRESS = 0x02F67B6C;
const static int C_TIMEADDRESS = 0x01A79870;
const static int C_LANGADDRESS = 0x0276C0D0;
const static int C_GAMEPAUSED = 0x8902B4;

// map ids
const static int MAIN_MENU_ID = 5;
const static int NO_MAP = 0;
const static int INVALID_MAP = -1;

#endif