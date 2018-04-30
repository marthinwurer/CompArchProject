/**
 * Source file for "connections" module that creates all of the connections
 * between z88 CPU components.
 *
 * Authors: Coleman Link and Ben Maitland
 */

//local project includes
#include "connections.h"
#include "components.h"

/**
 * Connect all of the registers in the register file to the input of the
 * specifed bus. Utility function.
 *
 * @param b The bus to connect the register file to.
 */
void connect_reg_file_to_bus_input(Bus &b);

/**
 * Connect all of the registers in the register file to the output of the
 * specified bus. Utility function.
 *
 * @param b The bus to connect the register file to.
 */
void connect_reg_file_to_bus_output(Bus &b);

/**
 * Make the connections that will be used in the fetch stage of the pipeline.
 */
void make_fetch_stage_connections(void);

/**
 * Make the connections that will be used in the decode stage of the pipeline.
 */
void make_decode_stage_connections(void);

/**
 * Make the connections that will be used in the execute stage of the
 * pipeline.
 */
void make_execute_stage_connections(void);

/**
 * Make the connections that will be used in the memory stage of the pipeline.
 */
void make_memory_stage_connections(void);

/**
 * Make the connections that will be used in the writeback stage of the
 * pipeline.
 */
void make_writeback_stage_connections(void);

/**
 * Make the connections that will be used for forwarding results from leter
 * instructions back to the execute stage.
 */
void make_connections_for_forwarding(void);

/**
 * Make the connections that will be used for inserting NOPs in order to do
 * stalling after load instructions.
 */
void make_connections_for_stalling(void);

void connect_components(void) {
	//connection for bootstrapping program entry point
	if_r.pc.connectsTo(instruction_mem.READ());

	//make the connections used for each stage of the pipeline
	make_fetch_stage_connections();
	make_decode_stage_connections();
	make_execute_stage_connections();
	make_memory_stage_connections();
	make_writeback_stage_connections();

	//make additional connections used in forwarding and stalling
	make_connections_for_forwarding();
	make_connections_for_stalling();
}

void make_fetch_stage_connections(void) {
	/* for loading the instruction memory MAR with the contents of the
		program counter */
	if_r.pc.connectsTo(if_instruction_mem_addr_bus.IN());
	instruction_mem.MAR().connectsTo(if_instruction_mem_addr_bus.OUT());
	//for retrieving instructions from instruction memory
	ifid_r.ir.connectsTo(instruction_mem.READ());

	/* for forwarding the program counter contents on to the IF/ID
		pipeline register. The 'if_r' pipeline register's program
		counter is considered to be the "real" one, since it is the
		one used to retrieve instructions from instruction memory.
		The PCs in other pipeline registers are mainly used for
		storing the previous PC values for use in instruction trace
		printouts and branch offset calculations */
	if_r.pc.connectsTo(if_pc_forward.IN());
	ifid_r.pc.connectsTo(if_pc_forward.OUT());
	ifid_r.new_pc.connectsTo(if_pc_forward.OUT());

	/* connection for receiving the destination address from a jump
		instruction into the program counter */
	if_r.pc.connectsTo(if_branch_bus.OUT());

	/* connection for receiving the destination address from a branch
		instruction into the program counter */
	if_r.pc.connectsTo(id_imm_alu.OUT());
}

