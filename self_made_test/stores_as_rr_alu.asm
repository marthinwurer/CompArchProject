	.org	0x50
one:	.word	0x00000000
;
	.org	0x100
	.entry	main
main:
	xori	r5,r0,#0x90
	nop
	nop
	nop
	nop
	xori	r5,r0,#0xff
	xori	r5,r0,#0xee
	xori	r5,r0,#0xdd
	sw	r5,0x50(r0)
	nop
	nop
	nop
	nop
	lw	r7,0x50(r0)
	nop
	nop
	nop
	nop
;
	break
	halt
