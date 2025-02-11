#include <stdio.h>
#include "gb.h"

#define GB_NUM_OF_OPCODES 512

//gb.c: opcodes/registers related to the GameBoy. emu.c: logic related specifically to emulation such as pausing/resuming execution

/*
 * @brief Helper function which returns the op code currently being pointed to by the Program Counter
 */
uint16_t gbGetOpCode(gameBoy_t* gb)
{
	return gb->memory[gb->pc];	
}

/*
 * @brief Handles invalid or unknown opcodes and prints it
 * @param Pointer to gb struct containing registers
 * @return void
 * @note Shouldn't exectute under normal circumstances
 */
void invalid(gameBoy_t* gb)
{
	printf("Invalid opcode: %#04x\r\n");
}

/*
 * @brief Op code function for No Operation instruction (0x00): NOP 
 * @details Performs no work but consumes 4 clock cycles
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 */
void opNOP_0x00(gameBoy_t* gb)
{
	printf("opcode: \r\n");
}

/*
 * @brief Op code function for Load instruction (0x01): LD BC, d16
 * @details Loads 16-bit immediate value into register BC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 12 cycles to execute
 */
void opLD_0x01(gameBoy_t* gb)
{
	printf("LD 0x01 Executed\r\n"); 
	uint16_t value = gb->memory[gb->pc + 1] | (gb->memory[gb->pc + 2] << 8);
	gb->generalReg.bc = value;
}

/*
 * @brief Op code function for Load instruction (0x02): LD (BC), A
 * @details Loads the value from register A to memory address specified by BC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x02(gameBoy_t* gb)
{
	printf("LD 0x02 Executed\r\n"); 
	uint8_t value = gb->generalReg.a;
	gb->memory[gb->generalReg.bc] = value;
}

/*
 * @brief Op code function for Increment instruction (0x03): INC BC
 * @details Increments the value in register BC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opINC_0x03(gameBoy_t* gb)
{
	printf("INC 0x03 Executed\r\n"); 
	gb->generalReg.bc++;
}

/*
 * @brief Op code function for Increment instruction (0x04): INC B
 * @details Increments the value in register B
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opINC_0x04(gameBoy_t* gb)
{
	printf("INC 0x04 Executed\r\n"); 
	gb->generalReg.b++;

	// Set Z if result is 0
	if(gb->generalReg.b == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}
	
	// Incrementing. Clear N
	gb->generalReg.f &= ~FLAG_REG_SUB;

	// Set if borrow from bit 3
	if((gb->generalReg.b & 0x0F) == 0x00)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}
}

/*
 * @brief Op code function for Decrement instruction (0x04): DEC B
 * @details Decrements the value in register B
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opDEC_0x05(gameBoy_t* gb)
{
	printf("DEC 0x05 Executed\r\n"); 
	gb->generalReg.b--;

	// Set Z if result is 0
	if(gb->generalReg.b == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}
	
	// Decrementing. Set N
	gb->generalReg.f |= FLAG_REG_SUB;

	// Set if borrow from bit 4
	if((gb->generalReg.b & 0x0F) == 0x0F)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}
}

/*
 * @brief Op code function for Load instruction (0x06): LD B, d8
 * @details Loads 8-bit immediate value into register B
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8 cycles to execute
 */
void opLD_0x06(gameBoy_t* gb)
{
	printf("LD 0x06 Executed\r\n"); 
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.b = value;
}

/*
 * @brief Op code function for Rotate Left instruction (0x07): RLCA
 * @details Shift bits left in register A by 1. bit 7 is shifted to C flag and bit 0
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, (H)alf Carry, and (C)arry flags
 */
