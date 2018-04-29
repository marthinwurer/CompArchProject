	.org	0x100
	.entry	main
main:
	xori	r2,r0,#0x5
	xori	r1,r0,#0xff
	nop
	nop
	nop
	nop
	sll	r20,r1,#0x8
	add	r29,r20,r0
	nop
	nop
	nop
	nop
	break
	halt
