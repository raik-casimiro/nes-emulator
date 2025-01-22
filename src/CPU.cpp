#include <iostream>
#include <algorithm>

using Byte = unsigned char;  // 8-bit
using Word = unsigned short; // 16-bit
using UInt32 = unsigned int; // 32-bit

// TODO: Separate Memory into a different file
// TODO: Abstract all LDA cases to functions, they all can be reused in other instructions, like LDA_INDIRECT_Y access its addres exactly like ORA_INDIRECT_Y does
// TODO: Move parts of code from .cpp file to .hpp file
// TODO: Better linting, the code is a mess

struct Memory  {

    static constexpr UInt32 CAPACITY = 1024 * 64; // 64 KB
    Byte Storage[CAPACITY] = {0};

    void clear() {
        std::fill(std::begin(Storage), std::end(Storage), 0);
    }

    Byte operator[](UInt32 address) const {
        return Storage[address];
    }

    Byte& operator[](UInt32 address) {
        return Storage[address];
    }

};

struct CPU {

    Word PC; // Program Counter
    Word SP; // Stack Pointer
    Byte ACC; // Register Accumulator
    Byte RX; // Register X
    Byte RY; // Register Y
    
    Byte FlagC : 1; // Carry
    Byte FlagZ : 1; // Zero
    Byte FlagI : 1; // Interrupt
    Byte FlagD : 1; // Decimal Mode
    Byte FlagB : 1; // Break Command
    Byte FlagO : 1; // Overflow
    Byte FlagN : 1; // Negative

    static constexpr Byte
        LDA_IMMEDIATE = 0xA9,
        LDA_ZERO_PAGE = 0xA5,
        LDA_ZERO_PAGE_X = 0xB5,
        LDA_ABSOLUTE = 0xAD,
        LDA_ABSOLUTE_X = 0xBD,
        LDA_ABSOLUTE_Y = 0xB9,
        LDA_INDIRECT_X = 0xA1,
        LDA_INDIRECT_Y = 0xB1;

    void Reset(Memory& mem) {
        PC = 0xFFFC;
        SP = 0x0100;
        ACC = 0;
        RX = 0;
        RY = 0;
        FlagD = 0;
        FlagI = 0;
        mem.clear();
    }

    Byte Fetch(UInt32& Cycles, Memory& mem) {
        Byte Data = mem[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Byte Access(UInt32& Cycles, Memory& mem, UInt32 address) {
        Byte Data = mem[address];
        Cycles--;
        return Data;
    }

    void LDASetFlags() {
        FlagZ = (ACC == 0);
        FlagN = (ACC & 0b10000000) > 0;
    }

    void Execute(UInt32 Cycles, Memory& mem) {
        while (Cycles > 0) {
            Byte instruction = Fetch(Cycles, mem);

            switch(instruction) {
                case LDA_IMMEDIATE: {
                    Byte value = Fetch(Cycles, mem);
                    ACC = value;
                    LDASetFlags();
                } break;

                case LDA_ZERO_PAGE: {
                    Byte zeroPageAddress = Fetch(Cycles, mem);
                    ACC = Access(Cycles, mem, zeroPageAddress);
                    LDASetFlags();
                } break;

                case LDA_ZERO_PAGE_X: {
                    Byte zeroPageAddress = Fetch(Cycles, mem);
                    zeroPageAddress += RX;
                    ACC = Access(Cycles, mem, zeroPageAddress);
                    LDASetFlags();
                } break;

                case LDA_ABSOLUTE: {
                    Word absoluteAddress = Fetch(Cycles, mem);
                    absoluteAddress |= (Fetch(Cycles, mem) << 8);
                    ACC = Access(Cycles, mem, absoluteAddress);
                    LDASetFlags();
                } break;

                case LDA_ABSOLUTE_X: {
                    Word absoluteAddress = Fetch(Cycles, mem);
                    absoluteAddress |= (Fetch(Cycles, mem) << 8);
                    absoluteAddress += RX;
                    ACC = Access(Cycles, mem, absoluteAddress);
                    LDASetFlags();
                } break;

                case LDA_ABSOLUTE_Y: {
                    Word absoluteAddress = Fetch(Cycles, mem);
                    absoluteAddress |= (Fetch(Cycles, mem) << 8);
                    absoluteAddress += RY;
                    ACC = Access(Cycles, mem, absoluteAddress);
                    LDASetFlags();
                } break;

                case LDA_INDIRECT_X: {
                    Byte zeroPageAddress = Fetch(Cycles, mem);
                    zeroPageAddress += RX;
                    Word effectiveAddress = mem[zeroPageAddress] | (mem[zeroPageAddress + 1] << 8);
                    ACC = Access(Cycles, mem, effectiveAddress);
                    LDASetFlags();
                } break;

                case LDA_INDIRECT_Y: {
                    Byte zeroPageAddress = Fetch(Cycles, mem);
                    Word effectiveAddress = mem[zeroPageAddress] | (mem[zeroPageAddress + 1] << 8);
                    effectiveAddress += RY;
                    ACC = Access(Cycles, mem, effectiveAddress);
                    LDASetFlags();
                } break;

                default: {
                    std::cerr << "Error: unknow instruction (0x" 
                              << std::hex << static_cast<int>(instruction) 
                              << ") at address 0x" << PC - 1 << "\n";
                    Cycles = 0;
                } break;
            }
        }
    }
};

int main() {
    std::cout << "Start\n";

    Memory mem;
    CPU cpu;

    cpu.Reset(mem);

    mem[0xFFFC] = CPU::LDA_ABSOLUTE;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0x42;

    cpu.Execute(4, mem);

    std::cout << "Register ACC: 0x" << std::hex << static_cast<int>(cpu.ACC) << "\n";
    std::cout << "End\n";

    return 0;
}
