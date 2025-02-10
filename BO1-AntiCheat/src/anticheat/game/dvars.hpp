#include <string>

struct DvarValueEntry
{
	const char* dvar_name;
	const char* expected_value;
};

namespace anticheat {
	namespace game {
		namespace dvars {
			void InitDvarQueue();
			void Cleanup();
			std::string GetModifiedDvars();
			const char* GetDvarString(const char* dvarName);
			const char* GetDvarInt(const char* dvar_name);
			const char* GetDvarBool(const char* dvar_name);
			const char* GetDvarDouble(const char* dvar_name);
			const char* CallHelperFunction(const char* dvar_name, const char* function_name);
		}
	}
}