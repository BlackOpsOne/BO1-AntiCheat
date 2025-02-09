#include <string>

struct DvarStringEntry
{
	const char* dvar_name;
	const char* expected_value;
};

struct DvarIntEntry
{
	const char* dvar_name;
	unsigned int expected_value;
};

struct DvarBoolEntry
{
	const char* dvar_name;
	bool expected_value;
};

namespace anticheat {
	namespace integrity {
		namespace dvars {
			void InitDvarQueue();
			std::string GetModifiedDvars();
			const char* GetDvarString(const char* dvarName);
			unsigned int GetDvarInt(const char* dvar_name);
			bool GetDvarBool(const char* dvar_name);
		}
	}
}