/**
 * Source file for "instruction decode" module that translates instruction
 * register contents into internal instruction representations and provides
 * information about instruction groups.
 *
 * Authors: Coleman Link and Ben Maitland
 */

//local project includes
#include "instruction_decode.h"

namespace z11 {
	//textual representation of instructions
	const char *mnemonics[] = {
		"NOP",
		"J",
		"JAL",
		"BEQ",
		"BNE",
		"ADDI",
		"SLTI",
		"ANDI",
		"ORI",
		"XORI",
		"LUI",
		"LW",
		"SW",
		"HALT",
		"JR",
		"JALR",
		"BREAK",
		"ADD",
		"SUB",
		"AND",
		"OR",
		"XOR",
		"SLT",
		"SLTU",
		"SLL",
		"SRL",
		"SRA",
		"SLLV",
		"SRLV",
		"SRAV",
		"???????",
		"ADDIU",
		"SLTIU",
		"LB",
		"LH",
		"LBU",
		"LHU",
		"SB",
		"SH",
		"BLTZ",
		"BLTZAL",
		"BGEZ",
		"BGEZAL",
		"BLEZ",
		"BGTZ",
		"SYSCALL",
		"ADDU",
		"SUBU",
		"NOR"
	};
}


/**
 * Helper function for decoding the 'funct' field of "special" instructions to
 * get their exact instruction type.
 *
 * @param ir The instruction register to read the contents of the 'funct'
 *	field from.
 * @returns An enum value representing the instruction that was in the
 *      specified IR.
 */
z11::op decode_special_instruction(StorageObject &ir);


z11::op decode_instruction(StorageObject &ir) {
	switch(ir(31, 26)) {
		case 0:
			return decode_special_instruction(ir);
		case 1: //NOP
			return z11::NOP;
		case 2: //J
			return z11::J;
		case 3: //JAL
			return z11::JAL;
		case 16: //ADDI
			return z11::ADDI;
		case 17: //ADDIU
			return z11::ADDIU;
		case 20: //ANDI
			return z11::ANDI;
		case 21: //ORI
			return z11::ORI;
		case 22: //XORI
			return z11::XORI;
		case 24: //SLTI
			return z11::SLTI;
		case 25: //SLTIU
			return z11::SLTIU;
		case 32: //LB
			return z11::LB;
		case 33: //LH
			return z11::LH;
		case 35: //LW
			return z11::LW;
		case 36: //LBU
			return z11::LBU;
		case 37: //LHU
			return z11::LHU;
		case 39: //LUI
			return z11::LUI;
		case 40: //SB
			return z11::SB;
		case 41: //SH
			return z11::SH;
		case 43: //SW
			return z11::SW;
		case 50: //BLTZ
			return z11::BLTZ;
		case 51: //BLTZAL
			return z11::BLTZAL;
		case 58: //BGEZ
			return z11::BGEZ;
		case 59: //BGEZAL
			return z11::BGEZAL;
		case 60: //BEQ
			return z11::BEQ;
		case 61: //BNE
			return z11::BNE;
		case 62: //BLEZ
			return z11::BLEZ;
		case 63: //BGTZ
			return z11::BGTZ;
		default:
			return z11::UNKNOWN;
	}
}

z11::op decode_special_instruction(StorageObject &ir) {
	switch(ir(5, 0)) {
		case 0: //HALT
			return z11::HALT;
		case 2: //JR
			return z11::JR;
		case 3: //JALR
			return z11::JALR;
		case 6: //SYSCALL
			return z11::SYSCALL;
		case 7: //BREAK
			return z11::BREAK;
		case 16: //ADD
			return z11::ADD;
		case 17: //ADDU
			return z11::ADDU;
		case 18: //SUB
			return z11::SUB;
		case 19: //SUBU
			return z11::SUBU;
		case 20: //AND
			return z11::AND;
		case 21: //OR
			return z11::OR;
		case 22: //XOR
			return z11::XOR;
		case 23: //NOR
			return z11::NOR;
		case 24: //SLT
			return z11::SLT;
		case 25: //SLTU
			return z11::SLTU;
		case 37: //SLL
			return z11::SLL;
		case 38: //SRL
			return z11::SRL;
		case 39: //SRA
			return z11::SRA;
		case 45: //SLLV
			return z11::SLLV;
		case 46: //SRLV
			return z11::SRLV;
		case 47: //SRAV
			return z11::SRAV;
		default:
			return z11::UNKNOWN;
	}
}


bool is_special_instruction(StorageObject &ir) {
	return (ir(31, 26) == 0);
}


bool is_register_alu_instruction(z11::op instruction) {
	switch(instruction) {
		case z11::ADD:
		case z11::SUB:
		case z11::AND:
		case z11::OR:
		case z11::XOR:
		/* shifts count as R-R ALU ops based on tests developed to see
			when the example solution does and doesn't do
			forwarding. Also, shifts read 'rt' and write 'rd',
			like R-R ALU instruction do. */
		case z11::SLT:
		case z11::SLTU:
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			return true;
	}

	return false;
}

bool is_immediate_alu_instruction(z11::op instruction) {
	switch(instruction) {
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
			return true;
	}

	return false;
}

bool is_load_instruction(z11::op instruction) {
	return (instruction == z11::LW);
}

bool is_store_instruction(z11::op instruction) {
	return (instruction == z11::SW);
}

bool is_branch_instruction(z11::op instruction) {
	return ((instruction == z11::BEQ) || (instruction == z11::BNE));
}

bool is_jump_register_instruction(z11::op instruction) {
	return ((instruction == z11::JR) || (instruction == z11::JALR));
}
