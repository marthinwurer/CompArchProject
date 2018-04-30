/**
 * Source file for "run_program" module that performs execution of
 * instructions.
 *
 * Authors: Coleman Link and Ben Maitland
 */

//C++ includes
#include <iostream>
#include <iomanip>

//arch library includes
#include <Clock.h>

//local project includes
#include "run_program.h"
#include "components.h"
#include "instruction_decode.h"



//global flag determining whether the CPU has been halted or not
bool halted = false;


/***********************************
 * Misc. functions
 ***********************************/

	/**
	 * Perform the initial read of a program's entry point into the
	 * program counter.
	 */
	void bootstrap_program(void);


/***********************************
 * Instruction fetch functions
 ***********************************/

	/**
	 * Determine if the instruction currently in the IF/ID register is a
	 * jump instruction.
	 *
	 * @returns True if the IF/ID register currently contains a jump
	 *	instruction, false otherwise.
	 */
	bool id_instruction_is_jump(void);

	/**
	 * Determine if the instruction currently in the IF/ID register is a
	 * branch instruction whose branch will be taken.
	 *
	 * @returns True if the IF/ID register contains a branch instruction,
	 *	false otherwise.
	 */
	bool id_instruction_is_taken_branch(void);

	/**
	 * Set up the CPU operations for the first tick (of two) in the
	 * fetch stage for the current cycle. Just loads the value in PC into
	 * the instruction memory's MAR.
	 */
	void fetch_part1(void);

	/**
	 * Set up the CPU operations for the second tick (of two) in the
	 * fetch stage for the current cycle.
	 * Reads next instruction from instruction memory, determines if we
	 * are going to branch/jump or just move to the next instruction and
	 * updates the PC accordingly. Forwards PC contents to IF/ID pipeline
	 * register.
	 */
	void fetch_part2(void);


/***********************************
 * Instruction decode functions
 ***********************************/

	/**
	 * Set up operations for sign extending the immediate value used for
	 * a branch offset.
	 */
	void decode_sign_extend_branch_offset(void);

	/**
	 * Within the decode stage, move the contents of the instructions 'rs'
	 * register (or a forwarded value that will be written to that
	 * register) into the decode stage temp register.
	 */
	void decode_load_rs_into_temp(void);

	/**
	 * Within the decode stage, get the value that should be used for the
	 * value in the 'rs' register specified by the branch instruction.
	 * This function may simply pull the value out of the register, or may
	 * retrieve (forward) it from a later stage in the pipeline that is
	 * going to update the register specified as 'rs' in the branch
	 * instruction.
	 *
	 * @returns The value contained in the 'rs' register for the
	 *	instruction in the IF/ID pipeline register.
	 */
	long decode_get_branch_rs_value(void);

	/**
	 * Within the decode stage, get the value that should be used for the
	 * value in the 'rt' register specified by the branch instruction.
	 * This function may simply pull the value out of the register, or may
	 * retrieve (forward) it from a later stage in the pipeline that is
	 * going to update the register specified as 'rt' in the branch
	 * instruction.
	 *
	 * @returns The value contained in the 'rt' register for the
	 *	instruction in the IF/ID pipeline register.
	 */
	long decode_get_branch_rt_value(void);

	/**
	 * Set up the CPU operations for the first tick (of two) in the
	 * decode stage for the current cycle.
	 * Performs necessary first-tick operations for instructions that need
	 * both ticks. This includes branches, jumps, and shifts, that must
	 * calculate targets, offsets, and extract the contents of the 'sh'
	 * field, respectively on the first tick.
	 */
	void decode_part1(void);

	/**
	 * Set up the CPU operations for the second tick (of two) in the
	 * decode stage for the current cycle.
	 * Loads the contents of 'rs' and 'rt' registers into A and B
	 * registers, perform sign or zero extension of immediate fields,
	 * and finish calculating branch and jump destinations, among other
	 * things.
	 */
	void decode_part2(void);


/***********************************
 * Instruction execute functions
 ***********************************/

	/**
	 * Determine what general purpose register (if any) is going to be
	 * written to by the instruction currently in the EX/MEM pipeline
	 * register.
	 *
	 * @return The index number of the GPR that will be written, or 0
	 *	if no GPR will be written by the instruction (note that GPR 0
	 *	cannot have its value overwritten, so returning 0 for
	 *	"no write" does not interfere with the results).
	 */
	long gpr_written_by_mem_stage_instruction(void);

	/**
	 * Determine what general purpose register (if any) is going to be
	 * written to by the instruction currently in the MEM/WB pipeline
	 * register.
	 *
	 * @return The index number of the GPR that will be written, or 0
	 *	if no GPR will be written by the instruction (note that GPR 0
	 *	cannot have its value overwritten, so returning 0 for
	 *	"no write" does not interfere with the results).
	 */
	long gpr_written_by_wb_stage_instruction(void);

	/**
	 * Set up the CPU operations for the first tick (of two) in the
	 * execute stage for the current cycle.
	 * The first tick of the execute stage is dedicated entirely to
	 * forwarding, and pulls in result values from the EX/MEM and MEM/WB
	 * pipeline registers into the ID/EX pipeline register for use in
	 * execution, as needed.
	 */
	void execute_part1(void);

	/**
	 * Perform the setup common to the execution of all ALU immediate
	 * instructions. Sets ALU inputs and outputs (leaving the caller to
	 * only specify an operation to perform).
	 */
	void execute_alu_immediate_common(void);

	/**
	 * Perform the setup common to the execution of all register-register
	 * ALU instructions. Sets ALU inputs and outputs (leaving the caller
	 * to only specify an operation to perform).
	 */
	void execute_alu_register_common(void);

	/**
	 * Perform the setup common to the execution of all shift
	 * instructions. Sets SLU inputs and outputs (leaving the caller to
	 * only specify an operation to perform).
	 */
	void execute_shift_common(void);

	/**
	 * Set up the CPU operations for the second tick (of two) in the
	 * execute stage for the current cycle.
	 * Forwards data from ID/EX pipeline register to EX/MEM pipeline
	 * register, performs ALU operations and other calculations.
	 */
	void execute_part2(void);


