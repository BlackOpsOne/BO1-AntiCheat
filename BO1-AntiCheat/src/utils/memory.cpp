#include "memory.hpp"

#include "../game/process.hpp"

using namespace std;

namespace utils {
	namespace memory {
		int ReadInt(HANDLE processHandle, int address) {
			if (address == -1 || processHandle == nullptr) {
				return -1;
			}

			int buffer = 0;
			SIZE_T bytesRead = 0;
			bool success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead);

			if (!success || bytesRead != sizeof(buffer)) {
				return -1;
			}

			return buffer;
		}

		std::string ReadString(HANDLE processHandle, int address) {
			if (address == -1 || processHandle == nullptr) {
				return "";
			}

			char buffer = 0;
			string result;
			SIZE_T bytesRead = 0;

			// Read string byte-by-byte until null terminator
			while (true) {
				bool success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead);
				if (!success || bytesRead != sizeof(buffer) || buffer == 0) {
					break;
				}

				result.push_back(buffer);
				address++;
			}

			return result;
		}

		std::vector<BYTE> GetFunctionOpcodes(void* targetAddr, size_t opcodeSize) {
			HANDLE handle = game::process::GetBlackOpsProcess();
			std::vector<BYTE> buffer(opcodeSize);
			SIZE_T bytesRead;

			std::vector<BYTE> opcodes;

			if (!game::process::IsGameOpen()) {
				return opcodes;
			}

			try {
				if (ReadProcessMemory(handle, targetAddr, buffer.data(), buffer.size(), &bytesRead) && bytesRead == buffer.size()) {
					if (!game::process::IsGameOpen()) {
						return opcodes;
					}

					opcodes = std::move(buffer);
				}
			}
			catch (const std::exception&) {
				return opcodes;
			}

			return opcodes;
		}

		bool DoOpcodesMatch(const std::vector<BYTE>& actualOpcodes, const std::vector<BYTE>& expectedOpcodes) {
			// check size
			if (actualOpcodes.size() != expectedOpcodes.size()) {
				// they may try to close the game during this
				if (!game::process::IsGameOpen())
				{
					return true;
				}
				return false;
			}

			// compare the bytes
			for (size_t i = 0; i < actualOpcodes.size(); ++i) {
				if (actualOpcodes[i] != expectedOpcodes[i]) {
					// they may try to close the game during this
					if (!game::process::IsGameOpen())
					{
						return true;
					}
					return false; // did not match
				}
			}

			return true; // all matched
		}
	} // memory
} // utils