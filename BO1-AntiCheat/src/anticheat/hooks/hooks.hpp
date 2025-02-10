#include <windows.h>

namespace anticheat {
	namespace hooks {
		bool IsSystemDLL(LPCWSTR module_name);
		bool IsModuleAllowedW(LPCWSTR module_name);
		bool IsModuleAllowedA(LPCSTR module_name);
		bool Initialize();
		void Cleanup();
	}
}