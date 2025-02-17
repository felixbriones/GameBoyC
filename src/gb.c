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
	printf("RLCA 0x07 Executed\r\n"); 
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
 * @brief Op code function for Add instruction (0x09): ADD HL, BC
 * @details Adds the value of register BC to register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 bytes long and requires 8 cycles to execute
 * @note Affects (N)Sub, (H)alf Carry, and (C)arry flags
 */
void opADD_0x09(gameBoy_t* gb)
{
	printf("ADD 0x09 Executed\r\n"); 
	uint16_t value = gb->generalReg.bc;

	// Set H if overflow from bit 11
	if((gb->generalReg.hl & 0xFFF) + (value & 0xFFF) > 0xFFF)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}

	// Set C if overflow from bit 15
	if((uint32_t)gb->generalReg.hl + (uint32_t)value > 0xFFFF)
	{
		gb->generalReg.f |= FLAG_REG_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_CARRY;
	}

	gb->generalReg.hl += value;	

	// Set N flag to 0
	gb->generalReg.f &= ~FLAG_REG_SUB;
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
	printf("LD 0x0A Executed\r\n"); 
	uint8_t value = gb->memory[gb->generalReg.bc];
	gb->generalReg.a = value;
}

/*
 * @brief Op code function for Decrement instruction (0x0B): DEC BC
 * @details Decrements the value in register BC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opDEC_0x0B(gameBoy_t* gb)
{
	printf("DEC 0x0B Executed\r\n"); 
	gb->generalReg.b--;
}

/*
 * @brief Op code function for Increment instruction (0x0C): INC C
 * @details Increments the value in register C
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, (H)alf Carry flags
 */
void opINC_0x0C(gameBoy_t* gb)
{
	printf("INC 0x0C Executed\r\n"); 
	gb->generalReg.c++;

	// Set if overflow from bit 3
	if((gb->generalReg.c & 0x0F) == 0x00)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;		
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;		
	}
	
	// Set if result equals 0
	if(gb->generalReg.c == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;		
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;		
	}

	// Clear flag
	gb->generalReg.f &= ~FLAG_REG_SUB;		
}

/*
 * @brief Op code function for Decrement instruction (0x0D): DEC C
 * @details Increments the value in register C
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, (H)alf Carry flags
 */
void opDEC_0x0D(gameBoy_t* gb)
{
	printf("DEC 0x0D Executed\r\n"); 

	gb->generalReg.c--;
	
	// Set if borrow from bit 4
	if((gb->generalReg.c & 0x0F) == 0x0F)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;		
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;		
	}
	
	// Set if result equals 0
	if(gb->generalReg.c == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;		
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;		
	}

	// Set flag
	gb->generalReg.f |= FLAG_REG_SUB;
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