/***********************************
 * Instruction memory functions
 ***********************************/

	/**
	 * Set up the CPU operations for the first tick (of two) in the
	 * memory stage for the current cycle.
	 * The first tick of the memory stage just loads the data memory
	 * address into the data memory MAR for load and store instructions.
	 * No operations are performed for any other instruction types.
	 */
	void memory_part1(void);

	/**
	 * Set up the CPU operations for the second tick (of two) in the
	 * memory stage for the current cycle.
	 * The second tick of the memory stage forwards the contents of the
	 * EX/MEM pipeline register to the MEM/WB pipeline register (if
	 * necessary) and performs memory reads/writes for load/store
	 * instructions.
	 */
	void memory_part2(void);


/***********************************
 * Instruction writeback functions
 ***********************************/

	/**
	 * Set up the CPU operations for the first tick (of two) in the
	 * writeback stage for the current cycle.
	 * Tick 1 of the writeback stage forwards results to the
	 * post-writeback pipeline register (for instructions tracing), writes
	 * retuls values into GPRs, and marks the CPU as halted on invalid,
	 * unimplemented, or halt instructions.
	 */
	void writeback_part1(void);

	/**
	 * Set up the operations needed to write a result value to the
	 * specified general purpose register.
	 *
	 * @param gpr_num The index in the register file of the GPR to write
	 *	to.
	 * @param src The source register for the data to be written to the
	 *	GPR. Must be connected to 'wb_register_write_bus' input.
	 */
	void writeback_to_GPR(int gpr_num, StorageObject &src);

	/**
	 * Set up the CPU operations for the second tick (of two) in the
	 * writeback stage for the current cycle.
	 * No operations are performed on the second tick for the writeback
	 * stage. This function is left as a placeholder.
	 */
	void writeback_part2(void);

/***********************************
 * Instruction tracing functions
 ***********************************/

	/**
	 * Print any non-zero general purpose registers, up to 4 per line.
	 * Used for executing break instructions.
	 */
	void print_break_information(void);

	/**
	 * Print out the details of the instruction that most recently
	 * completed the writeback stage. Also print halt messages if
	 * necessary.
	 */
	void print_execution_record(void);


/***********************************
 * Stalling functions              *
 ***********************************/

	/**
	 * Determine if the instruction currently in the IF/ID pipeline
	 * register will need to be stalled because it uses the value stored
	 * in a register that will be written by a load instruction currently
	 * in the ID/EX pipeline register.
	 *
	 * @return True if we must stall for this reason, false otherwise.
	 */
	bool must_stall_id_phase_due_to_load_in_idex_register(void);

	/**
	 * Determine if the instruction currently in the IF/ID pipeline
	 * register will need to be stalled because it uses the value stored
	 * in a register that will be written by a load instruction currently
	 * in the EX/MEM pipeline register. In particular, instructions that
	 * use the value of a register within the decode stage (branches,
	 * jumps, etc.) will need to wait if they pull from a register that
	 * will be written to by a load instruction in the memory stage.
	 *
	 * @return True if we must stall for this reason, false otherwise.
	 */
	bool must_stall_id_phase_due_to_load_in_exmem_register(void);

	/**
	 * Determine what general purpose register (if any) is going to be
	 * written to by the instruction currently in the ID/EX pipeline
	 * register.
	 *
	 * @return The index number of the GPR that will be written, or 0
	 *	if no GPR will be written by the instruction (note that GPR 0
	 *	cannot have its value overwritten, so returning 0 for
	 *	"no write" does not interfere with the resutls).
	 */
	long gpr_written_by_ex_stage_instruction(void);

	/**
	 * Determine if the instruction currently in the IF/ID pipeline
	 * register will need to be stalled because it uses the value stored
	 * in a register that will be written by an instruction (of any type)
	 * currently in the ID/EX pipeline register. In particular,
	 * instructions like branches and jumps, that use value they retrieve
	 * in the decode stage, will need to wait if the value they need to
	 * retrieve is currently being calculated by an instruction in the
	 * execute stage.
	 *
	 * @return True if we must stall for this reason, false otherwise.
	 */
	bool must_stall_id_phase_to_use_result_in_id_phase(void);

	/**
	 * Determine if the instruction in the IF/ID pipeline register must be
	 * stalled (delayed from entering the decode stage).
	 *
	 * @returns True if the instruction must be stalled, false otherwise.
	 */
	bool must_stall_id_phase(void);

	/**
	 * Perform the setup necessary to insert a NOP instruction with the
	 * correct source address into the ID/EX pipeline register. Doing this
	 * fills the gap created by stalling an instruction from entering the
	 * decode stage.
	 */
	void insert_nop_into_idex_reg(void);






