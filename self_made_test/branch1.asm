	.org	0x50
	.entry	main
main:
	xori	r1,r0,#0x2
	xori	r2,r0,#0x3
	xori	r3,r0,#0x5
	nop
	nop
	nop
	nop
	add	r4,r1,r2
	nop
	nop
	nop
	beq	r3,r4,asdf
	sub	r20,r1,r0
	sub	r21,r1,r0
	sub	r22,r1,r0
	sub	r23,r1,r0
asdf:
	add	r24,r1,r3
	add	r25,r1,r3
	add	r26,r1,r3
	add	r27,r1,r3
	add	r28,r1,r3
	nop
	nop
	nop
	nop
	break
	halt