// TODO: Research and implement STOP functionality 
void opSTOP_0x10(gameBoy_t* gb)
{
	printf("STOP 0x10 Executed\r\n"); 
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
 * @brief Op code function for Increment instruction (0x13): INC DE
 * @details Increments the value in register DE
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opINC_0x13(gameBoy_t* gb)
{
	printf("INC 0x13 Executed\r\n"); 
	gb->generalReg.de++;	
}

/*
 * @brief Op code function for Increment instruction (0x14): INC D
 * @details Increments the value in register D
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opINC_0x14(gameBoy_t* gb)
{
	printf("INC 0x14 Executed\r\n"); 
	
	gb->generalReg.d++;

	// Set Z flag if result is 0
	if(gb->generalReg.d == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}

	// Set H flag if overflow from bit 3
	if((gb->generalReg.d & 0x0F) == 0x00)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}

	// Clear N flag
	gb->generalReg.f &= ~FLAG_REG_SUB;
}

/*
 * @brief Op code function for Decrement instruction (0x15): DEC D
 * @details Decrements the value in register D
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opDEC_0x15(gameBoy_t* gb)
{
	printf("DEC 0x15 Executed\r\n"); 
	
	gb->generalReg.d--;

	// Set Z flag if result is 0
	if(gb->generalReg.d == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}

	// Set H flag if borrow from bit 4
	if((gb->generalReg.d & 0x0F) == 0x0F)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}

	// Clear N flag
	gb->generalReg.f |= FLAG_REG_SUB;
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

// Rotate register A left, through the carry flag (b7 -> C, C -> b0)
void opRLA_0x17(gameBoy_t* gb)
{
	printf("RLA 0x17 Executed\r\n"); 

	uint8_t carry = 0x00;

	// Save the carry to be moved to bit 0
	carry = (gb->generalReg.f & FLAG_REG_CARRY) ? 1 : 0;

	// Shift bit 7 to carry register
	if((gb->generalReg.a & 0x80) == 0x80)
	{
		gb->generalReg.f |= FLAG_REG_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_CARRY;
	}	

	// Shift register A left 1
	gb->generalReg.a = (gb->generalReg.a << 1) | carry;

	gb->generalReg.f &= ~FLAG_REG_ZERO;
	gb->generalReg.f &= ~FLAG_REG_SUB;
	gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
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
 * @brief Op code function for Decrement instruction (0x1B): DEC DE
 * @details Decrements the value in register DE
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opDEC_0x1B(gameBoy_t* gb)  // DEC DE
{
	printf("DEC 0x1B Executed\r\n"); 
	gb->generalReg.de--;
}

/*
 * @brief Op code function for Increment instruction (0x1C): INC E
 * @details Increment the value in register E
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opINC_0x1C(gameBoy_t* gb)
{
	printf("INC 0x1C Executed\r\n"); 
	gb->generalReg.e++;
	
	// Set Z if result is 0
	if(gb->generalReg.e == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}

	// Set H if overflow from bit 3
	if((gb->generalReg.e & 0xF) == 0x0)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}
	
	// Clear the register
	gb->generalReg.f &= ~FLAG_REG_SUB;
}

/*
 * @brief Op code function for Decrement instruction (0x1D): DEC E
 * @details Decrement the value in register E
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opDEC_0x1D(gameBoy_t* gb)
{
	printf("DEC 0x1D Executed\r\n"); 
	gb->generalReg.e--;

	// Set Z if result is 0
	if(gb->generalReg.e == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}

	// Set H if borrow from bit 4
	if((gb->generalReg.e & 0xF) == 0xF)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}
	
	// Set the flag
	gb->generalReg.f |= FLAG_REG_SUB;
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

// 
/*
 * @brief Op code function for Rotate Right instruction (0x1F): RRA
 * @details Rotate register A right, through the carry flag ( C -> b7, b0 -> C)
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 */
void opRRA_0x1F(gameBoy_t* gb)
{
	printf("RRA 0x1F Executed\r\n"); 

	uint8_t carry = 0x00;
	
	// Save the old carry
	carry = (gb->generalReg.f & FLAG_REG_CARRY) ? 0x80 : 0x00;

	// Move bit 0 to carry
	if((gb->generalReg.a & 0x01) == 0x01)
	{
		gb->generalReg.f |= FLAG_REG_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_CARRY;
	}
	
	// Shift bits right	
	gb->generalReg.a = (gb->generalReg.a >> 1) | carry;

	gb->generalReg.f &= ~FLAG_REG_ZERO;
	gb->generalReg.f &= ~FLAG_REG_SUB;
	gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
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
	{ opADD_0x09,   8,       1    },  // ADD HL, BC
	{ opLD_0x0A,    8,       1    },  // LD A, (BC) 
	{ opDEC_0x0B,   8,       1    },  // DEC BC 
	{ opINC_0x0C,   4,       1    },  // INC C 
	{ opDEC_0x0D,   4,       1    },  // DEC C 
	{ opLD_0x0E,    8,       2    },  // LD C, d8 
	{ opRRCA_0x0F,  4,       1    },  // RRCA
	{ opSTOP_0x10,  4,       2    },  // STOP d8
	{ opLD_0x11,    12,      3    },  // LD DE, d16
	{ opLD_0x12,    8,       1    },  // LD (DE), A
	{ opINC_0x13,   8,       1    },  // INC DE
	{ opINC_0x14,   4,       1    },  // INC D
	{ opDEC_0x15,   4,       1    },  // DEC D
	{ opLD_0x16,    8,       2    },  // LD D, d8
	{ opRLA_0x17,   4,       1    },  // RLA
	{ opLD_0x1A,    8,       1    },  // LD A,(DE)
	{ opDEC_0x1B,   8,       1    },  // DEC DE
	{ opINC_0x1C,   4,       1    },  // INC E
	{ opDEC_0x1D,   4,       1    },  // DEC E
	{ opLD_0x1E,    8,       2    },  // LD E,d8
	{ opRRA_0x1F,   4,       1    },  // RRA
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
