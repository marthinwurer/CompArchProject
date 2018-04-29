	.org	0x100
	.entry	main
main:
	xori	r20,r0,#0x5
	xori	r21,r0,#0xa
	xori	r22,r0,#0x64
	xori	r23,r0,#0x2
	nop
	nop
	nop
	nop
	nop
	break
	add	r22,r20,r21
	add	r24,r23,r22
;
	break
	halt
