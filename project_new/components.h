/**
 * Header file for "components" module that defines all of the hardware
 * components of the z88 CPU.
 *
 * Authors: Coleman Link and Ben Maitland
 */

#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

//arch library includes
#include <StorageObject.h>
#include <Clearable.h>
#include <Memory.h>
#include <Bus.h>
#include <BusALU.h>
#include <Counter.h>

/**
 * A pipeline register that is "positioned" before the instruction fetch
 * stage. This register holds only the "real" program counter (the one that
 * is actually used for loading the instruction memory's MAR).
 */
class if_reg {
	public:
		if_reg(void);

		Counter pc;
};

/**
 * IF/ID pipeline register.
 */
class ifid_reg {
	public:
		ifid_reg(void);

		/* bit that determines whether this register currently holds a
			valid instruction (used when filling and draining the
			pipeline) */
		Clearable valid;
		/* storage for memory address the instruction in the IF/ID
			register was pulled from */
		StorageObject pc;
		/* storage for a duplicate of the aforementioned memory
			address that can be incremented while performing
			branch address calculations without causing errors in
			the instruction trace */
		Counter new_pc;
		//storage for the instruction itself
		StorageObject ir;
};

/**
 * ID/EX pipeline register.
 */
class idex_reg {
	public:
		idex_reg(void);

		/* bit that determines whether this register currently holds a
			valid instruction (used when filling and draining the
			pipeline) */
		Clearable valid;
		/* storage for memory address the instruction in the ID/EX
			register was pulled from. Declared as a counter
			because it will need to be incremented when inserting
			a stall */
		Counter pc;
		//storage for the instruction itself
		StorageObject ir;
		/* storage for the contents of the 'rs' register specified
			by a decoded instruction */
		StorageObject a;
		/* storage for the contents of the 'rt' register specified
			by a decoded instruction */
		StorageObject b;
		/* storage for an immediate value specified in a decoded
			instruction */
		StorageObject imm;
		/* condition bit that is set if the branch specified by a
			decoded branch instruction should be taken */
		Clearable cond;
};

/**
 * EX/MEM pipeline register.
 */
class exmem_reg {
	public:
		exmem_reg(void);

		/* bit that determines whether this register currently holds a
			valid instruction (used when filling and draining the
			pipeline) */
		Clearable valid;
		/* storage for memory address the instruction in the EX/MEM
			register was pulled from. */
		StorageObject pc;
		//storage for the instruction itself
		StorageObject ir;
		/* storage for the contents of the 'rt' register specified
			by a decoded instruction, if it needs to be passed
			through the execute phase */
		StorageObject b;
		/* storage for the results of calculations done during the
			execute phase. */
		StorageObject c;
};

/**
 * MEM/WB pipeline register.
 */
class memwb_reg {
	public:
		memwb_reg(void);

		/* bit that determines whether this register currently holds a
			valid instruction (used when filling and draining the
			pipeline) */
		Clearable valid;
		/* storage for memory address the instruction in the MEM/WB
			register was pulled from. */
		StorageObject pc;
		//storage for the instruction itself
		StorageObject ir;
		/* storage for the results of calculations done during the
			execute phase, if they need to be passed through the
			memory phase, or for data read in from data memory. */
		StorageObject c;
};

/**
 * A pipeline register that is "positioned" after the writeback stage. Its
 * contents are used for printing the instruction trace information without
 * needing to spilt the trace printing into pre-second-tick and
 * post-second-tick sections. In particular, within the instruction trace, we
 * need to print out the new contents of any modified registers, but also the
 * address and mnemonic for the instruction. The results written to a register
 * will not be available until after the second tick of the cycle, but, after
 * this tick, the contents of the MEM/WB pipeline resgiter will be overwritten
 * by the information for the next instruction. Thus, we forward the existing
 * instruction to this register, where it can safely wait to be printed out
 * after it has finished executing.
 */
class post_wb_reg {
	public:
		post_wb_reg(void);

		/* bit that determines whether this register currently holds a
			valid instruction (used when filling and draining the
			pipeline) */
		Clearable valid;
		//storage for the instruction itself
		StorageObject ir;
		/* storage for memory address the instruction in the MEM/WB
			register was pulled from. */
		StorageObject pc;
};


//size of a register/standard memory unit (in bits)
extern const unsigned int WORD_WIDTH;
//number of general purpose registers available
extern const unsigned int NUM_GPRS;
//the size of an address (in bits)
extern const unsigned int ADDR_WIDTH;
//the size of the smallest addressable unit of memory (int bits)
extern const unsigned int UNIT_BITS;
//the highest allowed address in memory
extern const unsigned int MAX_ADDR;

