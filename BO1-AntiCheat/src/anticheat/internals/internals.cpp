#include "internals.hpp"

#include <filesystem>

#include "../game/process.hpp"
#include "../game/game.hpp"
#include "../utils/files.hpp"
#include "../checksums.h"
#include "../utils/memory.hpp"
#include "../constants.h"

namespace fs = std::filesystem;

namespace anticheat {
	namespace internals {
		bool CheckDLL_Integrity()
		{
			std::string anticheat_dir = utils::files::GetAntiCheatPath();
			std::string dll_path = anticheat_dir + "\\" + Constants::INTERNALS_NAME;

			// check if it exists
			if (!fs::exists(dll_path))
			{
				return false;
			}

			// check if the hash matches
			std::string md5 = utils::files::GetMD5(dll_path);
			if (md5 != Constants::INTERNALS_HASH)
			{
				return false;
			}

			return true;
		}

		bool Inject()
		{
			HANDLE handle = game::process::GetBlackOpsHandle();
			if (!game::process::IsGameOpen())
			{
				return false;
			}

			std::string anticheat_dir = utils::files::GetAntiCheatPath();
			std::string dll_path = anticheat_dir + "\\" + Constants::INTERNALS_NAME;

			if (!utils::memory::InjectDLL(handle, dll_path.c_str()))
			{
				return false;
			}

			return true;
		}
	}
}