/***********************************
 * Function implementations        *
 ***********************************/

void bootstrap_program(void) {
	if_r.pc.latchFrom(instruction_mem.READ());
	Clock::tick();
}

bool id_instruction_is_jump(void) {
	switch(decode_instruction(ifid_r.ir)) {
		case z11::J:
		case z11::JAL:
		case z11::JR:
		case z11::JALR:
			return true;
	}

	return false;
}

bool id_instruction_is_taken_branch(void) {
	switch(decode_instruction(ifid_r.ir)) {
		case z11::BEQ:
		case z11::BNE:
			/* 'cond' bit in ID/EX register will be set after
				first tick of decode phase for branch
				instruction */
			return (idex_r.cond.value());
	}

	return false;
}

void fetch_part1(void) {
	//load address of next instruction into MAR
	if_instruction_mem_addr_bus.IN().pullFrom(if_r.pc);
	instruction_mem.MAR().latchFrom(if_instruction_mem_addr_bus.OUT());
}

void fetch_part2(void) {
	/* read the next instruction from memory into the IR. This may result
		in an issue in edge cases where a HALT instruction has already
		been read from the highest allowed address in memory, at which
		point attempting to read further instructions while the HALT
		progressed to the writeback stage would result in an
		ArchLibError for reading out of bounds from instruction
		memory. We choose to acknowledge this case without handling
		it, as it is unlikely to ever occur. */
	instruction_mem.read();
	ifid_r.ir.latchFrom(instruction_mem.READ());

	/* instruction in decode phase is jump, new PC value is its
		specified destination */
	if(id_instruction_is_jump()) {
		if_r.pc.latchFrom(if_branch_bus.OUT());
	}
	/* instruction in decode phase is branch, new PC value is its
		specified destination */
	else if(id_instruction_is_taken_branch()) {
		if_r.pc.latchFrom(id_imm_alu.OUT());
	}
	else { //not a branch or jump, increment PC
		if_r.pc.perform(Counter::incr4);
	}

	//forward PC value
	if_pc_forward.IN().pullFrom(if_r.pc);
	ifid_r.pc.latchFrom(if_pc_forward.OUT());
	ifid_r.new_pc.latchFrom(if_pc_forward.OUT());

	ifid_r.valid.set();
}

void decode_sign_extend_branch_offset(void) {
	id_imm_alu.OP1().pullFrom(ifid_r.ir);
	id_imm_alu.OP2().pullFrom(id_imm_sign_extend_mask);
	id_imm_alu.perform(BusALU::op_extendSign);
	id_temp_reg.latchFrom(id_imm_alu.OUT());
}

void decode_load_rs_into_temp(void) {
	/* For instructions that use the values they fetch from GPRs before
		they reach the execute stage, we will need to do either
		forwarding or stalling. The implemented instructions that do
		this are all of the variable shift instructions, which
		retrieve the shift amount from a GPR in the decode phase
		(storing it into the 'imm' field of the ID/EX pipeline
		register), and the two jump register instructions
		(JR and JALR) which use the contents of a register as a target
		address. We break this set of instructions into the following
		cases, which we handle as listed:

		If the register being used will be written by the instruction
		in the MEM/WB pipeline register:

			Forward that result from MEM/WB register back to
			ID stage.

		Else, if the register being used will be written by the
		instruction in the EX/MEM pipeline register and that
		instruction is NOT a load (ie: the result that will be written
		is already known):

			Forward that result from EX/MEM register back to ID
			stage.

		Else, if the register being used will be written by the
		instruction in the EX/MEM pipeline register and that
		instruction IS a load (ie: the result that will be written
		will not be known until the end of this cycle):

			Stall the instruction in the IF/ID pipeline register.

		Else, if the register being used will be written by the
		instruction in the ID/EX pipeline register (ie: the result
		that will be written will no be known until the end of this
		cycle):

			Stall the instruction in the IF/ID pipeline register.


	This 'decode_load_rs_into_temp' function handles the above cases where
		forwarding is required, while the stalling cases are handled
		in the 'must_stall_id_phase' function. */



	//get GPR written to by mem stage instruction (if any)
	long mem_stage_gpr = ((exmem_r.valid()) ?
		gpr_written_by_mem_stage_instruction() : 0);
	//get GPR written to by wb stage instruction (if any)
	long wb_stage_gpr = ((memwb_r.valid()) ?
		gpr_written_by_wb_stage_instruction() : 0);

	//if 'rs' written by instrucion in mem stage
	if((mem_stage_gpr) && (mem_stage_gpr == RS(ifid_r.ir))) {
		id_temp_reg_load_bus.IN().pullFrom(exmem_r.c);
	}
	//otherwise, if 'rs' written by instruction in wb stage
	if((wb_stage_gpr) && (wb_stage_gpr == RS(ifid_r.ir))) {
		id_temp_reg_load_bus.IN().pullFrom(memwb_r.c);
	}
	/* no conflict occurs with mem or wb stage instruction
		=> no forwarding needed (conflicts with instructions in
		execute stage are handled by stalling) */
	else {
		//load 'rs' into temp register
		id_temp_reg_load_bus.IN().pullFrom(
			GPR(RS(ifid_r.ir)));
	}

	id_temp_reg.latchFrom(id_temp_reg_load_bus.OUT());
}

