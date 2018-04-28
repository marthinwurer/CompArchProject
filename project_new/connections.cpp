#include "connections.h"
#include "components.h"

void connect_reg_file_to_bus_input(Bus &b);
void connect_reg_file_to_bus_output(Bus &b);

void connect_components(void) {
	//used for bootstrapping program entry point
	if_r.pc.connectsTo(instruction_mem.READ());

	//IF stage connections
	if_r.pc.connectsTo(if_instruction_mem_addr_bus.IN());
	instruction_mem.MAR().connectsTo(if_instruction_mem_addr_bus.OUT());
	ifid_r.ir.connectsTo(instruction_mem.READ());

	if_r.pc.connectsTo(if_pc_forward.IN());
	ifid_r.pc.connectsTo(if_pc_forward.OUT());
	//TODO: add connections needed for a branch

	//ID stage connections
	connect_reg_file_to_bus_input(id_a_load_bus);
	idex_r.a.connectsTo(id_a_load_bus.OUT());

	connect_reg_file_to_bus_input(id_b_load_bus);
	idex_r.b.connectsTo(id_b_load_bus.OUT());

	ifid_r.ir.connectsTo(id_ir_forward.IN());
	idex_r.ir.connectsTo(id_ir_forward.OUT());

	ifid_r.ir.connectsTo(id_imm_alu.OP1());
	id_imm_sign_extend_mask.connectsTo(id_imm_alu.OP2());
	id_imm_zero_extend_mask.connectsTo(id_imm_alu.OP2());
	idex_r.imm.connectsTo(id_imm_alu.OUT());

	ifid_r.pc.connectsTo(id_pc_forward.IN());
	idex_r.pc.connectsTo(id_pc_forward.OUT());

	ifid_r.new_pc.connectsTo(id_newpc_forward.IN());
	idex_r.new_pc.connectsTo(id_newpc_forward.OUT());

	ifid_r.valid.connectsTo(id_valid_forward.IN());
	idex_r.valid.connectsTo(id_valid_forward.OUT());

	id_sh_field_shift_amount.connectsTo(id_imm_alu.OP2());
	id_shift_temp_reg.connectsTo(id_imm_alu.OUT());
	id_shift_temp_reg.connectsTo(id_imm_alu.OP1());
	id_shift_field_mask.connectsTo(id_imm_alu.OP2());

	connect_reg_file_to_bus_input(id_shift_temp_reg_load_bus);
	id_shift_temp_reg.connectsTo(id_shift_temp_reg_load_bus.OUT());

	//EX stage connections
	idex_r.ir.connectsTo(ex_ir_forward.IN());
	exmem_r.ir.connectsTo(ex_ir_forward.OUT());

	idex_r.a.connectsTo(ex_alu.OP1());
	idex_r.b.connectsTo(ex_alu.OP2());
	idex_r.imm.connectsTo(ex_alu.OP2());
	exmem_r.c.connectsTo(ex_alu.OUT());

	idex_r.imm.connectsTo(ex_alu.OP1());
	ex_lui_shift_amount.connectsTo(ex_alu.OP2());

	idex_r.b.connectsTo(ex_alu.OP1());

	idex_r.b.connectsTo(ex_b_forward.IN());
	exmem_r.b.connectsTo(ex_b_forward.OUT());

	idex_r.new_pc.connectsTo(ex_alu.OP1());

	idex_r.pc.connectsTo(ex_pc_forward.IN());
	exmem_r.pc.connectsTo(ex_pc_forward.OUT());

	idex_r.new_pc.connectsTo(ex_newpc_forward.IN());
	exmem_r.new_pc.connectsTo(ex_newpc_forward.OUT());

	idex_r.valid.connectsTo(ex_valid_forward.IN());
	exmem_r.valid.connectsTo(ex_valid_forward.OUT());

	//MEM stage connections
	exmem_r.ir.connectsTo(mem_ir_forward.IN());
	memwb_r.ir.connectsTo(mem_ir_forward.OUT());

	exmem_r.c.connectsTo(mem_c_forward.IN());
	memwb_r.c.connectsTo(mem_c_forward.OUT());

	exmem_r.c.connectsTo(mem_data_mem_addr_bus.IN());
	data_mem.MAR().connectsTo(mem_data_mem_addr_bus.OUT());
	memwb_r.memory_data.connectsTo(data_mem.READ());
	exmem_r.b.connectsTo(data_mem.WRITE());

	exmem_r.pc.connectsTo(mem_pc_forward.IN());
	memwb_r.pc.connectsTo(mem_pc_forward.OUT());

	exmem_r.new_pc.connectsTo(mem_newpc_forward.IN());
	memwb_r.new_pc.connectsTo(mem_newpc_forward.OUT());

	exmem_r.valid.connectsTo(mem_valid_forward.IN());
	memwb_r.valid.connectsTo(mem_valid_forward.OUT());

	//WB stage connections
	memwb_r.c.connectsTo(wb_register_write_bus.IN());
	connect_reg_file_to_bus_output(wb_register_write_bus);

	memwb_r.memory_data.connectsTo(wb_register_write_bus.IN());

	memwb_r.valid.connectsTo(wb_valid_forward.IN());
	post_wb_r.valid.connectsTo(wb_valid_forward.OUT());

	memwb_r.pc.connectsTo(wb_pc_forward.IN());
	post_wb_r.pc.connectsTo(wb_pc_forward.OUT());

	memwb_r.ir.connectsTo(wb_ir_forward.IN());
	post_wb_r.ir.connectsTo(wb_ir_forward.OUT());
}

void connect_reg_file_to_bus_input(Bus &b) {
	for(unsigned int i = 0; i < NUM_GPRS; ++i) {
		GPR(i).connectsTo(b.IN());
	}
}

void connect_reg_file_to_bus_output(Bus &b) {
	for(unsigned int i = 0; i < NUM_GPRS; ++i) {
		GPR(i).connectsTo(b.OUT());
	}
}
