#include <stdint.h>
#include <stdbool.h>

#define FLAG_REG_ZERO  	    (1 << 7)
#define FLAG_REG_SUB  	    (1 << 6)
#define FLAG_REG_HALF_CARRY (1 << 5)
#define FLAG_REG_CARRY 	    (1 << 4)

#define GB_MEMORY_SIZE 	    0x10000

#ifndef GB_H
#define GB_H

// GameBoy bootROM starts at memory address 0x0
// Cartridge ROM loaded into memory address 0x0100

// CPU specification for the GameBoy 
// CPU is Sharp LR35902 custom chip (Z80-like CPU, similar to Intel 8080)

// Struct for general purpose registers
typedef struct
{
	// Union for the A and F registers
	union
	{
		struct
		{
			uint8_t a; // Accumulator
			uint8_t f; // Flags reg. Note: Lower nibble is always 0
		};
		uint16_t af;
	};
	// Union for the B and C registers
	union
	{
		struct
		{
			uint8_t b;
			uint8_t c;
		};
		uint16_t bc;
	};
	// Union for the D and E registers
	union
	{
		struct
		{
			uint8_t d;
			uint8_t e;
		};
		uint16_t de;
	};
	// Union for the H and L registers
	union
	{
		struct
		{
			uint8_t h;
			uint8_t l;
		};
		uint16_t hl;
	};
} cpuReg_t;

typedef struct 
{
	// 8-bit General-Purpose Registers: (A)ccumulator, B, C, D, E, H, L
	// Can be paired for 16-bit operations: (AF, BC, DE, HL)
	cpuReg_t generalReg;
	
	bool cyclesExtraFlag;
	// Increments by 1
	uint64_t cyclesCurrent;
	// Will be used to ensure we maintain proper instruction timing
	uint64_t cyclesTarget;
	// Special Purpose Registers: (F)lags, Program Counter, Stack Pointer
	uint16_t pc;
	uint16_t sp;	
	// The Gameboy has 64KB of addressable memory (65535 bytes)
	uint8_t memory [GB_MEMORY_SIZE];
	// 
} gameBoy_t;

// Define generalized opcode function data type which will be used to take one of many opcode functions at a time
// Each function in function table needs to have the same function signature, but different opcodes interact with
// different registers. Best way to handle this is to pass a pointer to the entire CPU
typedef void gbOpCode (gameBoy_t* gb);

typedef struct gbInstruction
{
	gbOpCode* operation;
	uint8_t clockCycles;
	uint8_t clockCyclesExtra;
	uint8_t opCodeSize;
} gbInstruction;

void gbHandleCycle(gameBoy_t* gb);

#endif // GB_H

