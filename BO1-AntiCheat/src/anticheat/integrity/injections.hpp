#include <windows.h>

namespace anticheat {
	namespace integrity {
		namespace injections {
			bool IsSystemDLL(LPCWSTR module_name);
			bool IsModuleAllowedW(LPCWSTR module_name);
			bool IsModuleAllowedA(LPCSTR module_name);
			bool Initialize();
			void Cleanup();
		} // injections
	} // integrity
} // anticheat