#ifndef _INSTRUCTION_DECODE_H_
#define _INSTRUCTION_DECODE_H_

#include <StorageObject.h>

namespace z11 {
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

	extern const char *mnemonics[];
}

#define RS(ir) (ir(25, 21))
#define RT(ir) (ir(20, 16))
#define RD(ir) (ir(15, 11))

z11::op decode_instruction(StorageObject &ir);

bool is_special_instruction(StorageObject &ir);

bool is_register_alu_instruction(z11::op instruction);

bool is_immediate_alu_instruction(z11::op instruction);

bool is_load_instruction(z11::op instruction);

bool is_store_instruction(z11::op instruction);

bool is_branch_instruction(z11::op instruction);

#endif // _INSTRUCTION_DECODE_H_
