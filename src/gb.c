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
 * @brief Helper function for incrementing an 8-bit register
 * @param gb pointer to gb struct containing registers
 * @param reg 8-bit register to be incremented
 * @return void
 * @note Affects flags: Z N H 
 */
void gbINC_r8(gameBoy_t* gb, uint8_t* reg)
{
	uint8_t result = *reg + 1;

	// Set Z if result is 0
	if(result == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}
	
	// Set H if borrow from bit 3
	if((result & 0x0F) == 0x00)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}

	// Clear N
	gb->generalReg.f &= ~FLAG_REG_SUB;
	*reg = result;
}

/*
 * @brief Helper function for decrementing an 8-bit register
 * @param gb pointer to gb struct containing registers
 * @param reg 8-bit register to be incremented
 * @return void
 * @note Affects flags: Z N H 
 */
void gbDEC_r8(gameBoy_t* gb, uint8_t* reg)
{
	uint8_t result = *reg - 1;

	// Set Z if result is 0
	if(result == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}
	
	// Set if borrow from bit 4
	if((result & 0x0F) == 0x0F)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}

	// Set N
	gb->generalReg.f |= FLAG_REG_SUB;
	*reg = result;
}

/*
 * @brief Helper function for adding a 16-bit value to register HL
 * @param gb pointer to gb struct containing registers
 * @param value 16-bit number to be added to register HL
 * @return void
 * @note Affects flags: N H C
 */
void gbADD_HL_r16(gameBoy_t* gb, uint16_t value)
{
	gb->generalReg.hl += value;

	// Set H if overflow from bit 11
	if(((gb->generalReg.hl & 0xFFF) + (value & 0xFFF)) > 0xFFF)
	{
		gb->generalReg.f |= FLAG_REG_HALF_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
	}

	// Set C if overflow from bit 15
	if(((uint32_t)gb->generalReg.hl + (uint32_t)value) > 0xFFFF)
	{
		gb->generalReg.f |= FLAG_REG_CARRY;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_CARRY;
	}

	// Clear N flag
	gb->generalReg.f &= ~FLAG_REG_SUB;
}

/*
 * @brief Handles invalid or unknown opcodes and prints it
 * @param Pointer to gb struct containing registers
 * @return void
 * @note Shouldn't exectute under normal circumstances
 */
void invalid(gameBoy_t* gb)
{
	printf("Invalid opcode: %#04x\r\n", gb->memory[gb->pc]);
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
	printf("NOP: %#04x\r\n", gb->memory[gb->pc]);
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
	gbINC_r8(gb, &gb->generalReg.b);
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
	gbDEC_r8(gb, &gb->generalReg.b);
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
	uint16_t value = gb->generalReg.bc;
	gbADD_HL_r16(gb, value);
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
	gbINC_r8(gb, &gb->generalReg.c);
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
	gbDEC_r8(gb, &gb->generalReg.c);
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
	printf("STOP: %#04x\r\n", gb->memory[gb->pc]);
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
	gbINC_r8(gb, &gb->generalReg.d);
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
	gbDEC_r8(gb, &gb->generalReg.d);
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
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.d = value;
}

/*
 * @brief Op code function for Load instruction (0x17): RLA
 * @details Rotate register A left, through the carry flag (b7 -> C, C -> b0)
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8 cycles to execute
 * @note Affects (Z)ero, (N)Sub, (H)alf Carry, and (C)arry flags
 */
