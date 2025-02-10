#include <string>
#include <fstream>
#include <windows.h>
#include <iostream>
#include <filesystem>
#include "md5.h"

namespace anticheat {
	namespace utils {
		namespace files {
			std::string GetMD5(std::string file);
			std::string GetAntiCheatPath();
			int SizeInBytes(std::string file);
		} // files
	} // utils
} // anticheat