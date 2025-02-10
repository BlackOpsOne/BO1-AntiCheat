#include <string>

#ifndef Constants
#define Constants

const static std::string VERSION = "1.0.2";
const static std::string GUIDELINES = "- Show the BO1-AntiCheat.exe file properties. Right click -> Properties, the exact amount of bytes of \"Size on disk\" must be shown.\n- Show yourself opening the same tool.\n- Display the tool on your stream/recording.";

const static std::string HELPER_NAME = "BO1-AntiCheat.dll";
const static std::string HELPER_MD5 = "3e20ba736e9aea7de8a3dcd4f9e7b093";

// addresses
const static int C_MAPADDRESS = 0x02F67B6C;
const static int C_TIMEADDRESS = 0x01A79870;
const static int C_LANGADDRESS = 0x0276C0D0;
const static int C_GAMEPAUSED = 0x8902B4;

// map ids
const static int NO_MAP = 0;
const static int INVALID_MAP = -1;

#endif