long decode_get_branch_rs_value(void) {
	/* In a situation that is very similar to the above
		'decode_load_rs_into_temp' function, branch instructions use
		the value in the 'rs' register in the decode stage to
		determine whether a branch is taken or not. However,
		instructions that come before a branch instruction may change
		the value in this register, so we need to read the value that
		will be written by the most recent instruction that does so.

		This is essentially forwarding, except it forwards values to
		the "control hardware" (ie: the C++ code) rather than the data
		hardware.

		Stalling also must be used in cases where the instruction
		immediately before a branch (or a load instruction up to 2
		instructions before a branch) writes one of the registers it
		uses for its check. See	'must_stall_id_phase' function for
		details. */

	//get GPR written to by mem stage instruction (if any)
	long mem_stage_gpr = ((exmem_r.valid()) ?
		gpr_written_by_mem_stage_instruction() : 0);
	//get GPR written to by wb stage instruction (if any)
	long wb_stage_gpr = ((memwb_r.valid()) ?
		gpr_written_by_wb_stage_instruction() : 0);

	//if 'rs' written by instrucion in mem stage
	if((mem_stage_gpr) && (mem_stage_gpr == RS(ifid_r.ir))) {
		/* return value to be written to 'rs' by instruction in EX/MEM
			pipeline register */
		return exmem_r.c.value();
	}
	//otherwise, if 'rs' written by instruction in wb stage
	if((wb_stage_gpr) && (wb_stage_gpr == RS(ifid_r.ir))) {
		/* return value to be written to 'rs' by instruction in MEM/WB
			pipeline register */
		return memwb_r.c.value();
	}

	/* otherwise, there are no conflicts, and we can just use the existing
		value in 'rs' */
	GPR(RS(ifid_r.ir)).value();
}

long decode_get_branch_rt_value(void) {
	/* This is the same as the above 'decode_get_branch_rs_value'
		function, except for the 'rt' value used by a branch. See the
		comments in that function for details. */

	//get GPR written to by mem stage instruction (if any)
	long mem_stage_gpr = ((exmem_r.valid()) ?
		gpr_written_by_mem_stage_instruction() : 0);
	//get GPR written to by wb stage instruction (if any)
	long wb_stage_gpr = ((memwb_r.valid()) ?
		gpr_written_by_wb_stage_instruction() : 0);

	//if 'rt' written by instrucion in mem stage
	if((mem_stage_gpr) && (mem_stage_gpr == RT(ifid_r.ir))) {
		/* return value to be written to 'rt' by instruction in EX/MEM
			pipeline register */
		return exmem_r.c.value();
	}
	//otherwise, if 'rt' written by instruction in wb stage
	if((wb_stage_gpr) && (wb_stage_gpr == RT(ifid_r.ir))) {
		/* return value to be written to 'rt' by instruction in MEM/WB
			pipeline register */
		return memwb_r.c.value();
	}

	/* otherwise, there are no conflicts, and we can just use the existing
		value in 'rt' */
	return GPR(RT(ifid_r.ir)).value();
}

void decode_part1(void) {
	//only continue if a valid instruction is waiting to be decoded
	if(!ifid_r.valid.value()) {
		return;
	}

	long t1, t2;

	switch(decode_instruction(ifid_r.ir)) {
		//non-variable shift operations
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
			//shift 'sh' field of instruction over to the right
			id_imm_alu.OP1().pullFrom(ifid_r.ir);
			id_imm_alu.OP2().pullFrom(id_sh_field_shift_amount);
			id_imm_alu.perform(BusALU::op_rshift);
			id_temp_reg.latchFrom(id_imm_alu.OUT());
			break;

		//variable shift and jump register operations
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
		case z11::JR:
		case z11::JALR:
			/* load 'rs' (or value that will be written into 'rs'
				by earlier instruction) into temp register */
			decode_load_rs_into_temp();
			break;

		//jump instructions
		case z11::J:
		case z11::JAL:
			//load jump target into temp register
			id_imm_alu.OP1().pullFrom(ifid_r.ir);
			id_imm_alu.OP2().pullFrom(id_jump_target_mask);
			id_imm_alu.perform(BusALU::op_and);
			id_temp_reg.latchFrom(id_imm_alu.OUT());
			break;

		case z11::BEQ:
			//sign extend the branch offset and set 'cond' bit
			decode_sign_extend_branch_offset();
			t1 = decode_get_branch_rs_value();
			t2 = decode_get_branch_rt_value();
//			std::cout << "comparing " << t1 << " and " << t2 << std::endl;
			if(t1 == t2) {
				idex_r.cond.set();
			}
			else {idex_r.cond.clear();}
			ifid_r.new_pc.perform(Counter::incr4);
			break;

		case z11::BNE:
			//sign extend the branch offset and set 'cond' bit
			decode_sign_extend_branch_offset();
			if(decode_get_branch_rs_value() !=
				decode_get_branch_rt_value()) {

				idex_r.cond.set();
			}
			else {idex_r.cond.clear();}
			ifid_r.new_pc.perform(Counter::incr4);
			break;
	}
}

