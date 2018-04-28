#include <iostream>
#include <iomanip>

#include <Clock.h>

#include "run_program.h"
#include "components.h"
#include "instruction_decode.h"

bool halted = false;

void bootstrap_program(void) {
	if_r.pc.latchFrom(instruction_mem.READ());
	Clock::tick();
}

//TODO: add busses to propogate valid bits through the stages, rather than just setting them

void fetch_part1(void) {
	if_instruction_mem_addr_bus.IN().pullFrom(if_r.pc);
	instruction_mem.MAR().latchFrom(if_instruction_mem_addr_bus.OUT());
//TODO: add code for branch instructions
}

void fetch_part2(void) {
	instruction_mem.read();
	ifid_r.ir.latchFrom(instruction_mem.READ());

//TODO: add code for branch instructions
	if_r.pc.perform(Counter::incr4);
	ifid_r.new_pc.perform(Counter::incr4);

	if_pc_forward.IN().pullFrom(if_r.pc);
	ifid_r.pc.latchFrom(if_pc_forward.OUT());

	ifid_r.valid.set();
}

void decode_part1(void) {
	if(!ifid_r.valid.value()) {
		return;
	}

	switch(decode_instruction(ifid_r.ir)) {
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
			id_imm_alu.OP1().pullFrom(ifid_r.ir);
			id_imm_alu.OP2().pullFrom(id_sh_field_shift_amount);
			id_imm_alu.perform(BusALU::op_rshift);
			id_shift_temp_reg.latchFrom(id_imm_alu.OUT());
			break;

		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			id_shift_temp_reg_load_bus.IN().pullFrom(
				GPR(ifid_r.ir(25, 21)));
			id_shift_temp_reg.latchFrom(
				id_shift_temp_reg_load_bus.OUT());
			break;
	}
}

void decode_part2(void) {
	id_valid_forward.IN().pullFrom(ifid_r.valid);
	idex_r.valid.latchFrom(id_valid_forward.OUT());

	if(!ifid_r.valid.value()) {
		return;
	}

	/* we load the contents of registers 'rs' and 'rt' into A and B no
		matter what here, as the 'rs' and 'rt' fields are only 5 bits
		wide, so they will always refer to a valid general purpose
		register, and, at worst, we can just ignore the value we
		loaded later */

	//load A with contents of register 'rs'
	id_a_load_bus.IN().pullFrom(GPR(ifid_r.ir(25, 21)));
	idex_r.a.latchFrom(id_a_load_bus.OUT());

	//load B with contents of register 'rt'
	id_b_load_bus.IN().pullFrom(GPR(ifid_r.ir(20, 16)));
	idex_r.b.latchFrom(id_b_load_bus.OUT());

	id_ir_forward.IN().pullFrom(ifid_r.ir);
	idex_r.ir.latchFrom(id_ir_forward.OUT());


	switch(decode_instruction(ifid_r.ir)) {
		//sign extension
		case z11::ADDI:
		case z11::SLTI:
		case z11::LW:
		case z11::SW:
			id_imm_alu.OP1().pullFrom(ifid_r.ir);
			id_imm_alu.OP2().pullFrom(id_imm_sign_extend_mask);
			id_imm_alu.perform(BusALU::op_extendSign);
			idex_r.imm.latchFrom(id_imm_alu.OUT());
			break;

		//zero extension
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
			id_imm_alu.OP1().pullFrom(ifid_r.ir);
			id_imm_alu.OP2().pullFrom(id_imm_zero_extend_mask);
			id_imm_alu.perform(BusALU::op_and);
			idex_r.imm.latchFrom(id_imm_alu.OUT());
			break;

		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			id_imm_alu.OP1().pullFrom(id_shift_temp_reg);
			id_imm_alu.OP2().pullFrom(id_shift_field_mask);
			id_imm_alu.perform(BusALU::op_and);
			idex_r.imm.latchFrom(id_imm_alu.OUT());
			break;

		//non-immediate instructions, do nothing
		case z11::ADD:
		case z11::SUB:
		case z11::SLT:
		case z11::SLTU:
		case z11::AND:
		case z11::OR:
		case z11::XOR:
		case z11::NOP:
		case z11::HALT:
		case z11::BREAK:
		case z11::UNKNOWN:
			break;

		//TODO: unimplemented cases
		case z11::J:
		case z11::JAL:
		case z11::BEQ:
		case z11::BNE:
		case z11::JR:
		case z11::JALR:
			break;
	}

	id_pc_forward.IN().pullFrom(ifid_r.pc);
	idex_r.pc.latchFrom(id_pc_forward.OUT());

	id_newpc_forward.IN().pullFrom(ifid_r.new_pc);
	idex_r.new_pc.latchFrom(id_newpc_forward.OUT());
}

