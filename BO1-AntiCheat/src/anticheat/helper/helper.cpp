#include "helper.hpp"

#include <filesystem>

#include "../game/process.hpp"
#include "../game/game.hpp"
#include "../utils/files.hpp"
#include "../checksums.h"
#include "../utils/memory.hpp"
#include "../constants.h"

namespace fs = std::filesystem;

namespace anticheat {
	namespace helper {
		bool CheckHelperIntegrity()
		{
			std::string anticheatDir = utils::files::GetAntiCheatPath();
			std::string helperPath = anticheatDir + "\\" + Constants::HELPER_NAME;

			// check if it exists
			if (!fs::exists(helperPath))
			{
				return false;
			}

			// check if the hash matches
			std::string md5 = utils::files::GetMD5(helperPath);
			if (md5 != Constants::HELPER_MD5)
			{
				return false;
			}

			return true;
		}

		bool InjectHelper()
		{
			HANDLE handle = game::process::GetBlackOpsHandle();
			if (!game::process::IsGameOpen())
			{
				return false;
			}

			std::string anticheatDir = utils::files::GetAntiCheatPath();
			std::string helperPath = anticheatDir + "\\" + Constants::HELPER_NAME;

			if (!utils::memory::InjectDLL(handle, helperPath.c_str()))
			{
				return false;
			}

			return true;
		}
	}
}