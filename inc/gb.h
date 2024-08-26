#include <stdint.h>

#define FLAG_REG_ZERO  	    (1 << 7) //
#define FLAG_REG_SUB  	    (1 << 6)
#define FLAG_REG_HALF_CARRY (1 << 5)
#define FLAG_REG_CARRY 	    (1 << 4)

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
	
	// Special Purpose Registers: (F)lags, Program Counter, Stack Pointer
	uint16_t pc;
	uint16_t sp;	
	
	uint8_t opCode;

} gameBoy_t;