void execute_part1(void) {
}

void execute_alu_immediate_common(void) {
	ex_alu.OP1().pullFrom(idex_r.a);
	ex_alu.OP2().pullFrom(idex_r.imm);
	exmem_r.c.latchFrom(ex_alu.OUT());
}

void execute_alu_register_common(void) {
	ex_alu.OP1().pullFrom(idex_r.a);
	ex_alu.OP2().pullFrom(idex_r.b);
	exmem_r.c.latchFrom(ex_alu.OUT());
}

void execute_shift_common(void) {
	ex_alu.OP1().pullFrom(idex_r.b);
	ex_alu.OP2().pullFrom(idex_r.imm);
	exmem_r.c.latchFrom(ex_alu.OUT());
}

void execute_part2(void) {
	ex_valid_forward.IN().pullFrom(idex_r.valid);
	exmem_r.valid.latchFrom(ex_valid_forward.OUT());

	if(!idex_r.valid.value()) {
		return;
	}

	ex_pc_forward.IN().pullFrom(idex_r.pc);
	exmem_r.pc.latchFrom(ex_pc_forward.OUT());

	ex_newpc_forward.IN().pullFrom(idex_r.new_pc);
	exmem_r.new_pc.latchFrom(ex_newpc_forward.OUT());

	ex_ir_forward.IN().pullFrom(idex_r.ir);
	exmem_r.ir.latchFrom(ex_ir_forward.OUT());

	switch(decode_instruction(idex_r.ir)) {
		//load/store operations
		case z11::LW:
		case z11::SW:
			ex_b_forward.IN().pullFrom(idex_r.b);
			exmem_r.b.latchFrom(ex_b_forward.OUT());

			ex_alu.OP1().pullFrom(idex_r.a);
			ex_alu.OP2().pullFrom(idex_r.imm);
			ex_alu.perform(BusALU::op_add);
			exmem_r.c.latchFrom(ex_alu.OUT());
			break;

		//ALU operations
		case z11::ADDI:
			execute_alu_immediate_common();
			ex_alu.perform(BusALU::op_add);
			break;
		case z11::ANDI:
			execute_alu_immediate_common();
			ex_alu.perform(BusALU::op_and);
			break;
		case z11::ORI:
			execute_alu_immediate_common();
			ex_alu.perform(BusALU::op_or);
			break;
		case z11::XORI:
			execute_alu_immediate_common();
			ex_alu.perform(BusALU::op_xor);
			break;
		case z11::ADD:
			execute_alu_register_common();
			ex_alu.perform(BusALU::op_add);
			break;
		case z11::SUB:
			execute_alu_register_common();
			ex_alu.perform(BusALU::op_sub);
			break;
		case z11::AND:
			execute_alu_register_common();
			ex_alu.perform(BusALU::op_and);
			break;
		case z11::OR:
			execute_alu_register_common();
			ex_alu.perform(BusALU::op_or);
			break;
		case z11::XOR:
			execute_alu_register_common();
			ex_alu.perform(BusALU::op_xor);
			break;
		case z11::LUI:
			ex_alu.OP1().pullFrom(idex_r.imm);
			ex_alu.OP2().pullFrom(ex_lui_shift_amount);
			exmem_r.c.latchFrom(ex_alu.OUT());
			ex_alu.perform(BusALU::op_lshift);
			break;
		case z11::SLTI:
			if(((int32_t)GPR(idex_r.ir(25, 21)).value()) <
				((int32_t)idex_r.imm.value())) {

				ex_alu.perform(BusALU::op_one);
			}
			else {
				ex_alu.perform(BusALU::op_zero);
			}
			exmem_r.c.latchFrom(ex_alu.OUT());
			break;
		case z11::SLT:
			if(((int32_t)GPR(idex_r.ir(25, 21)).value()) <
				((int32_t)GPR(idex_r.ir(20, 16)).value())) {

				ex_alu.perform(BusALU::op_one);
			}
			else {
				ex_alu.perform(BusALU::op_zero);
			}
			exmem_r.c.latchFrom(ex_alu.OUT());
			break;
		case z11::SLTU:
			if(((uint32_t)GPR(idex_r.ir(25, 21)).value()) <
				((uint32_t)GPR(idex_r.ir(20, 16)).value())) {

				ex_alu.perform(BusALU::op_one);
			}
			else {
				ex_alu.perform(BusALU::op_zero);
			}
			exmem_r.c.latchFrom(ex_alu.OUT());
			break;
		case z11::SLL:
		case z11::SLLV:
			execute_shift_common();
			ex_alu.perform(BusALU::op_lshift);
			break;
		case z11::SRL:
		case z11::SRLV:
			execute_shift_common();
			ex_alu.perform(BusALU::op_rshift);
			break;
		case z11::SRA:
		case z11::SRAV:
			execute_shift_common();
			ex_alu.perform(BusALU::op_rashift);
			break;

		//do nothing
		case z11::NOP:
		case z11::HALT:
			break;

		//TODO: unimplemented
		case z11::J:
		case z11::JAL:
		case z11::BEQ:
		case z11::BNE:
		case z11::JR:
		case z11::JALR:
		case z11::BREAK:
		case z11::UNKNOWN:
			break;
	}
}

