#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include <StorageObject.h>
#include <Clearable.h>
#include <Memory.h>
#include <Bus.h>
#include <BusALU.h>
#include <Counter.h>

class if_reg {
	public:
		if_reg(void);

		Counter pc;
};

class ifid_reg {
	public:
		ifid_reg(void);

		Clearable valid;
		StorageObject pc;
		Counter new_pc;
		StorageObject ir;
};

class idex_reg {
	public:
		idex_reg(void);

		Clearable valid;
		Counter pc;
		StorageObject ir;
		StorageObject a;
		StorageObject b;
		StorageObject imm;
		Clearable cond;
};

class exmem_reg {
	public:
		exmem_reg(void);

		Clearable valid;
		StorageObject pc;
		StorageObject ir;
		StorageObject b;
		StorageObject c;
};

class memwb_reg {
	public:
		memwb_reg(void);

		Clearable valid;
		StorageObject pc;
		StorageObject ir;
		StorageObject c;
		StorageObject memory_data;
};

class post_wb_reg {
	public:
		post_wb_reg(void);

		Clearable valid;
		StorageObject ir;
		StorageObject pc;
};


extern const unsigned int WORD_WIDTH;
extern const unsigned int NUM_GPRS;
extern const unsigned int ADDR_WIDTH;
extern const unsigned int UNIT_BITS;
extern const unsigned int MAX_ADDR;

extern if_reg if_r;
extern ifid_reg ifid_r;
extern idex_reg idex_r;
extern exmem_reg exmem_r;
extern memwb_reg memwb_r;
extern post_wb_reg post_wb_r;

extern StorageObject *gprs[];
#define GPR(x) (*(gprs[x]))

extern Memory instruction_mem;
extern Memory data_mem;

//if stage busses and ALUs
//extern BusALU if_pc_incrementer;
extern Bus if_instruction_mem_addr_bus;
extern Bus if_pc_forward;
extern Bus if_branch_bus;

//id stage busses and ALUs
extern Bus id_valid_forward;
extern Bus id_pc_forward;
extern Bus id_ir_forward;
extern Bus id_a_load_bus;
extern Bus id_b_load_bus;
extern BusALU id_imm_alu;
extern StorageObject id_imm_sign_extend_mask;
extern StorageObject id_imm_zero_extend_mask;
extern StorageObject id_sh_field_shift_amount;
extern StorageObject id_shift_temp_reg;
extern Bus id_shift_temp_reg_load_bus;
extern StorageObject id_shift_field_mask;
extern StorageObject id_jump_target_mask;

//ex stage busses and ALUs
extern Bus ex_valid_forward;
extern Bus ex_pc_forward;
extern Bus ex_ir_forward;
extern Bus ex_b_forward;
extern BusALU ex_alu;
extern StorageObject ex_lui_shift_amount;
extern StorageObject ex_jump_link_return_offset;

//mem stage busses and ALUs
extern Bus mem_valid_forward;
extern Bus mem_pc_forward;
extern Bus mem_ir_forward;
extern Bus mem_c_forward;
extern Bus mem_data_mem_addr_bus;

//wb stage busses and ALUs
extern Bus wb_register_write_bus;
extern Bus wb_pc_forward;
extern Bus wb_ir_forward;
extern Bus wb_valid_forward;

//forwarding busses
extern Bus idex_a_fill;
extern Bus idex_b_fill;

//stalling hardware
extern Bus idex_nop_insert_bus;
extern StorageObject stalling_nop_constant;

#endif // _COMPONENTS_H_