void opRLCA_0x07(gameBoy_t* gb)
{
	uint8_t carry = 0x00;

	// Set C flag if bit 7 is set
	if((gb->generalReg.a & 0x80) == 0x80)
	{
		carry = 0x01;
		gb->generalReg.f |= FLAG_REG_CARRY;
	}
	
	// Shift left by 1
	gb->generalReg.a = (gb->generalReg.a << 1) | carry;

	// Clear the Z, N, and H flags
	gb->generalReg.f &= ~FLAG_REG_ZERO;
	gb->generalReg.f &= ~FLAG_REG_SUB;
	gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
}

/*
 * @brief Op code function for Load instruction (0x08): LD (a16), SP
 * @details Loads the stack pointer into a 16-bit memory address
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 20 cycles to execute
 */
void opLD_0x08(gameBoy_t* gb)
{
	printf("LD 0x08 Executed\r\n"); 
	uint16_t value = gb->sp;
	uint16_t memAddr = gb->memory[gb->pc + 1] | (gb->memory[gb->pc + 2] << 8);
	// memory is uint8_t array, but sp is uint16_t. Store in 8-bit chunks (little-endian)
	gb->memory[memAddr] = value & 0xFF;
	gb->memory[memAddr + 1] = value >> 8;
}

/*
 * @brief Op code function for Load instruction (0x0A): LD A, (BC) 
 * @details Loads the value from a memory address specified by BC to register A
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x0A(gameBoy_t* gb)
{
	printf("LD 0x08 Executed\r\n"); 
	uint8_t value = gb->memory[gb->generalReg.bc];
	gb->generalReg.a = value;
}

/*
 * @brief Op code function for Load instruction (0x0E): LD C, d8 
 * @details Loads the 8-bit value to register C
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 byte long and requires 8 cycles to execute
 */
void opLD_0x0E(gameBoy_t* gb)
{
	printf("LD 0x0E Executed\r\n"); 
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.c = value;
}

/*
 * @brief Op code function for Rotate Right instruction (0x0F): RRCA
 * @details Shift bits right in register A by 1. bit 0 is shifted to C flag and bit 7
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, (H)alf Carry, and (C)arry flags
 */
void opRRCA_0x0F(gameBoy_t* gb)
{
	uint8_t carry = 0x00;

	// Set C flag if bit 0 is set
	if((gb->generalReg.a & 0x01) == 0x01)
	{
		carry = 0x80;
		gb->generalReg.f |= FLAG_REG_CARRY;
	}
	
	// Shift left by 1
	gb->generalReg.a = (gb->generalReg.a >> 1) | carry;

	// Clear the Z, N, and H flags
	gb->generalReg.f &= ~FLAG_REG_ZERO;
	gb->generalReg.f &= ~FLAG_REG_SUB;
	gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
}

/*
 * @brief Op code function for Load instruction (0x11): LD DE, d16
 * @details Loads 16-bit value into register DE
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 12 cycles to execute
 */
void opLD_0x11(gameBoy_t* gb)
{
	printf("LD 0x11 Executed\r\n"); 
	uint16_t value = gb->memory[gb->pc + 1] | gb->memory[gb->pc + 2] << 8;
	gb->generalReg.de = value;
}

/*
 * @brief Op code function for Load instruction (0x12): LD (DE), A
 * @details Loads value from register A to memory address specified by register DE
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x12(gameBoy_t* gb)
{
	printf("LD 0x12 Executed\r\n"); 
	uint8_t value = gb->generalReg.a;
	uint16_t memAddr = gb->generalReg.de;
	gb->memory[memAddr] = value;
}

/*
 * @brief Op code function for Load instruction (0x16): LD D, d8
 * @details Loads 8-bit value into register D
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8 cycles to execute
 */
void opLD_0x16(gameBoy_t* gb)
{
	printf("LD 0x16 Executed\r\n"); 
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.d = value;
}

/*
 * @brief Op code function for Load instruction (0x1A): LD A,(DE)
 * @details Loads value pointed to register DE to register A
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x1A(gameBoy_t* gb)
{
	printf("LD 0x1A Executed\r\n"); 
	uint8_t value = gb->memory[gb->generalReg.de];
	gb->generalReg.a = value;
}

/*
 * @brief Op code function for Load instruction (0x1E): LD E,d8
 * @details Loads 8-bit value into register E
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8 cycles to execute
 */