void decode_part2(void) {
	//forward valid bit
	id_valid_forward.IN().pullFrom(ifid_r.valid);
	idex_r.valid.latchFrom(id_valid_forward.OUT());

	//only continue if a valid instruction is waiting to be decoded
	if(!ifid_r.valid.value()) {
		return;
	}

	/* we load the contents of registers 'rs' and 'rt' into A and B no
		matter what here, as the 'rs' and 'rt' fields are only 5 bits
		wide, so they will always refer to a valid general purpose
		register, and, at worst, we can just ignore the value we
		loaded later */

	//load A with contents of register 'rs'
	id_a_load_bus.IN().pullFrom(GPR(RS(ifid_r.ir)));
	idex_r.a.latchFrom(id_a_load_bus.OUT());

	//load B with contents of register 'rt'
	id_b_load_bus.IN().pullFrom(GPR(RT(ifid_r.ir)));
	idex_r.b.latchFrom(id_b_load_bus.OUT());

	//forward IR contents
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

		//masking out 5-bit shift amount
		case z11::SLL:
		case z11::SRL:
		case z11::SRA:
		case z11::SLLV:
		case z11::SRLV:
		case z11::SRAV:
			id_imm_alu.OP1().pullFrom(id_temp_reg);
			id_imm_alu.OP2().pullFrom(id_shift_field_mask);
			id_imm_alu.perform(BusALU::op_and);
			idex_r.imm.latchFrom(id_imm_alu.OUT());
			break;

		//feed jump destination addr into bus
		case z11::J:
		case z11::JAL:
		case z11::JR:
		case z11::JALR:
			if_branch_bus.IN().pullFrom(id_temp_reg);
			break;

		//add branch offset to PC and send result to PC
		case z11::BEQ:
		case z11::BNE:
			if(idex_r.cond.value()) {
				id_imm_alu.OP1().pullFrom(id_temp_reg);
				id_imm_alu.OP2().pullFrom(ifid_r.new_pc);
				id_imm_alu.perform(BusALU::op_add);
			}
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
		case z11::UNKNOWN: //invalid instructions
		default: //valid but unimplemented instructions
			break;
	}

	//forward PC
	id_pc_forward.IN().pullFrom(ifid_r.pc);
	idex_r.pc.latchFrom(id_pc_forward.OUT());
}

long gpr_written_by_mem_stage_instruction(void) {
	z11::op instruction = decode_instruction(exmem_r.ir);

	/* don't need to worry about load instructions here, as the
		instructions that come after them and use the register they
		load into will be stalled */

	if(is_register_alu_instruction(instruction)) {
		return RD(exmem_r.ir);
	}
	else if(is_immediate_alu_instruction(instruction)) {
		return RT(exmem_r.ir);
	}
	else if(instruction == z11::JALR) {
		return RD(exmem_r.ir);
	}
	else if(instruction == z11::JAL) {
		return 31;
	}


	return 0;
}

long gpr_written_by_wb_stage_instruction(void) {
	z11::op instruction = decode_instruction(memwb_r.ir);
	if(is_register_alu_instruction(instruction)) {
		return RD(memwb_r.ir);
	}
	else if(is_immediate_alu_instruction(instruction)) {
		return RT(memwb_r.ir);
	}
	else if(is_load_instruction(instruction)) {
		return RT(memwb_r.ir);
	}
	else if(instruction == z11::JALR) {
		return RD(exmem_r.ir);
	}
	else if(instruction == z11::JAL) {
		return 31;
	}

	return 0;
}

