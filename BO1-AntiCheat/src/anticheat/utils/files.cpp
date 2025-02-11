#include "files.hpp"

#include <tchar.h>
#include <windows.h>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <mscat.h>

#include "../utils/strings.hpp"

namespace anticheat {
	namespace utils {
		namespace files {
			std::string GetMD5(std::string file)
			{
				std::ifstream inFile(file, std::ios::binary);

				if (!inFile) {
					return "";
				}

				inFile.seekg(0, std::ios::end);
				long length = inFile.tellg();
				inFile.seekg(0, std::ios::beg);

				if (length <= 0) {
					return "";
				}

				std::vector<char> fileData(length);
				inFile.read(fileData.data(), length);

				if (!inFile) {
					return "";
				}

				std::string hash = md5(fileData.data(), length);
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

			bool IsDigitallySigned(const std::string& file_path)
			{
				WINTRUST_FILE_INFO wintrust_file_info = {};
				wintrust_file_info.cbStruct = sizeof(WINTRUST_FILE_INFO);
				wintrust_file_info.pcwszFilePath = utils::strings::ToWideString(file_path).c_str();

				// set the wintrust data
				WINTRUST_DATA wintrust_data = {};
				wintrust_data.cbStruct = sizeof(WINTRUST_DATA);
				wintrust_data.dwUIChoice = WTD_UI_NONE;
				wintrust_data.fdwRevocationChecks = WTD_REVOKE_NONE;
				wintrust_data.dwUnionChoice = WTD_CHOICE_FILE;
				wintrust_data.pFile = &wintrust_file_info;
				wintrust_data.dwStateAction = WTD_STATEACTION_VERIFY;

				// get the action id and status
				GUID action_id = WINTRUST_ACTION_GENERIC_VERIFY_V2;
				long status = WinVerifyTrust(NULL, &action_id, &wintrust_data);

				wintrust_data.dwStateAction = WTD_STATEACTION_CLOSE;
				WinVerifyTrust(NULL, &action_id, &wintrust_data);

				// return if it was a success
				return (status == ERROR_SUCCESS);
			}
		} // files
	} // utils
} // anticheat