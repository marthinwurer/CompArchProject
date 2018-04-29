#include "components.h"

const unsigned int WORD_WIDTH(32);
const unsigned int NUM_GPRS(32);
const unsigned int ADDR_WIDTH(32);
const unsigned int UNIT_BITS(8);
const unsigned int MAX_ADDR(0xFFFF);

StorageObject r0("R0", WORD_WIDTH, 0);
StorageObject r1("R1", WORD_WIDTH, 0);
StorageObject r2("R2", WORD_WIDTH, 0);
StorageObject r3("R3", WORD_WIDTH, 0);
StorageObject r4("R4", WORD_WIDTH, 0);
StorageObject r5("R5", WORD_WIDTH, 0);
StorageObject r6("R6", WORD_WIDTH, 0);
StorageObject r7("R7", WORD_WIDTH, 0);
StorageObject r8("R8", WORD_WIDTH, 0);
StorageObject r9("R9", WORD_WIDTH, 0);
StorageObject r10("R10", WORD_WIDTH, 0);
StorageObject r11("R11", WORD_WIDTH, 0);
StorageObject r12("R12", WORD_WIDTH, 0);
StorageObject r13("R13", WORD_WIDTH, 0);
StorageObject r14("R14", WORD_WIDTH, 0);
StorageObject r15("R15", WORD_WIDTH, 0);
StorageObject r16("R16", WORD_WIDTH, 0);
StorageObject r17("R17", WORD_WIDTH, 0);
StorageObject r18("R18", WORD_WIDTH, 0);
StorageObject r19("R19", WORD_WIDTH, 0);
StorageObject r20("R20", WORD_WIDTH, 0);
StorageObject r21("R21", WORD_WIDTH, 0);
StorageObject r22("R22", WORD_WIDTH, 0);
StorageObject r23("R23", WORD_WIDTH, 0);
StorageObject r24("R24", WORD_WIDTH, 0);
StorageObject r25("R25", WORD_WIDTH, 0);
StorageObject r26("R26", WORD_WIDTH, 0);
StorageObject r27("R27", WORD_WIDTH, 0);
StorageObject r28("R28", WORD_WIDTH, 0);
StorageObject r29("R29", WORD_WIDTH, 0);
StorageObject r30("R30", WORD_WIDTH, 0);
StorageObject r31("R31", WORD_WIDTH, 0);

StorageObject *gprs[NUM_GPRS] = {
	&r0,
	&r1,
	&r2,
	&r3,
	&r4,
	&r5,
	&r6,
	&r7,
	&r8,
	&r9,
	&r10,
	&r11,
	&r12,
	&r13,
	&r14,
	&r15,
	&r16,
	&r17,
	&r18,
	&r19,
	&r20,
	&r21,
	&r22,
	&r23,
	&r24,
	&r25,
	&r26,
	&r27,
	&r28,
	&r29,
	&r30,
	&r31
};

Memory instruction_mem("IMemory", ADDR_WIDTH, UNIT_BITS, MAX_ADDR, ADDR_WIDTH / UNIT_BITS);
Memory data_mem("DMemory", ADDR_WIDTH, UNIT_BITS, MAX_ADDR, ADDR_WIDTH / UNIT_BITS);

if_reg::if_reg(void) :
	pc("PC", ADDR_WIDTH, 0)
{}

ifid_reg::ifid_reg(void) :
	valid("valid", 1, 0),
	pc("PC", ADDR_WIDTH, 0),
	new_pc("new PC", ADDR_WIDTH, 0),
	ir("IR", WORD_WIDTH, 0)
{}

idex_reg::idex_reg(void) :
	valid("valid", 1, 0),
	pc("PC", ADDR_WIDTH, 0),
	ir("IR", WORD_WIDTH, 0),
	a("A", WORD_WIDTH, 0),
	b("B", WORD_WIDTH, 0),
	imm("IMM", WORD_WIDTH, 0),
	cond("cond", 1, 0)
{}

exmem_reg::exmem_reg(void) :
	valid("valid", 1, 0),
	pc("PC", ADDR_WIDTH, 0),
	ir("IR", WORD_WIDTH, 0),
	b("B", WORD_WIDTH, 0),
	c("C", WORD_WIDTH, 0)
{}

