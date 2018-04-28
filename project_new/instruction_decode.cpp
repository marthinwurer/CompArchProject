#include "instruction_decode.h"

namespace z11 {
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
		"???????"
	};
}


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
		case 20: //ANDI
			return z11::ANDI;
		case 21: //ORI
			return z11::ORI;
		case 22: //XORI
			return z11::XORI;
		case 24: //SLTI
			return z11::SLTI;
		case 35: //LW
			return z11::LW;
		case 39: //LUI
			return z11::LUI;
		case 43: //SW
			return z11::SW;
		case 60: //BEQ
			return z11::BEQ;
		case 61: //BNE
			return z11::BNE;
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
		case 7: //BREAL
			return z11::BREAK;
		case 16: //ADD
			return z11::ADD;
		case 18: //SUB
			return z11::SUB;
		case 20: //AND
			return z11::AND;
		case 21: //OR
			return z11::OR;
		case 22: //XOR
			return z11::XOR;
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


bool is_special_instruction(z11::op instruction) {
	switch(instruction) {
		case z11::NOP:
		case z11::J:
		case z11::JAL:
		case z11::ADDI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::SLTI:
		case z11::LW:
		case z11::LUI:
		case z11::SW:
		case z11::BEQ:
		case z11::BNE:
			return false;
	}

	return true;
}