void make_decode_stage_connections(void) {
	//connections for reading 'rs' register contents into A
	connect_reg_file_to_bus_input(id_a_load_bus);
	idex_r.a.connectsTo(id_a_load_bus.OUT());

	//connections for reading 'rt' register contents into B
	connect_reg_file_to_bus_input(id_b_load_bus);
	idex_r.b.connectsTo(id_b_load_bus.OUT());

	/* connections for forwarding contents of instruction register through
		to ID/EX pipeline register */
	ifid_r.ir.connectsTo(id_ir_forward.IN());
	idex_r.ir.connectsTo(id_ir_forward.OUT());

	/* connections used for sign extension (or zero extension) on
		immediate values in instructions */
	ifid_r.ir.connectsTo(id_imm_alu.OP1());
	id_imm_sign_extend_mask.connectsTo(id_imm_alu.OP2());
	id_imm_zero_extend_mask.connectsTo(id_imm_alu.OP2());
	idex_r.imm.connectsTo(id_imm_alu.OUT());

	/* for forwarding the contents of the program counter through to the
		ID/EX pipeline register */
	ifid_r.pc.connectsTo(id_pc_forward.IN());
	idex_r.pc.connectsTo(id_pc_forward.OUT());

	/* for forwarding the contents of the valid bit through to the ID/EX
		pipeline register */
	ifid_r.valid.connectsTo(id_valid_forward.IN());
	idex_r.valid.connectsTo(id_valid_forward.OUT());

	/* connections for extracting the contents of the 'sh' field in shift
		instructions */
	id_sh_field_shift_amount.connectsTo(id_imm_alu.OP2());
	id_temp_reg.connectsTo(id_imm_alu.OUT());
	id_temp_reg.connectsTo(id_imm_alu.OP1());
	id_shift_field_mask.connectsTo(id_imm_alu.OP2());

	/* connections for loading the contents of one of the general purpose
		registers into the temporary decode register. This is needed
		for instructions whose work in the decode phase spans two
		clock ticks, like jump instructions JR and JALR, that must
		retrieve the contents of the register containing the
		destination address to jump to on the first tick, then write
		it to the PC on the second tick. Similarly, variable shift
		instructions need to retrieve the contents of the register
		containing the shift amount on the first tick, then apply a
		mask to get the low order 5 bits on the second tick. */
	connect_reg_file_to_bus_input(id_temp_reg_load_bus);
	id_temp_reg.connectsTo(id_temp_reg_load_bus.OUT());

	/* connections for computing the destination of jump instructions and
		sending it to the program counter */
	id_jump_target_mask.connectsTo(id_imm_alu.OP2());
	id_temp_reg.connectsTo(if_branch_bus.IN());

	/* connection for computing branch offsets based on program counter
		contents */
	ifid_r.new_pc.connectsTo(id_imm_alu.OP2());
}

void make_execute_stage_connections(void) {
	/* for forwarding the instruction register contents through to the
		EX/MEM pipeline register */
	idex_r.ir.connectsTo(ex_ir_forward.IN());
	exmem_r.ir.connectsTo(ex_ir_forward.OUT());

	/* for performing ALU instructions (and other instructions that make
		use of the ALU */
	idex_r.a.connectsTo(ex_alu.OP1());
	idex_r.b.connectsTo(ex_alu.OP2());
	idex_r.imm.connectsTo(ex_alu.OP2());
	exmem_r.c.connectsTo(ex_alu.OUT());

	//for performing shifts needed for LUI instructions
	idex_r.imm.connectsTo(ex_alu.OP1());
	ex_lui_shift_amount.connectsTo(ex_alu.OP2());

	//for executing shift instructions
	idex_r.b.connectsTo(ex_alu.OP1());

	/* for forwarding register 'rt' contents through to EX/MEM pipeline
		register */
	idex_r.b.connectsTo(ex_b_forward.IN());
	exmem_r.b.connectsTo(ex_b_forward.OUT());

	/* for forwarding program counter contents through to EX/MEM pipeline
		register */
	idex_r.pc.connectsTo(ex_pc_forward.IN());
	exmem_r.pc.connectsTo(ex_pc_forward.OUT());

	/* for forwarding valid bit contents through to EX/MEM pipeline
		register */
	idex_r.valid.connectsTo(ex_valid_forward.IN());
	exmem_r.valid.connectsTo(ex_valid_forward.OUT());

	/* for computing the return addresses to be saved by JAL and JALR
		instructions */
	idex_r.pc.connectsTo(ex_alu.OP1());
	ex_jump_link_return_offset.connectsTo(ex_alu.OP2());
}