memwb_reg::memwb_reg(void) :
	valid("valid", 1, 0),
	pc("PC", ADDR_WIDTH, 0),
	ir("IR", WORD_WIDTH, 0),
	c("C", WORD_WIDTH, 0),
	memory_data("memory data", WORD_WIDTH, 0)
{}

post_wb_reg::post_wb_reg(void) :
	valid("valid", 1, 0),
	ir("IR", WORD_WIDTH, 0),
	pc("PC", ADDR_WIDTH, 0)
{}


if_reg if_r{};
ifid_reg ifid_r{};
idex_reg idex_r{};
exmem_reg exmem_r{};
memwb_reg memwb_r{};
post_wb_reg post_wb_r{};

//if stage busses and ALUs
//BusALU if_pc_incrementer("if_pc_incremented", WORD_WIDTH);
Bus if_instruction_mem_addr_bus("if_instruction_mem_addr_bus", ADDR_WIDTH);
Bus if_pc_forward("if_pc_forward", WORD_WIDTH);
Bus if_branch_bus("if_branch_bus", ADDR_WIDTH);

//id stage busses and ALUs
Bus id_valid_forward("id_valid_forward", 1);
Bus id_pc_forward("if_pc_forward", WORD_WIDTH);
Bus id_ir_forward("id_ir_forward", WORD_WIDTH);
Bus id_a_load_bus("id_a_load_bus", WORD_WIDTH);
Bus id_b_load_bus("id_b_load_bus", WORD_WIDTH);
BusALU id_imm_alu("id_imm_alu", WORD_WIDTH);
StorageObject id_imm_sign_extend_mask("id_imm_sign_extend_mask", WORD_WIDTH, 0x00008000);
StorageObject id_imm_zero_extend_mask("id_imm_zero_extend_mask", WORD_WIDTH, 0x0000FFFF);
StorageObject id_sh_field_shift_amount("id_sh_field_shift_amount", WORD_WIDTH, 0x00000006);
StorageObject id_shift_temp_reg("id_shift_temp_reg", WORD_WIDTH, 0);
Bus id_shift_temp_reg_load_bus("id_shift_temp_reg_load_bus", WORD_WIDTH);
StorageObject id_shift_field_mask("id_shift_field_mask", WORD_WIDTH, 0x0000001F);
StorageObject id_jump_target_mask("id_jump_target_mask", WORD_WIDTH, 0x03FFFFFF);

//ex stage busses and ALUs
Bus ex_valid_forward("ex_valid_forward", 1);
Bus ex_pc_forward("ex_pc_forward", WORD_WIDTH);
Bus ex_ir_forward("ex_ir_forward", WORD_WIDTH);
Bus ex_b_forward("ex_b_forward", WORD_WIDTH);
BusALU ex_alu("ex_alu", WORD_WIDTH);
StorageObject ex_lui_shift_amount("ex_lui_shift_amount", WORD_WIDTH, 0x00000010);
StorageObject ex_jump_link_return_offset("ex_jump_link_return_offset", WORD_WIDTH, 0x00000008);

//mem stage busses and ALUs
Bus mem_valid_forward("mem_valid_forward", 1);
Bus mem_pc_forward("mem_pc_forward", WORD_WIDTH);
Bus mem_ir_forward("mem_ir_forward", WORD_WIDTH);
Bus mem_c_forward("mem_c_forward", WORD_WIDTH);
Bus mem_data_mem_addr_bus("mem_data_mem_addr_bus", ADDR_WIDTH);

//wb stage busses and ALUs
Bus wb_register_write_bus("wb_register_write_bus", WORD_WIDTH);
Bus wb_pc_forward("wb_pc_forward", ADDR_WIDTH);
Bus wb_ir_forward("wb_ir_forward", WORD_WIDTH);
Bus wb_valid_forward("wb_valid_forward", 1);


//forwarding busses
Bus idex_a_fill("idex_a_fill", WORD_WIDTH);
Bus idex_b_fill("idex_b_fill", WORD_WIDTH);
