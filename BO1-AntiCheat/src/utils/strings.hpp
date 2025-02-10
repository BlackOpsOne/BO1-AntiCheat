#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace utils {
	namespace strings {
		std::string ToLower(std::string str);
		bool CompareConstChar(const char* a, const char* b);
		bool Contains(std::string& str, std::string& sub);
		bool ContainsIgnoreCase(const std::string& str, const std::string& substring);
		bool StartsWith(const std::string& str, const std::string& prefix);
		std::string GetCurrentEpoch();
		std::string FormatVector(std::vector<std::string> vec);
		int ToInt(const char* str, bool& success);
	} // strings
} // utils