void memory_part1(void) {
	if(!exmem_r.valid.value()) {
		return;
	}

	switch(decode_instruction(exmem_r.ir)) {
		//load/store instructions - put addr into MAR
		case z11::LW:
		case z11::SW:
			mem_data_mem_addr_bus.IN().pullFrom(exmem_r.c);
			data_mem.MAR().latchFrom(mem_data_mem_addr_bus.OUT());
			break;

		//do nothing cases
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
		case z11::ADD:
		case z11::SUB:
		case z11::SLT:
		case z11::SLTU:
		case z11::AND:
		case z11::OR:
		case z11::XOR:
		case z11::NOP:
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			break;

		//TODO: unimplemented cases
		case z11::J:
		case z11::JAL:
		case z11::BEQ:
		case z11::BNE:
		case z11::HALT:
		case z11::JR:
		case z11::JALR:
		case z11::BREAK:
		case z11::UNKNOWN:
			break;
	}
}

void memory_part2(void) {
	mem_valid_forward.IN().pullFrom(exmem_r.valid);
	memwb_r.valid.latchFrom(mem_valid_forward.OUT());

	if(!exmem_r.valid.value()) {
		return;
	}

	mem_pc_forward.IN().pullFrom(exmem_r.pc);
	memwb_r.pc.latchFrom(mem_pc_forward.OUT());

	mem_newpc_forward.IN().pullFrom(exmem_r.new_pc);
	memwb_r.new_pc.latchFrom(mem_newpc_forward.OUT());

	mem_ir_forward.IN().pullFrom(exmem_r.ir);
	memwb_r.ir.latchFrom(mem_ir_forward.OUT());


	switch(decode_instruction(exmem_r.ir)) {
		//ALU instructions
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
		case z11::ADD:
		case z11::SUB:
		case z11::SLT:
		case z11::SLTU:
		case z11::AND:
		case z11::OR:
		case z11::XOR:
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			mem_c_forward.IN().pullFrom(exmem_r.c);
			memwb_r.c.latchFrom(mem_c_forward.OUT());
			break;

		case z11::LW:
			data_mem.read();
			memwb_r.memory_data.latchFrom(data_mem.READ());
			break;

		case z11::SW:
			data_mem.write();
			data_mem.WRITE().pullFrom(exmem_r.b);
			break;

		//do nothing cases
		case z11::NOP:
		case z11::HALT:
			break;

		//TODO: unimplemented cases
		case z11::J:
		case z11::JAL:
		case z11::BEQ:
		case z11::BNE:
		case z11::JR:
		case z11::JALR:
		case z11::BREAK:
		case z11::UNKNOWN:
			break;
	}
}

void writeback_part1(void) {
}

void writeback_to_GPR(int gpr_num, StorageObject &src) {
	if(gpr_num == 0) {return;}

	wb_register_write_bus.IN().pullFrom(src);
	GPR(gpr_num).latchFrom(wb_register_write_bus.OUT());
}

