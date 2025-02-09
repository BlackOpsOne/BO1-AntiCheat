#include "strings.hpp"

#include <chrono>

namespace utils {
	namespace strings {
		bool ConstCharEquals(const char* a, const char* b)
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
	} // strings
} // utils