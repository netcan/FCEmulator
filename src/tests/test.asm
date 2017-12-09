.segment "CHARS"
    ;.incbin "chr.bin" ; if you have one
.segment "HEADER"
    .byte "NES",26,2,1 ; 32K PRG, 8K CHR
.segment "VECTORS"
    .word 0, 0, 0, nmi, reset, irq
.segment "STARTUP" ; avoids warning
.segment "CODE"

irq:
nmi:
reset:
	sed
	cld
	asl ; A << 1
	asl $66 ; ZeroPage addressing
	asl $ff,X ; ZeroPageX addressing
	asl $6666 ; Absolute addressing
	asl $6666,X ; Absolute,X addressing

