#include <windows.h>
#include <string>

namespace anticheat {
	namespace game {
		namespace process {
			HANDLE GetBlackOpsHandle();
			bool IsGameOpen();
			std::string GetPathToExe();
			DWORD GetProcessIdByName(const std::wstring& procName);
			HMODULE GetHelperModule();
			void Cleanup();
		} // process
	} // game
} // anticheat