void execute_part1(void) {
	//only continue if a valid instruction is waiting to be executed
	if(!idex_r.valid.value()) {
		return;
	}

	//get instruction we are about to execute
	z11::op instruction = decode_instruction(idex_r.ir);

	//get GPR written to by mem stage instruction (if any)
	long mem_stage_gpr = ((exmem_r.valid()) ?
		gpr_written_by_mem_stage_instruction() : 0);
	//get GPR written to by wb stage instruction (if any)
	long wb_stage_gpr = ((memwb_r.valid()) ?
		gpr_written_by_wb_stage_instruction() : 0);

	/* we assume forwarding is always needed for 'rs' if it conflicts with
		a register being updated by an earlier instruction. This
		assumption works because all of the instructions either need
		forwarding of rs, or don't use the results of 'rs' at all
		(meaning it doesn't change anything to forward anyway) */

	//if 'rs' written by instrucion in mem stage
	if((mem_stage_gpr) && (mem_stage_gpr == RS(idex_r.ir))) {
		//forward from mem stage
		idex_a_fill.IN().pullFrom(exmem_r.c);
		idex_r.a.latchFrom(idex_a_fill.OUT());
	}
	//if 'rs' written by instrucion in wb stage
	else if((wb_stage_gpr) && (wb_stage_gpr == RS(idex_r.ir))) {
		//forward from wb stage
		idex_a_fill.IN().pullFrom(memwb_r.c);
		idex_r.a.latchFrom(idex_a_fill.OUT());
	}

	//if we use 'rt' (ie: we are R-R ALU, store, SLT, or SLTU)
	if(is_register_alu_instruction(instruction) ||
		is_store_instruction(instruction) ||
		//this also captures 'SLTI', but that doesn't change anything
		is_set_if_less_than_instruction(instruction)) {

		//if 'rt' written by instrucion in mem stage
		if((mem_stage_gpr) && (mem_stage_gpr == RT(idex_r.ir))) {
			//forward from mem stage
			idex_b_fill.IN().pullFrom(exmem_r.c);
			idex_r.b.latchFrom(idex_b_fill.OUT());
		}
		//if 'rt' written by instrucion in wb stage
		else if((wb_stage_gpr) && (wb_stage_gpr == RT(idex_r.ir))) {
			//forward from wb stage
			idex_b_fill.IN().pullFrom(memwb_r.c);
			idex_r.b.latchFrom(idex_b_fill.OUT());
		}
	}
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
	//forward valid bit
	ex_valid_forward.IN().pullFrom(idex_r.valid);
	exmem_r.valid.latchFrom(ex_valid_forward.OUT());

	//only continue if a valid instruction is waiting to be executed
	if(!idex_r.valid.value()) {
		return;
	}

	//forward PC
	ex_pc_forward.IN().pullFrom(idex_r.pc);
	exmem_r.pc.latchFrom(ex_pc_forward.OUT());

	//forward IR
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
			if(((int32_t)idex_r.a.value()) <
				((int32_t)idex_r.imm.value())) {

				ex_alu.perform(BusALU::op_one);
			}
			else {
				ex_alu.perform(BusALU::op_zero);
			}
			exmem_r.c.latchFrom(ex_alu.OUT());
			break;
		case z11::SLT:
			if(((int32_t)idex_r.a.value()) <
				((int32_t)idex_r.b.value())) {

				ex_alu.perform(BusALU::op_one);
			}
			else {
				ex_alu.perform(BusALU::op_zero);
			}
			exmem_r.c.latchFrom(ex_alu.OUT());
			break;
		case z11::SLTU:
			if(((uint32_t)idex_r.a.value()) <
				((uint32_t)idex_r.b.value())) {

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

		//jump and link instructions
		case z11::JAL:
		case z11::JALR:
			//compute addr to saved into GPR 31
			ex_alu.OP1().pullFrom(idex_r.pc);
			ex_alu.OP2().pullFrom(ex_jump_link_return_offset);
			exmem_r.c.latchFrom(ex_alu.OUT());
			ex_alu.perform(BusALU::op_add);
			break;

		//do nothing
		case z11::NOP:
		case z11::HALT:
		case z11::J:
		case z11::JR:
		case z11::BEQ:
		case z11::BNE:
		case z11::BREAK:
		case z11::UNKNOWN: //invalid instructions
		default: //valid but unimplemented instructions
			break;
	}
}

void memory_part1(void) {
	//only continue if a valid instruction is waiting to enter mem stage
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
	}
}

void memory_part2(void) {
	//forward valid bit
	mem_valid_forward.IN().pullFrom(exmem_r.valid);
	memwb_r.valid.latchFrom(mem_valid_forward.OUT());

	//only continue if a valid instruction is waiting to enter mem stage
	if(!exmem_r.valid.value()) {
		return;
	}

	//forward PC
	mem_pc_forward.IN().pullFrom(exmem_r.pc);
	memwb_r.pc.latchFrom(mem_pc_forward.OUT());

	//forward IR
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
		case z11::JAL:
		case z11::JALR:
			//just forward result through
			mem_c_forward.IN().pullFrom(exmem_r.c);
			memwb_r.c.latchFrom(mem_c_forward.OUT());
			break;

		//load instruction
		case z11::LW:
			//read from memory
			data_mem.read();
			memwb_r.c.latchFrom(data_mem.READ());
			break;

		//store instruction
		case z11::SW:
			//write to memory
			data_mem.write();
			data_mem.WRITE().pullFrom(exmem_r.b);
			break;

		//do nothing cases
		case z11::NOP:
		case z11::HALT:
		case z11::J:
		case z11::JR:
		case z11::BEQ:
		case z11::BNE:
		case z11::BREAK:
		case z11::UNKNOWN: //invalid instructions
		default: //valid but unimplemented instructions
			break;
	}
}

