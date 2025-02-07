#include "memory.hpp"

#include "../game/process.hpp"

#include "../constants.h"

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

		bool InjectDLL(HANDLE process, const char* dll_path)
		{
			LPVOID pDllPath = VirtualAllocEx(process, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
			if (!pDllPath)
			{
				return false;
			}

			if (!WriteProcessMemory(process, pDllPath, (LPVOID)dll_path, strlen(dll_path) + 1, NULL))
			{
				VirtualFreeEx(process, pDllPath, 0, MEM_RELEASE);
				return false;
			}

			HANDLE hThread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllPath, 0, NULL);
			if (!hThread)
			{
				VirtualFreeEx(process, pDllPath, 0, MEM_RELEASE);
				return false;
			}

			WaitForSingleObject(hThread, INFINITE);

			VirtualFreeEx(process, pDllPath, 0, MEM_RELEASE);
			CloseHandle(hThread);

			return true;
		}

		FARPROC GetRemoteProcAddress(HANDLE hProcess, HMODULE hModule, const char* functionName)
		{
			if (!hModule)
				return nullptr;

			// Read the IMAGE_DOS_HEADER
			IMAGE_DOS_HEADER dosHeader;
			if (!ReadProcessMemory(hProcess, (LPCVOID)hModule, &dosHeader, sizeof(IMAGE_DOS_HEADER), nullptr) ||
				dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
			{
				return nullptr;
			}

			// Read the IMAGE_NT_HEADERS
			IMAGE_NT_HEADERS ntHeaders;
			if (!ReadProcessMemory(hProcess, (LPCVOID)((BYTE*)hModule + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS), nullptr) ||
				ntHeaders.Signature != IMAGE_NT_SIGNATURE)
			{
				return nullptr;
			}

			// Read the IMAGE_EXPORT_DIRECTORY
			IMAGE_DATA_DIRECTORY exportDataDir = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			IMAGE_EXPORT_DIRECTORY exportDirectory;
			if (!ReadProcessMemory(hProcess, (LPCVOID)((BYTE*)hModule + exportDataDir.VirtualAddress), &exportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY), nullptr))
			{
				return nullptr;
			}

			// Read function names and addresses
			DWORD* namePointers = new DWORD[exportDirectory.NumberOfNames];
			WORD* ordinalTable = new WORD[exportDirectory.NumberOfNames];
			DWORD* functionPointers = new DWORD[exportDirectory.NumberOfFunctions];

			ReadProcessMemory(hProcess, (LPCVOID)((BYTE*)hModule + exportDirectory.AddressOfNames), namePointers, exportDirectory.NumberOfNames * sizeof(DWORD), nullptr);
			ReadProcessMemory(hProcess, (LPCVOID)((BYTE*)hModule + exportDirectory.AddressOfNameOrdinals), ordinalTable, exportDirectory.NumberOfNames * sizeof(WORD), nullptr);
			ReadProcessMemory(hProcess, (LPCVOID)((BYTE*)hModule + exportDirectory.AddressOfFunctions), functionPointers, exportDirectory.NumberOfFunctions * sizeof(DWORD), nullptr);

			// Iterate through function names to find the matching one
			FARPROC remoteFunctionAddress = nullptr;
			for (DWORD i = 0; i < exportDirectory.NumberOfNames; i++)
			{
				char functionNameBuffer[256] = { 0 };
				ReadProcessMemory(hProcess, (LPCVOID)((BYTE*)hModule + namePointers[i]), functionNameBuffer, sizeof(functionNameBuffer), nullptr);

				if (_stricmp(functionName, functionNameBuffer) == 0)
				{
					WORD functionOrdinal = ordinalTable[i];
					DWORD functionRVA = functionPointers[functionOrdinal];
					remoteFunctionAddress = (FARPROC)((BYTE*)hModule + functionRVA);
					break;
				}
			}

			// Cleanup
			delete[] namePointers;
			delete[] ordinalTable;
			delete[] functionPointers;

			return remoteFunctionAddress;
		}

		HMODULE GetRemoteModuleHandle(HANDLE hProcess, const char* moduleName)
		{
			HMODULE hMods[1024];
			DWORD cbNeeded;

			// Get a list of all the modules in the remote process
			if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
				{
					char modName[MAX_PATH];
					if (GetModuleBaseNameA(hProcess, hMods[i], modName, sizeof(modName) / sizeof(char)))
					{
						if (_stricmp(modName, moduleName) == 0)
						{
							return hMods[i]; // Return the module handle (base address)
						}
					}
				}
			}
			return nullptr; // Module not found
		}
	} // memory
} // utils