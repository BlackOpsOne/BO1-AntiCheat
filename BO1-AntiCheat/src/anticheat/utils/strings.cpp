#include "strings.hpp"

#include <chrono>

namespace anticheat {
	namespace utils {
		namespace strings {
			bool CompareConstChar(const char* a, const char* b)
			{
				return std::strcmp(a, b) == 0;
			}

			std::string ToLower(std::string str)
			{
				std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
					{
						return std::tolower(c);
					});
				return str;
			}

			bool Contains(std::string& str, std::string& sub)
			{
				return str.find(sub) != std::string::npos;
			}

			bool ContainsIgnoreCase(const std::string& str, const std::string& sub)
			{
				return ToLower(str).find(ToLower(sub)) != std::string::npos;
			}

			bool StartsWith(const std::string& str, const std::string& prefix)
			{
				if (prefix.size() > str.size())
					return false;
				return std::equal(prefix.begin(), prefix.end(), str.begin());
			}

			std::string GetCurrentEpoch()
			{
				auto now = std::chrono::system_clock::now();
				auto epoch_time = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch();
				return std::to_string(epoch_time.count());
			}

			std::string FormatVector(std::vector<std::string> vec)
			{
				std::string formatted_vec = "";
				for (int i = 0; i < vec.size(); i++)
				{
					formatted_vec += vec[i];
					if (i != vec.size() - 1)
					{
						formatted_vec += ", ";
					}
				}
				return formatted_vec;
			}

			int ToInt(const char* str, bool& success) {
				if (!str) {
					success = false;
					return 0;
				}

				char* end;
				errno = 0;
				long result = std::strtol(str, &end, 10);

				if (errno == ERANGE || result > INT_MAX || result < INT_MIN) {
					success = false;
					return 0;
				}

				if (*end != '\0') {
					success = false;
					return 0;
				}

				success = true;
				return static_cast<int>(result);
			}

			std::wstring ToWideString(const std::string& str)
			{
				int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
				std::wstring wstr(size_needed, 0);
				MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
				return wstr;
			}

			std::string WStringToString(const std::wstring& wstr)
			{
				int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
				std::string str(size_needed - 1, 0); // -1 to remove null terminator from size
				WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
				return str;
			}
		} // strings
	} // utils
} // anticheat