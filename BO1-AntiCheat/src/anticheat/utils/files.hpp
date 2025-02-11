#include <string>
#include <fstream>
#include <windows.h>
#include <iostream>
#include <filesystem>
#include "md5.h"

#pragma comment (lib, "wintrust")
#pragma comment (lib, "Crypt32")

namespace anticheat {
	namespace utils {
		namespace files {
			std::string GetMD5(std::string file);
			std::string GetAntiCheatPath();
			int SizeInBytes(std::string file);
			bool IsDigitallySigned(const std::string& file_path);
		} // files
	} // utils
} // anticheat