void writeback_part1(void) {
	//forward valid bit
	wb_valid_forward.IN().pullFrom(memwb_r.valid);
	post_wb_r.valid.latchFrom(wb_valid_forward.OUT());

	//only continue if a valid instruction is waiting to enter wb stage
	if(!memwb_r.valid.value()) {
		return;
	}

	//forward PC
	wb_pc_forward.IN().pullFrom(memwb_r.pc);
	post_wb_r.pc.latchFrom(wb_pc_forward.OUT());

	//forward IR
	wb_ir_forward.IN().pullFrom(memwb_r.ir);
	post_wb_r.ir.latchFrom(wb_ir_forward.OUT());

	switch(decode_instruction(memwb_r.ir)) {
		//immediate ALU instructions
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
			//write result to 'rt'
			writeback_to_GPR(RT(memwb_r.ir), memwb_r.c);
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
		case z11::JALR:
			//write result to 'rd'
			writeback_to_GPR(RD(memwb_r.ir), memwb_r.c);
			break;

		//load instructions
		case z11::LW:
			//write result to 'rt'
			writeback_to_GPR(RT(memwb_r.ir), memwb_r.c);
			break;

		//instructions that lead to halting
		case z11::HALT:
		case z11::UNKNOWN: //invalid instructions
		default: //valid but unimplemented instructions
			halted = true;
			break;

		//JAL writes return addr to r31
		case z11::JAL:
			writeback_to_GPR(31, memwb_r.c);
			break;

		//do nothing cases
		case z11::NOP:
		case z11::SW:
		case z11::J:
		case z11::JR:
		case z11::BEQ:
		case z11::BNE:
		case z11::BREAK:
			break;
	}
}

void writeback_to_GPR(int gpr_num, StorageObject &src) {
	//r0 can't be overwritten
	if(gpr_num == 0) {return;}

	wb_register_write_bus.IN().pullFrom(src);
	GPR(gpr_num).latchFrom(wb_register_write_bus.OUT());
}

void writeback_part2(void) {}

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
	/* only continue if the instruction that just finished the writeback
		stage was a valid one */
	if(!post_wb_r.valid.value()) {
		return;
	}

	//print address of instruction
	std::cout << std::hex << std::setw(8) << std::setfill('0') <<
		post_wb_r.pc.value() << ":  ";

	//print hex opcode value
	std::cout << std::hex << std::setw(2) << std::setfill('0') <<
		post_wb_r.ir(31, 26);

	z11::op operation = decode_instruction(post_wb_r.ir);

	//print 'funct' value if it was a special instruction
	if(is_special_instruction(post_wb_r.ir)) {
		std::cout << " " << std::hex << std::setw(2) <<
			std::setfill('0') << post_wb_r.ir(5, 0);
	}
	else {
		std::cout << "   ";
	}

	//print mnemonic
	std::cout << " " << std::left << std::setw(7) << std::setfill(' ') <<
		z11::mnemonics[operation] << std::right;


	//print any overwritten registers and print info for breaks
	switch(operation) {
		//immediate ALU instructions write 'rt'
		case z11::ADDI:
		case z11::SLTI:
		case z11::ANDI:
		case z11::ORI:
		case z11::XORI:
		case z11::LUI:
		case z11::LW:
			std::cout << " " << std::hex <<
				GPR(RT(post_wb_r.ir));
			break;

		//register-register ALU instructions write 'rd'
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
		case z11::JALR:
			std::cout << " " << std::hex <<
				GPR(RD(post_wb_r.ir));
			break;

		case z11::BREAK:
			print_break_information();
			break;

		//JAL instructions write r31
		case z11::JAL:
			std::cout << " " << std::hex << GPR(31);
			break;

		//do nothing cases
		case z11::HALT:
		case z11::NOP:
		case z11::SW:
		case z11::J:
		case z11::JR:
		case z11::BEQ:
		case z11::BNE:
		case z11::UNKNOWN: //invalid instructions
		default: //valid but unimplemented instructions
			break;
	}

	std::cout << std::endl;

	//print halt message (if necessary)
	if(halted) {
		std::cout << "Machine Halted - ";
		switch(operation) {
			case z11::HALT:
				std::cout << "HALT instruction executed";
				break;
			case z11::UNKNOWN:
				std::cout << "undefined instruction";
				break;
			default:
				std::cout << "unimplemented instruction";
				break;
		}

		std::cout << std::endl;
	}
}


bool must_stall_id_phase_due_to_load_in_idex_register(void) {
	z11::op ifid_ins = decode_instruction(ifid_r.ir);

	//if load instruction in ID/EX (immediately/"one place" ahead)
	if(idex_r.valid.value() &&
		is_load_instruction(decode_instruction(idex_r.ir))) {

		/* if IF/ID instruction uses contents of 'rs' (ie: is an
			R-R ALU, ALU imm, load, store, branch, JR, JALR) */
		if(is_register_alu_instruction(ifid_ins) ||
			is_immediate_alu_instruction(ifid_ins) ||
			is_load_instruction(ifid_ins) ||
			is_store_instruction(ifid_ins) ||
			is_branch_instruction(ifid_ins) ||
			is_jump_register_instruction(ifid_ins)) {

			/* check the load's 'rt' against the other
				instruction's 'rs' */
			if(RT(idex_r.ir) == RS(ifid_r.ir)) {
				//if the same, stall
				return true;
			}
		}

		/* if IF/ID instruction uses 'rt' (ie: is R-R ALU, branch, or
			store): */
		if(is_register_alu_instruction(ifid_ins) ||
			is_branch_instruction(ifid_ins) ||
			is_store_instruction(ifid_ins)) {
			/* check the load's 'rt' against the other
				instruction's 'rt' */
			if(RT(idex_r.ir) == RT(ifid_r.ir)) {
				//if the same, stall
				return true;
			}
		}
	}

	return false;
}

