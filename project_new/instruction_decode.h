/**
 * Header file for "instruction decode" module that translates instruction
 * register contents into internal instruction representations and provides
 * information about instruction groups.
 *
 * Authors: Coleman Link and Ben Maitland
 */

#ifndef _INSTRUCTION_DECODE_H_
#define _INSTRUCTION_DECODE_H_

//arch library imports
#include <StorageObject.h>

/* enum is namespaced so as not to pollute global space with names of every
	instruction */
namespace z11 {
	/* enum representing every z88 instruction. Values in this enum are
		used as indexes into an array of instruction mnemonics for
		printing instructions */
	enum op {
		//valid and implemented instructions
		NOP = 0,
		J = 1,
		JAL = 2,
		BEQ = 3,
		BNE = 4,
		ADDI = 5,
		SLTI = 6,
		ANDI = 7,
		ORI = 8,
		XORI = 9,
		LUI = 10,
		LW = 11,
		SW = 12,
		HALT = 13,
		JR = 14,
		JALR = 15,
		BREAK = 16,
		ADD = 17,
		SUB = 18,
		AND = 19,
		OR = 20,
		XOR = 21,
		SLT = 22,
		SLTU = 23,
		SLL = 24,
		SRL = 25,
		SRA = 26,
		SLLV = 27,
		SRLV = 28,
		SRAV = 29,
		//instruction for invalid opcodes
		UNKNOWN = 30,
		//unimplemented instructions
		ADDIU = 31,
		SLTIU = 32,
		LB = 33,
		LH = 34,
		LBU = 35,
		LHU = 36,
		SB = 37,
		SH = 38,
		BLTZ = 39,
		BLTZAL = 40,
		BGEZ = 41,
		BGEZAL = 42,
		BLEZ = 43,
		BGTZ = 44,
		SYSCALL = 45,
		ADDU = 46,
		SUBU = 47,
		NOR = 48
	};

	//list of strings containing the names of instructions
	extern const char *mnemonics[];
}

//macros for extracting fields from the contents of an instruction register
#define RS(ir) (ir(25, 21))
#define RT(ir) (ir(20, 16))
#define RD(ir) (ir(15, 11))

/**
 * Translate the instruction stored in the specified instruction register into
 * the CPU's internal enum-based representation.
 *
 * @param ir The instruction register to read the instruction from.
 * @returns An enum value representing the instruction that was in the
 *	specified IR.
 */
z11::op decode_instruction(StorageObject &ir);

/**
 * Determine whether the instruction stored in the specified instruction
 * register is a "special" instruction that uses the 'funct' field to
 * determine its exact instruction type. This function assumes that the
 * instruction stored in the specified register is valid.
 *
 * @param ir The instruction register to read the instruction from.
 * @returns True if the instruction is a "special" instruction, false
 *	otherwise.
 */
bool is_special_instruction(StorageObject &ir);

/**
 * Determine if the specified instruction is a register-to-register ALU
 * instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a R-R ALU instruction,
 *	false otherwise.
 */
bool is_register_alu_instruction(z11::op instruction);

/**
 * Determine if the specified instruction is an ALU immediate instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is an ALU immediate instruction,
 *	false otherwise.
 */
bool is_immediate_alu_instruction(z11::op instruction);

/**
 * Determine if the specified instruction is a load instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a load instruction,
 *	false otherwise.
 */
bool is_load_instruction(z11::op instruction);

/**
 * Determine if the specified instruction is a store instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a store instruction,
 *	false otherwise.
 */
bool is_store_instruction(z11::op instruction);

/**
 * Determine if the specified instruction is a branch instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a branch instruction,
 *	false otherwise.
 */
bool is_branch_instruction(z11::op instruction);

/**
 * Determine if the specified instruction if a jump instruction that used a
 * register operand.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a JR or JALR instruction,
 *	false otherwise.
 */
bool is_jump_register_instruction(z11::op instruction);

/**
 * Determine if the specified instruction is a shift variable instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a SLLV, SRLV, or SRAV
 *	instruction, false otherwise.
 */
bool is_variable_shift_instruction(z11::op instruction);

/**
 * Determine if the specified instruction is a set if less than instruction.
 *
 * @param instruction The instruction to test.
 * @returns True if the specified instruction is a SLTI, SLT, or SLTU
 *	instruction, false otherwise.
 */
bool is_set_if_less_than_instruction(z11::op instruction);

#endif // _INSTRUCTION_DECODE_H_