void opLD_0x1E(gameBoy_t* gb)
{
	printf("LD 0x1E Executed\r\n"); 
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.e = value;
}

/*
 * @brief Op code function for Load instruction (0x21): LD HL, d16
 * @details Loads 16-bit value into register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 12 cycles to execute
 */
void opLD_0x21(gameBoy_t* gb)
{
	printf("LD 0x21 Executed\r\n"); 
	uint16_t value = gb->memory[gb->pc + 1] | gb->memory[gb->pc + 2];
	gb->generalReg.hl = value;
}

/*
 * @brief Op code function for Load instruction (0x22): LD (HL+),A
 * @details Stores the value of register A into register HL. HL is then incremented by 1
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 8 cycles to execute
 */
void opLD_0x22(gameBoy_t* gb)
{
	printf("LD 0x22 Executed\r\n"); 
	uint16_t value = gb->generalReg.a;
	gb->generalReg.hl = value;
	gb->generalReg.hl++;
}

/*
 * @brief Consolidated table containing data for each operation supported by the LR35902 processor (Intel 8080 + Zilog Z80)
 * @details Contains the following data: { function pointer, cycles required, size of op code in bytes }
 */
struct gbInstruction gbDispatchTable[GB_NUM_OF_OPCODES] =
{
//	{ function,     cycles,  size } 
	{ opNOP_0x00,   4,       1    },  // NOP
	{ opLD_0x01,    12,      3    },  // LD BC, d16
	{ opLD_0x02,    8,       1    },  // LD (BC), A
	{ opINC_0x03,   8,       1    },  // INC BC 
	{ opINC_0x04,   4,       1    },  // INC B
	{ opDEC_0x05,   4,       1    },  // DEC B
	{ opLD_0x06,    8,       2    },  // LD B, d8
	{ opRLCA_0x07,  4,       1    },  // RLCA
	{ opLD_0x08,    20,      3    },  // LD (a16), SP
	{ opLD_0x0A,    8,       1    },  // LD A, (BC) 
	{ opLD_0x0E,    8,       2    },  // LD C, d8 
	{ opRRCA_0x0F,  4,       1    },  // RRCA
	{ opLD_0x11,    12,      3    },  // LD DE, d16
	{ opLD_0x12,    8,       1    },  // LD (DE), A
	{ opLD_0x16,    8,       2    },  // LD D, d8
	{ opLD_0x1A,    8,       1    },  // LD A,(DE)
	{ opLD_0x1E,    8,       2    },  // LD E,d8
	{ opLD_0x21,    12,      3    },  // LD HL, d16
	{ opLD_0x22,    8,       1    },  // LD (HL+),A
};

/*
 * @brief Function for handling the emulator's dispatch process (fetching, decoding, executing)
 * @details Uses a function/dispatch table to minimize time complexity by mapping each function pointer's index to their 
	corresponding op code
 * @param Pointer to gb struct containing registers
 * @return void
 */
void gbHandleCycle(gameBoy_t* gb)
{
	uint16_t currentOpCode = 0;
	// If the execution time for the current operation has elapsed, move on to the next
	if(gb->cyclesCurrent == gb->cyclesTarget)
	{
		currentOpCode = gbGetOpCode(gb);
		// Execute operation
		gbDispatchTable[currentOpCode].operation(gb);
		// Different opcodes have different lengths. Increment PC by length of most recent operation in bytes
		gb->pc+= gbDispatchTable[currentOpCode].opCodeSize;
		// Set cyclesTarget so we know when to move on
		gb->cyclesTarget = gb->cyclesCurrent + gbDispatchTable[currentOpCode].clockCycles;
	}

	gb->cyclesCurrent++;
}