void writeback_part2(void) {
	wb_valid_forward.IN().pullFrom(memwb_r.valid);
	post_wb_r.valid.latchFrom(wb_valid_forward.OUT());

	if(!memwb_r.valid.value()) {
		return;
	}

	wb_pc_forward.IN().pullFrom(memwb_r.pc);
	post_wb_r.pc.latchFrom(wb_pc_forward.OUT());

	wb_ir_forward.IN().pullFrom(memwb_r.ir);
	post_wb_r.ir.latchFrom(wb_ir_forward.OUT());
//TODO: make sure we block writes to r0


	switch(decode_instruction(memwb_r.ir)) {
		//immediate ALU instructions
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
			writeback_to_GPR(memwb_r.ir(20, 16), memwb_r.c);
//			wb_register_write_bus.IN().pullFrom(memwb_r.c);
//			GPR(memwb_r.ir(20, 16)).latchFrom(wb_register_write_bus.OUT());
			break;

		//register-register ALU instructions
		case z11::ADD:
		case z11::SUB:
		case z11::SLT:
		case z11::SLTU:
		case z11::AND:
		case z11::OR:
		case z11::XOR:
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			//place result in 'rd'
			writeback_to_GPR(memwb_r.ir(15, 11), memwb_r.c);
//			wb_register_write_bus.IN().pullFrom(memwb_r.c);
//			GPR(memwb_r.ir(15, 11)).latchFrom(wb_register_write_bus.OUT());
			break;

		//load instructions
		case z11::LW:
			writeback_to_GPR(memwb_r.ir(20, 16), memwb_r.memory_data);
//			wb_register_write_bus.IN().pullFrom(memwb_r.memory_data);
//			GPR(memwb_r.ir(20, 16)).latchFrom(wb_register_write_bus.OUT());
			break;

		case z11::HALT:
			halted = true;
			break;

		//do nothing cases
		case z11::NOP:
		case z11::SW:
			break;

		//TODO: unimplemented cases
		case z11::J:
		case z11::JAL:
		case z11::BEQ:
		case z11::BNE:
		case z11::JR:
		case z11::JALR:
		case z11::BREAK:
		case z11::UNKNOWN:
			break;
	}
}

void print_break_information(void) {
	std::cout << std::endl << "   ";

	int num_printed = 0;
	for(unsigned int i = 0; i < NUM_GPRS; ++i) {
		if(GPR(i).value() != 0) {
			if((num_printed) && ((num_printed % 4) == 0)) {
				std::cout << std::endl << "   ";
			}

			std::cout << std::setw(4) << std::right <<
				std::setfill(' ') << GPR(i);

			num_printed++;
		}
	}
}

void print_execution_record(void) {
	if(!post_wb_r.valid.value()) {
		return;
	}

	std::cout << std::hex << std::setw(8) << std::setfill('0') <<
		post_wb_r.pc.value() << ":  ";

	std::cout << std::hex << std::setw(2) << std::setfill('0') <<
		post_wb_r.ir(31, 26);

	z11::op operation = decode_instruction(post_wb_r.ir);

	if(is_special_instruction(operation)) {
		std::cout << " " << std::hex << std::setw(2) <<
			std::setfill('0') << post_wb_r.ir(5, 0);
	}
	else {
		std::cout << "   ";
	}

	std::cout << " " << std::left << std::setw(7) << std::setfill(' ') <<
		z11::mnemonics[operation] << std::right;


	switch(operation) {
		//immediate ALU instructions
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
		case z11::LW:
			std::cout << " " << std::hex <<
				GPR(post_wb_r.ir(20, 16));
			break;

		//register-register ALU instructions
		case z11::ADD:
		case z11::SUB:
		case z11::SLT:
		case z11::SLTU:
		case z11::AND:
		case z11::OR:
		case z11::XOR:
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			std::cout << " " << std::hex <<
				GPR(post_wb_r.ir(15, 11));
			break;

		case z11::BREAK:
			print_break_information();
			break;

		//do nothing cases
		case z11::HALT:
		case z11::NOP:
		case z11::SW:
			break;

		//TODO: unimplemented cases
		case z11::J:
		case z11::JAL:
		case z11::BEQ:
		case z11::BNE:
		case z11::JR:
		case z11::JALR:
		case z11::UNKNOWN:
			break;
	}

	std::cout << std::endl;

	if(halted) {
		std::cout << "Machine Halted - ";
		if(operation == z11::HALT) {
			std::cout << "HALT instruction executed";
		}

		std::cout << std::endl;
	}
}

void run_program(void) {
	bootstrap_program();
	int ctr = 0;

	while(!halted) {
		fetch_part1();
		decode_part1();
		execute_part1();
		memory_part1();
		writeback_part1();
		Clock::tick();

		fetch_part2();
		decode_part2();
		execute_part2();
		memory_part2();
		writeback_part2();
		Clock::tick();

		print_execution_record();

/*
		std::cout << "Got instruction: " << z11::mnemonics[decode_instruction(ifid_r.ir)] << std::endl;

		std::cout << ctr << " contents of registers:" << std::hex << std::endl;
		for(int i = 0; i < 32; ++i) {
			std::cout << "\t" << i << "[" << GPR(i).value() << "]" << std::endl;
		}
		std::cout << "\tIDEX A: " << idex_r.a << std::endl;
		std::cout << "\tIDEX imm: " << idex_r.imm << std::endl;
		std::cout << "\tEXMEM c: " << exmem_r.c << std::endl;
		std::cout << "\tMEMWB c: " << memwb_r.c << std::endl;
		ctr++;
*/
//		if(ctr > 30) {halted = true;}
	}
}