void make_memory_stage_connections(void) {
	/* for forwarding instruction register contents through to MEM/WB
		pipeline register */
	exmem_r.ir.connectsTo(mem_ir_forward.IN());
	memwb_r.ir.connectsTo(mem_ir_forward.OUT());

	/* for forwarding execution results through to the MEM/WB popeline
		register */
	exmem_r.c.connectsTo(mem_c_forward.IN());
	memwb_r.c.connectsTo(mem_c_forward.OUT());

	/* for loading the address of data to be stored or loaded into the
		MAR of the data memory */
	exmem_r.c.connectsTo(mem_data_mem_addr_bus.IN());
	data_mem.MAR().connectsTo(mem_data_mem_addr_bus.OUT());

	//for reading data in from memory (for loads)
	memwb_r.c.connectsTo(data_mem.READ());
	//for writing data out to memory (for stores)
	exmem_r.b.connectsTo(data_mem.WRITE());

	/* for forwarding the program counter through to the MEM/WB pipeline
		register */
	exmem_r.pc.connectsTo(mem_pc_forward.IN());
	memwb_r.pc.connectsTo(mem_pc_forward.OUT());

	/* for forwarding the valid bit through to the MEM/WB pipeline
		register */
	exmem_r.valid.connectsTo(mem_valid_forward.IN());
	memwb_r.valid.connectsTo(mem_valid_forward.OUT());
}

void make_writeback_stage_connections(void) {
	//used to write ALU results to destination registers
	memwb_r.c.connectsTo(wb_register_write_bus.IN());
	connect_reg_file_to_bus_output(wb_register_write_bus);

	//for forwarding valid bit into final "printout" storage
	memwb_r.valid.connectsTo(wb_valid_forward.IN());
	post_wb_r.valid.connectsTo(wb_valid_forward.OUT());

	//for forwarding program counter to final "printout" storage
	memwb_r.pc.connectsTo(wb_pc_forward.IN());
	post_wb_r.pc.connectsTo(wb_pc_forward.OUT());

	//for forwarding instruction register to final "printout" storage
	memwb_r.ir.connectsTo(wb_ir_forward.IN());
	post_wb_r.ir.connectsTo(wb_ir_forward.OUT());
}

void make_connections_for_forwarding(void) {
	/* for forwarding execution and load results that write the executing
		instruction's 'rs' register back to the execute stage */
	exmem_r.c.connectsTo(idex_a_fill.IN());
	memwb_r.c.connectsTo(idex_a_fill.IN());
	idex_r.a.connectsTo(idex_a_fill.OUT());

	/* for forwarding execution and load results that write the executing
		instruction's 'rt' register back to the execute stage */
	exmem_r.c.connectsTo(idex_b_fill.IN());
	memwb_r.c.connectsTo(idex_b_fill.IN());
	idex_r.b.connectsTo(idex_b_fill.OUT());

	/* for forwarding execution results that write the 'rs' register of
		instructions that use the value in that register in the decode
		stage */
	exmem_r.c.connectsTo(id_temp_reg_load_bus.IN());
	memwb_r.c.connectsTo(id_temp_reg_load_bus.IN());
}

void make_connections_for_stalling(void) {
	/* for inserting NOP instructions into the pipeline to produce a
		stall effect */
	stalling_nop_constant.connectsTo(idex_nop_insert_bus.IN());
	idex_r.ir.connectsTo(idex_nop_insert_bus.OUT());
}

void connect_reg_file_to_bus_input(Bus &b) {
	//for each general purpose register
	for(unsigned int i = 0; i < NUM_GPRS; ++i) {
		//connect it to the bus
		GPR(i).connectsTo(b.IN());
	}
}

void connect_reg_file_to_bus_output(Bus &b) {
	//for each general purpose register
	for(unsigned int i = 0; i < NUM_GPRS; ++i) {
		//connect it to the bus
		GPR(i).connectsTo(b.OUT());
	}
}
