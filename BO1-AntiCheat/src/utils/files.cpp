#include "files.hpp"

using namespace std;

namespace utils {
	namespace files {
		std::string GetMD5(std::string file)
		{
			ifstream inFile(file, ios::binary);

			if (!inFile) {
				return "";
			}

			inFile.seekg(0, ios::end);
			long length = inFile.tellg();
			inFile.seekg(0, ios::beg);

			if (length <= 0) {
				return "";
			}

			vector<char> fileData(length);
			inFile.read(fileData.data(), length);

			if (!inFile) {
				return "";
			}

			string hash = md5(fileData.data(), length);
			return hash.c_str();
		}

		std::string GetAntiCheatPath()
		{
			char path[MAX_PATH];
			if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0)
			{
				return "";
			}

			std::string fullPath(path);
			size_t lastSlash = fullPath.find_last_of("\\/");
			if (lastSlash == std::string::npos)
			{
				return "";
			}

			return fullPath.substr(0, lastSlash + 1);
		}

		int SizeInBytes(std::string filePath)
		{
			std::ifstream file(filePath, std::ios::binary | std::ios::ate);
			if (!file.is_open()) {
				return -1;
			}
			return file.tellg();
		}
	}
}