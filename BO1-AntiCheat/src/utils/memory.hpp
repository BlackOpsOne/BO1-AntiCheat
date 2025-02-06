#include <windows.h>
#include <vector>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <psapi.h>
#include <iomanip>

namespace utils {
    namespace memory {
        int ReadInt(HANDLE processHandle, int address);
        std::string ReadString(HANDLE processHandle, int address);

        // reads the opcodes of a certain function
        std::vector<BYTE> GetFunctionOpcodes(void* targetAddr, size_t opcodeSize = 5);

        // checks if the opcode bytes match
        bool DoOpcodesMatch(const std::vector<BYTE>& actualOpcodes, const std::vector<BYTE>& expectedOpcodes);
    } // memory
} // utils