//pipeline register for fetch stage
extern if_reg if_r;
//IF/ID pipeline register
extern ifid_reg ifid_r;
//ID/EX pipeline register
extern idex_reg idex_r;
//EX/MEM pipeline register
extern exmem_reg exmem_r;
//MEM/WB pipeline register
extern memwb_reg memwb_r;
//special post-WB pipeline register for instruction trace printouts
extern post_wb_reg post_wb_r;

//the general puspose registers and a macro to access them more easily
extern StorageObject *gprs[];
#define GPR(x) (*(gprs[x]))

//instruction memory
extern Memory instruction_mem;
//data memory
extern Memory data_mem;


//fetch stage busses, ALUs, temporary registers, and constants

	//bus for loading PC contents into instruction memory MAR
	extern Bus if_instruction_mem_addr_bus;
	//bus for forwarding PC contents into IF/ID register
	extern Bus if_pc_forward;
	//bus for loading destainations of branch instructions into PC
	extern Bus if_branch_bus;


//decode stage busses, ALUs, temporary registers, and constants

	//bus for forwarding valid bit through ID stage
	extern Bus id_valid_forward;
	//bus for forwarding PC through ID stage
	extern Bus id_pc_forward;
	//bus for forwarding IR through ID stage
	extern Bus id_ir_forward;
	//bus for loading A register with contents of GPR specified in 'rs'
	extern Bus id_a_load_bus;
	//bus for loading B register with contents of GPR specified in 'rt'
	extern Bus id_b_load_bus;
	//ALU for decode stage, performs a wide variety of operations
	extern BusALU id_imm_alu;
	//constant used for sign extending immediate value from instruction
	extern StorageObject id_imm_sign_extend_mask;
	//constant used for zero extending immediate value from instruction
	extern StorageObject id_imm_zero_extend_mask;
	/* constant used for shifting when extracting the 'sh' field from a
		shift instruction */
	extern StorageObject id_sh_field_shift_amount;
	/* temporary register for use in decode stage. holds intermediate
		results between the first and second clock ticks of the
		decode stage for certain instructions. In particular, shift
		instructions require a temporary value while extracting the
		'sh' field from their instruction, and jump and branch
		instructions require intermediate values when computing
		destination addresses */
	extern StorageObject id_temp_reg;
	//bus for loading values into aforementioned temporary register
	extern Bus id_temp_reg_load_bus;
	/* constant mask for extracting the contents of the 'sh' field for
		shift instructions */
	extern StorageObject id_shift_field_mask;
	/* constant mask for extracting the target address from jump
		instructions */
	extern StorageObject id_jump_target_mask;


//execute stage busses, ALUs, temporary registers, and constants

	//bus for forwarding valid bit through ID stage
	extern Bus ex_valid_forward;
	//bus for forwarding PC through ID stage
	extern Bus ex_pc_forward;
	//bus for forwarding IR through ID stage
	extern Bus ex_ir_forward;
	/* bus for forwarding contents of GPR specified in 'rt' through the
		EX stage */
	extern Bus ex_b_forward;
	//primary ALU for executing instructions
	extern BusALU ex_alu;
	/* constant amount to shift immediate values by when executing LUI
		instructions */
	extern StorageObject ex_lui_shift_amount;
	/* constant offset to add to PC when saving the return address for JAL
		and JALR instructions */
	extern StorageObject ex_jump_link_return_offset;


//memory stage busses, ALUs, temporary registers, and constants

	//bus for forwarding valid bit through MEM stage
	extern Bus mem_valid_forward;
	//bus for forwarding PC through MEM stage
	extern Bus mem_pc_forward;
	//bus for forwarding IR through MEM stage
	extern Bus mem_ir_forward;
	/* bus for forwarding the results of ALU computations through MEM
		stage */
	extern Bus mem_c_forward;
	//bus for loading addresses into the data memory's MAR
	extern Bus mem_data_mem_addr_bus;


//writeback stage busses, ALUs, temporary registers, and constants

	//bus used for writing results into general purpose registers
	extern Bus wb_register_write_bus;
	//bus for forwarding PC through WB stage
	extern Bus wb_pc_forward;
	//bus for forwarding IR through WB stage
	extern Bus wb_ir_forward;
	//bus for forwarding valid bit through WB stage
	extern Bus wb_valid_forward;


//busses used to implement forwarding

	//bus for forwarding data that will be written to 'rs' register
	extern Bus idex_a_fill;
	//bus for forwarding data that will be writtent to 'rt' register
	extern Bus idex_b_fill;


//busses and constants used to implement stalling

	/* bus used to fill in the ID/EX pipeline registers IR field with a
		NOP instruction */
	extern Bus idex_nop_insert_bus;
	//constant holding the opcode for a NOP instruction
	extern StorageObject stalling_nop_constant;

#endif // _COMPONENTS_H_