void opRLA_0x17(gameBoy_t* gb)
{

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
 * @brief Op code function for Relative Jump instruction (0x18): JR r8
 * @details Unconditional relative jump by adding 8-bit offset to PC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 12 cycles to execute
 */
void opJR_0x18(gameBoy_t* gb)
{
	int8_t offset = (int8_t)gb->memory[gb->pc + 1];

	gb->pc += offset;
}

/*
 * @brief Op code function for Add instruction (0x19): ADD HL, DE
 * @details Add the value in register DE to register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 * @note Affects (N)Sub, (H)alf Carry, (C)arry flags
 */
void opADD_0x19(gameBoy_t* gb) 
{
	uint16_t value = gb->generalReg.de;
	gbADD_HL_r16(gb, value);
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
	gbINC_r8(gb, &gb->generalReg.e);
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
 * @brief Op code function for Relative Jump instruction (0x20): JR r8
 * @details If Z flag is clear, jump to 8-bit signed offset 
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8(false) or 12(true) cycles to execute
 */
void opJR_0x20(gameBoy_t* gb)  // JR NZ, e8 
{
	// Memory needs to be casted as int8_t 
	int8_t offset = (int8_t)gb->memory[gb->pc + 1];

	// Jump if flag Z is not set
	if(!(gb->generalReg.f & FLAG_REG_ZERO))
	{
		gb->pc += offset;
		// If true, add 4 cycles to the 8 in table to get 12
		gb->cyclesExtraFlag = true;
	}
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
	uint16_t value = gb->generalReg.a;
	gb->generalReg.hl = value;
	gb->generalReg.hl++;
}

/*
 * @brief Op code function for Increment instruction (0x1C): INC HL
 * @details Increment the value in register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opINC_0x23(gameBoy_t* gb)
{
	gb->generalReg.hl++;
}

/*
 * @brief Op code function for Increment instruction (0x1C): INC H
 * @details Increment the value in register H
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opINC_0x24(gameBoy_t* gb)
{
	gbINC_r8(gb, &gb->generalReg.h);
}

/*
 * @brief Op code function for Decrement instruction (0x1D): DEC H
 * @details Decrement the value in register H
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opDEC_0x25(gameBoy_t* gb)
{
	gbDEC_r8(gb, &gb->generalReg.h);
}

/*
 * @brief Op code function for Decrement instruction (0x1D): DEC HL
 * @details Decrement the value in register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 */
void opDEC_0x2B(gameBoy_t* gb)
{
	gb->generalReg.hl--;
}

/*
 * @brief Op code function for Increment instruction (0x1C): INC L
 * @details Increment the value in register L
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opINC_0x2C(gameBoy_t* gb) 
{
	gbINC_r8(gb, &gb->generalReg.l);
}

/*
 * @brief Op code function for Decrement instruction (0x1D): DEC L
 * @details Decrement the value in register L
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects (Z)ero, (N)Sub, and (H)alf Carry flags
 */
void opDEC_0x2D(gameBoy_t* gb)
{
	gbDEC_r8(gb, &gb->generalReg.l);
}

/*
 * @brief Op code function for Load instruction (0x26): LD H, d8
 * @details Loads 8-bit value to register H
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 byte long and requires 8 cycles to execute
 */
void opLD_0x26(gameBoy_t* gb)  
{
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.h = value;
}

/*
 * @brief Op code function for Decimal Adjust Accumulator instruction (0x27): DAA
 * @details if (N):: if (H): Add $6 to adjust / if (C): Add $60 to adjust. Subtract adjust from register A
	if (!N):: initialize adjust to 0 / if(H || A & $F > $9): add $6 to adjust / if(C || A > $99): add
	$60 to adjust and set C flag/ Add adjust to register A. Binary Coded Decimal
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 * @note Affects flags: Z, H, C
 */
void opDAA_0x27(gameBoy_t* gb)  
{
	uint8_t adjust = 0;

	// Behavior differs based on whether N flag is set or not
	if(gb->generalReg.f & FLAG_REG_SUB)
	{
		if(gb->generalReg.f & FLAG_REG_HALF_CARRY)
		{
			adjust += 0x06;
		}
		if(gb->generalReg.f & FLAG_REG_CARRY)
		{
			adjust += 0x60;
		}
		gb->generalReg.a -= adjust;
	}
	else
	{
		if((gb->generalReg.f & FLAG_REG_HALF_CARRY) || ((gb->generalReg.a & 0x0F) > 0x09))
		{
			adjust += 0x06;
		}
		if((gb->generalReg.f & FLAG_REG_CARRY) || (gb->generalReg.a > 0x99))
		{
			adjust += 0x60;
			gb->generalReg.f |= FLAG_REG_CARRY;
		}
		gb->generalReg.a += adjust;
	}

	// Set Z flag if 0. Clear otherwise
	if(gb->generalReg.a == 0)
	{
		gb->generalReg.f |= FLAG_REG_ZERO;
	}
	else
	{
		gb->generalReg.f &= ~FLAG_REG_ZERO;
	}

	gb->generalReg.f &= ~FLAG_REG_HALF_CARRY;
}

/*
 * @brief Op code function for Relative Jump instruction (0x20): JR r8
 * @details If Z flag is set, jump to 8-bit signed offset 
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8(false) or 12(true) cycles to execute
 */
void opJR_0x28(gameBoy_t* gb) 
{
	// Memory needs to be casted as int8_t 
	int8_t offset = (int8_t)gb->memory[gb->pc + 1];

	// Jump if flag Z is set
	if(gb->generalReg.f & FLAG_REG_ZERO)
	{
		gb->pc += offset;
		// If true, add 4 cycles to the 8 in table to get 12
		gb->cyclesExtraFlag = true;
	}
}

/*
 * @brief Op code function for Add instruction (0x29): ADD HL, HL
 * @details Add the value in register HL to register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 * @note Affects flags: N, H, C
 */
void opADD_0x29(gameBoy_t* gb)
{
	uint16_t value = gb->generalReg.hl;
	gbADD_HL_r16(gb, value);
}

/*
 * @brief Op code function for Load instruction (0x2A): LD A,(HL+)
 * @details Loads value pointed to register HL to register A, then increments HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x2A(gameBoy_t* gb)
{
	uint8_t value = gb->memory[gb->generalReg.hl];
	gb->generalReg.a = value;
	gb->generalReg.hl++;
}

/*
 * @brief Op code function for Load instruction (0x2E): LD L, d8
 * @details Loads 8-bit value to register L 
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 byte long and requires 8 cycles to execute
 */
void opLD_0x2E(gameBoy_t* gb)
{
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.l = value;
}

/*
 * @brief Op code function for Complement instruction (0x2F): CPL
 * @details Complements (Bitwise NOT) operation on accumulator
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 */
void opCPL_0x2F(gameBoy_t* gb)
{
	gb->generalReg.a = ~gb->generalReg.a;
}

/*
 * @brief Op code function for Load instruction (0x31): LD SP, n16
 * @details Copy the value n16 into register SP
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 12 cycles to execute
 */
void opLD_0x31(gameBoy_t* gb) 
{
	uint16_t value = gb->memory[gb->pc + 1];
	gb->sp = value;
}

/*
 * @brief Op code function for Load instruction (0x32): LD (HL-), A
 * @details Loads the value in register A to the memory address pointed to by register HL. HL is then decremented
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x32(gameBoy_t* gb) 
{
	uint8_t value = gb->generalReg.a;
	gb->memory[gb->generalReg.hl] = value;
	gb->generalReg.hl--;
}

/*
 * @brief Op code function for Load instruction (0x36): LD (HL), n8
 * @details Copies value n8 into address pointed to by register HL
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 12 cycles to execute
 */
void opLD_0x36(gameBoy_t* gb) 
{
	uint8_t value = gb->memory[gb->pc + 1];
	gb->memory[gb->generalReg.hl] = value;
}

/*
 * @brief Op code function for Load instruction (0x3A): LD A, (HL-)
 * @details Loads the value pointed to by register HL to register A. HL is then decremented
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 bytes long and requires 8 cycles to execute
 */
void opLD_0x3A(gameBoy_t* gb) 
{
	uint8_t value = gb->memory[gb->generalReg.hl];
	gb->generalReg.a = value;
	gb->generalReg.hl--;
}

/*
 * @brief Op code function for Load instruction (0x3E): LD A, n8
 * @details Copies value n8 into register A
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8 cycles to execute
 */
void opLD_0x3E(gameBoy_t* gb) 
{
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.a = value;
}

/*
 * @brief Consolidated table containing data for each operation supported by the LR35902 processor (Intel 8080 + Zilog Z80)
 * @details { function pointer, cycles required, extra cycles required (for ops w/ variable timing), size in bytes }
 */
struct gbInstruction gbDispatchTable[GB_NUM_OF_OPCODES] =
{
//	{ function,     cycles,  extra,  size } 
	{ opNOP_0x00,   4,       0,      1    },  // NOP
	{ opLD_0x01,    12,      0,	 3    },  // LD BC, d16
	{ opLD_0x02,    8,       0,	 1    },  // LD (BC), A
	{ opINC_0x03,   8,       0,	 1    },  // INC BC 
	{ opINC_0x04,   4,       0,	 1    },  // INC B
	{ opDEC_0x05,   4,       0,	 1    },  // DEC B
	{ opLD_0x06,    8,       0,	 2    },  // LD B, d8
	{ opRLCA_0x07,  4,       0,	 1    },  // RLCA
	{ opLD_0x08,    20,      0,	 3    },  // LD (a16), SP
	{ opADD_0x09,   8,       0,	 1    },  // ADD HL, BC
	{ opLD_0x0A,    8,       0,	 1    },  // LD A, (BC) 
	{ opDEC_0x0B,   8,       0,	 1    },  // DEC BC 
	{ opINC_0x0C,   4,       0,	 1    },  // INC C 
	{ opDEC_0x0D,   4,       0,	 1    },  // DEC C 
	{ opLD_0x0E,    8,       0,	 2    },  // LD C, d8 
	{ opRRCA_0x0F,  4,       0,	 1    },  // RRCA
	{ opSTOP_0x10,  4,       0,	 2    },  // STOP d8
	{ opLD_0x11,    12,      0,	 3    },  // LD DE, d16
	{ opLD_0x12,    8,       0,	 1    },  // LD (DE), A
	{ opINC_0x13,   8,       0,	 1    },  // INC DE
	{ opINC_0x14,   4,       0,	 1    },  // INC D
	{ opDEC_0x15,   4,       0,	 1    },  // DEC D
	{ opLD_0x16,    8,       0,	 2    },  // LD D, d8
	{ opRLA_0x17,   4,       0,	 1    },  // RLA
	{ opJR_0x18,    12,      0,	 2    },  // JR r8
	{ opADD_0x19,   8,       0,	 1    },  // ADD HL, DE
	{ opLD_0x1A,    8,       0,	 1    },  // LD A,(DE)
	{ opDEC_0x1B,   8,       0,	 1    },  // DEC DE
	{ opINC_0x1C,   4,       0,	 1    },  // INC E
	{ opDEC_0x1D,   4,       0,	 1    },  // DEC E
	{ opLD_0x1E,    8,       0,	 2    },  // LD E,d8
	{ opRRA_0x1F,   4,       0,	 1    },  // RRA
	{ opJR_0x20,    8,       4,	 2    },  // JR NZ, e8 
	{ opLD_0x21,    12,      0,	 3    },  // LD HL, d16
	{ opLD_0x22,    8,       0,	 1    },  // LD (HL+), A
	{ opINC_0x23,   8,       0,	 1    },  // INC HL
	{ opINC_0x24,   4,       0,	 1    },  // INC H
	{ opDEC_0x25,   4,       0,	 1    },  // DEC H
	{ opLD_0x26,    8,       0,	 2    },  // LD H, d8
	{ opDAA_0x27,   4,       0,	 1    },  // DAA
	{ opJR_0x28,    8,       4,	 2    },  // JR Z
	{ opADD_0x29,   8,       0,	 1    },  // ADD HL, HL
	{ opLD_0x2A,    8,       0,	 1    },  // LD A, (HL+)
	{ opDEC_0x2B,   8,       0,	 1    },  // DEC HL
	{ opINC_0x2C,   4,       0,	 1    },  // INC L
	{ opDEC_0x2D,   4,       0,	 1    },  // DEC L
	{ opLD_0x2E,    8,       0,	 2    },  // LD L, d8
	{ opCPL_0x2F,   4,       0,	 1    },  // CPL
	{ opLD_0x31,   12,       0,	 3    },  // LD SP, n16
	{ opLD_0x32,    8,       0,	 1    },  // LD (HL-), A
	{ opLD_0x36,   12,       0,	 2    },  // LD (HL), n8 
	{ opLD_0x3A,   	8,       0,	 1    },  // LD A, (HL-) 
	{ opLD_0x3E,    8,       0,	 2    },  // LD A, n8 
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

		// Some operations consume a variable amount of time. Account for that extra time if necessary
		if(gb->cyclesExtraFlag == true)
		{
			gb->cyclesTarget += gbDispatchTable[currentOpCode].clockCyclesExtra;
			gb->cyclesExtraFlag = false;
		}
	}

	gb->cyclesCurrent++;
}
