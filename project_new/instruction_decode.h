#ifndef _INSTRUCTION_DECODE_H_
#define _INSTRUCTION_DECODE_H_

#include <StorageObject.h>

namespace z11 {
	enum op {
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
		UNKNOWN = 30
	};

	extern const char *mnemonics[];
}

z11::op decode_instruction(StorageObject &ir);

bool is_special_instruction(z11::op instruction);

#endif // _INSTRUCTION_DECODE_H_
