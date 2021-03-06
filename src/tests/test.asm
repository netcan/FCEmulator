.segment "CHARS"
    ;.incbin "chr.bin" ; if you have one
.segment "HEADER"
    .byte "NES",26,2,1 ; 32K PRG, 8K CHR
.segment "VECTORS"
    .word 0, 0, 0, nmi, reset, irq
.segment "STARTUP" ; avoids warning
.segment "CODE"

irq:
    rti
nmi:
reset:
	sed
	cld

	asl ; A << 1
	asl $66 ; ZeroPage addressing
	asl $ff,X ; ZeroPageX addressing
	asl $6666 ; Absolute addressing
	asl $6666,X ; Absolute,X addressing

	clc
	cli
	clv

	dex
	dex

	inx
	inx

	dey
	dey

	iny
	iny

	lsr
	lsr $6666

	nop

	rol
	rol $66
	rol $66

    ror
    ror $60,X

    sec
    sei

    tax
    tay
    tsx
    txa
    txs
    tya

    pha
    php

    pla
    plp

    brk

bcc_test:   nop
            bcc bcc_test

    nop
    bcs *-1

    nop
    beq *-1

    nop
    bmi *-1

    nop
    bne *-1

    nop
    bpl *-1

    nop
    bvc *-1

    nop
    bvs *-1

    adc #$ff
    adc $66ff,Y ; Absolute,Y addressing
    adc ($66,X) ; IndexIndirect(IDX) addressing
    adc ($66),Y ; IndirectIndex(IDY) addressing

    and $6600,Y

    cmp #$ff
    cpx #$ff
    cpy #$00

    eor #$00

    lda #$23
    ldx $60,Y  ; ZeroPage Y addressing
    ldy #$aa

    ora #$ab

    sbc #$ff

    bit $66

    sta $23
    stx $23
    sty $23

    dec $23
    inc $23

    jsr jsr_test
    jmp after_jsr


jsr_test:
    nop
    rts
after_jsr:
    nop
    lda #$20
    sta $2006
    lda #$18
    sta $2006
    lda $2007 ; discard the first read
    lda $2007

    lda #$80
    sta $2002 ; set 2002.7
    lda $2002
    lda $2002
