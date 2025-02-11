#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

namespace anticheat {
	namespace internals {
		bool CheckDLL_Integrity();
		bool Inject();
	}
}