bool must_stall_id_phase_due_to_load_in_exmem_register(void) {
	z11::op ifid_ins = decode_instruction(ifid_r.ir);

	//if load instruction in EX/MEM ("two places" ahead)
	if(exmem_r.valid.value() &&
		is_load_instruction(decode_instruction(exmem_r.ir))) {

		/* if IF/ID instruction uses contents of 'rs' register in
			decode stage (ie: is a branch, jump register, or
			shift variable) */
		if(is_branch_instruction(ifid_ins) ||
			is_jump_register_instruction(ifid_ins) ||
			is_variable_shift_instruction(ifid_ins)) {

			/* check the load's 'rt' against the other
				instructions 'rs' */
			if(RT(exmem_r.ir) == RS(ifid_r.ir)) {
				//if the same, stall
				return true;
			}
		}

		/* if IF/ID uses contents of 'rt' register in decode stage
			(ie: is a branch) */
		if(is_branch_instruction(decode_instruction(ifid_r.ir))) {
			/* check the load's 'rt' against the other
				instructions 'rt' */
			if(RT(exmem_r.ir) == RT(ifid_r.ir)) {
				//if the same, stall
				return true;
			}
		}
	}

	return false;
}

long gpr_written_by_ex_stage_instruction(void) {
	z11::op instruction = decode_instruction(idex_r.ir);

	if(is_load_instruction(instruction)) {
		return RT(idex_r.ir);
	}
	if(is_register_alu_instruction(instruction)) {
		return RD(idex_r.ir);
	}
	else if(is_immediate_alu_instruction(instruction)) {
		return RT(idex_r.ir);
	}
	else if(instruction == z11::JALR) {
		return RD(idex_r.ir);
	}
	else if(instruction == z11::JAL) {
		return 31;
	}

	return 0;
}

bool must_stall_id_phase_to_use_result_in_id_phase(void) {
	z11::op ifid_ins = decode_instruction(ifid_r.ir);

	//get GPR written to by ex stage instruction (if any)
        long ex_stage_gpr = ((idex_r.valid()) ?
                gpr_written_by_ex_stage_instruction() : 0);

	/* if instruction in IF/ID pipeline register uses contents of 'rs' in
		branch stage */
	if(is_branch_instruction(ifid_ins) ||
		is_jump_register_instruction(ifid_ins) ||
		is_variable_shift_instruction(ifid_ins)) {

		/* and IF/ID instruction's 'rs' matched ID/EX instruction's
			written GPR */
		if((ex_stage_gpr) && (ex_stage_gpr == RS(ifid_r.ir))) {
			/* we will have to wait until execution stage is done
				in order to use its results */
				return true;
		}
	}

	/* if instruction in IF/ID pipeline register uses contents of 'rt' in
		branch stage (ie: is a branch instruction) */
	if(is_branch_instruction(ifid_ins)) {
		/* and IF/ID instruction's 'rt' matched ID/EX instruction's
			written GPR */
		if((ex_stage_gpr) && (ex_stage_gpr == RT(ifid_r.ir))) {
			/* we will have to wait until execution stage is done
				in order to use its results */
				return true;
		}
	}

	return false;
}

bool must_stall_id_phase(void) {
	//only continue if a valid instruction is waiting to be decoded
	if(!ifid_r.valid.value()) {
		return false;
	}

	//if any of these cases happen, we need to stall
	return (must_stall_id_phase_due_to_load_in_idex_register() ||
		must_stall_id_phase_due_to_load_in_exmem_register() ||
		must_stall_id_phase_to_use_result_in_id_phase());
}

void insert_nop_into_idex_reg(void) {
	/* insert a NOP into the ID/EX register and give it the correct
		address */
	idex_r.pc.perform(Counter::incr4);

	idex_nop_insert_bus.IN().pullFrom(stalling_nop_constant);
	idex_r.ir.latchFrom(idex_nop_insert_bus.OUT());
}

void run_program(void) {
	//initial load of entry point into PC
	bootstrap_program();

	while(!halted) {
		//determine if we need to stall this cycle
		bool stall_id_phase = must_stall_id_phase();

		/* first clock tick of cycle */

			//stall fetch and decode phases if necessary
			if(!stall_id_phase) {
				fetch_part1();
				decode_part1();
			}

			execute_part1();
			memory_part1();
			writeback_part1();
			Clock::tick();

		/* second clock tick of cycle */

			//stall fetch and decode phases if necessary
			if(!stall_id_phase) {
				fetch_part2();
				decode_part2();
			}
			else {
				//example solution does stall by inserting NOP
				insert_nop_into_idex_reg();
			}

			execute_part2();
			memory_part2();
			writeback_part2();
			Clock::tick();


		//print instruction trace
		print_execution_record();
	}
}
