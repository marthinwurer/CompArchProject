	.org	0x50
one:	.word	0x0000ffff
;
	.org	0x100
	.entry	main
main:
	lw	r1,0x50(r0)
	add	r2,r1,r0
	nop
	nop
	nop
	nop
	break
	halt
