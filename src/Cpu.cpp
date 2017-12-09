/****************************************************************************
 > File Name: cpu.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/
#include "Cpu.h"

ProcessorStatus::ProcessorStatus(uint8_t value): Negative(this->P), Overflow(this->P),
                                    BrkExecuted(this->P), Decimal(this->P),
                                    IrqDisabled(this->P), Zero(this->P),
                                    Carry(this->P) {
	P = value; // 初值
}


const Operation **CPU::optable = CPU::InitOptable();


const Operation ** CPU::InitOptable() {
	static const Operation op_entity[] = {
			{ 0x69, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_ADC },
			{ 0x65, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_ADC },
			{ 0x75, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_ADC },
			{ 0x6D, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_ADC },
			{ 0x7D, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_ADC },
			{ 0x79, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_ADC },
			{ 0x61, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_ADC },
			{ 0x71, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_ADC },
			{ 0x29, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_AND },
			{ 0x25, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_AND },
			{ 0x35, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_AND },
			{ 0x2D, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_AND },
			{ 0x3D, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_AND },
			{ 0x39, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_AND },
			{ 0x21, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_AND },
			{ 0x31, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_AND },
			{ 0x0A, OpAddressingMode::   Accumulator, 1, 2, 0, CPU::OP_ASL },
			{ 0x06, OpAddressingMode::      ZeroPage, 2, 5, 0, CPU::OP_ASL },
			{ 0x16, OpAddressingMode::     ZeroPageX, 2, 6, 0, CPU::OP_ASL },
			{ 0x0E, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_ASL },
			{ 0x1E, OpAddressingMode::     AbsoluteX, 3, 7, 0, CPU::OP_ASL },
			{ 0x90, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BCC },
			{ 0xB0, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BCS },
			{ 0xF0, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BEQ },
			{ 0x24, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_BIT },
			{ 0x2C, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_BIT },
			{ 0x30, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BMI },
			{ 0xD0, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BNE },
			{ 0x10, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BPL },
			{ 0x00, OpAddressingMode::      Implicit, 1, 7, 0, CPU::OP_BRK },
			{ 0x50, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BVC },
			{ 0x70, OpAddressingMode::      Relative, 2, 2, 2, CPU::OP_BVS },
			{ 0x18, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_CLC },
			{ 0xD8, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_CLD },
			{ 0x58, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_CLI },
			{ 0xB8, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_CLV },
			{ 0xC9, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_CMP },
			{ 0xC5, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_CMP },
			{ 0xD5, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_CMP },
			{ 0xCD, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_CMP },
			{ 0xDD, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_CMP },
			{ 0xD9, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_CMP },
			{ 0xC1, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_CMP },
			{ 0xD1, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_CMP },
			{ 0xE0, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_CPX },
			{ 0xE4, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_CPX },
			{ 0xEC, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_CPX },
			{ 0xC0, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_CPY },
			{ 0xC4, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_CPY },
			{ 0xCC, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_CPY },
			{ 0xC6, OpAddressingMode::      ZeroPage, 2, 5, 0, CPU::OP_DEC },
			{ 0xD6, OpAddressingMode::     ZeroPageX, 2, 6, 0, CPU::OP_DEC },
			{ 0xCE, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_DEC },
			{ 0xDE, OpAddressingMode::     AbsoluteX, 3, 7, 0, CPU::OP_DEC },
			{ 0xCA, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_DEX },
			{ 0x88, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_DEY },
			{ 0x49, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_EOR },
			{ 0x45, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_EOR },
			{ 0x55, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_EOR },
			{ 0x4D, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_EOR },
			{ 0x5D, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_EOR },
			{ 0x59, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_EOR },
			{ 0x41, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_EOR },
			{ 0x51, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_EOR },
			{ 0xE6, OpAddressingMode::      ZeroPage, 2, 5, 0, CPU::OP_INC },
			{ 0xF6, OpAddressingMode::     ZeroPageX, 2, 6, 0, CPU::OP_INC },
			{ 0xEE, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_INC },
			{ 0xFE, OpAddressingMode::     AbsoluteX, 3, 7, 0, CPU::OP_INC },
			{ 0xE8, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_INX },
			{ 0xC8, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_INY },
			{ 0x4C, OpAddressingMode::      Absolute, 3, 3, 0, CPU::OP_JMP },
			{ 0x6C, OpAddressingMode::      Indirect, 3, 5, 0, CPU::OP_JMP },
			{ 0x20, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_JSR },
			{ 0xA9, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_LDA },
			{ 0xA5, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_LDA },
			{ 0xB5, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_LDA },
			{ 0xAD, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_LDA },
			{ 0xBD, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_LDA },
			{ 0xB9, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_LDA },
			{ 0xA1, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_LDA },
			{ 0xB1, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_LDA },
			{ 0xA2, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_LDX },
			{ 0xA6, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_LDX },
			{ 0xB6, OpAddressingMode::     ZeroPageY, 2, 4, 0, CPU::OP_LDX },
			{ 0xAE, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_LDX },
			{ 0xBE, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_LDX },
			{ 0xA0, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_LDY },
			{ 0xA4, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_LDY },
			{ 0xB4, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_LDY },
			{ 0xAC, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_LDY },
			{ 0xBC, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_LDY },
			{ 0x4A, OpAddressingMode::   Accumulator, 1, 2, 0, CPU::OP_LSR },
			{ 0x46, OpAddressingMode::      ZeroPage, 2, 5, 0, CPU::OP_LSR },
			{ 0x56, OpAddressingMode::     ZeroPageX, 2, 6, 0, CPU::OP_LSR },
			{ 0x4E, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_LSR },
			{ 0x5E, OpAddressingMode::     AbsoluteX, 3, 7, 0, CPU::OP_LSR },
			{ 0xEA, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_NOP },
			{ 0x09, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_ORA },
			{ 0x05, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_ORA },
			{ 0x15, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_ORA },
			{ 0x0D, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_ORA },
			{ 0x1D, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_ORA },
			{ 0x19, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_ORA },
			{ 0x01, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_ORA },
			{ 0x11, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_ORA },
			{ 0x48, OpAddressingMode::      Implicit, 1, 3, 0, CPU::OP_PHA },
			{ 0x08, OpAddressingMode::      Implicit, 1, 3, 0, CPU::OP_PHP },
			{ 0x68, OpAddressingMode::      Implicit, 1, 4, 0, CPU::OP_PLA },
			{ 0x28, OpAddressingMode::      Implicit, 1, 4, 0, CPU::OP_PLP },
			{ 0x2A, OpAddressingMode::   Accumulator, 1, 2, 0, CPU::OP_ROL },
			{ 0x26, OpAddressingMode::      ZeroPage, 2, 5, 0, CPU::OP_ROL },
			{ 0x36, OpAddressingMode::     ZeroPageX, 2, 6, 0, CPU::OP_ROL },
			{ 0x2E, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_ROL },
			{ 0x3E, OpAddressingMode::     AbsoluteX, 3, 7, 0, CPU::OP_ROL },
			{ 0x6A, OpAddressingMode::   Accumulator, 1, 2, 0, CPU::OP_ROR },
			{ 0x66, OpAddressingMode::      ZeroPage, 2, 5, 0, CPU::OP_ROR },
			{ 0x76, OpAddressingMode::     ZeroPageX, 2, 6, 0, CPU::OP_ROR },
			{ 0x6E, OpAddressingMode::      Absolute, 3, 6, 0, CPU::OP_ROR },
			{ 0x7E, OpAddressingMode::     AbsoluteX, 3, 7, 0, CPU::OP_ROR },
			{ 0x40, OpAddressingMode::      Implicit, 1, 6, 0, CPU::OP_RTI },
			{ 0x60, OpAddressingMode::      Implicit, 1, 6, 0, CPU::OP_RTS },
			{ 0xE9, OpAddressingMode::     Immediate, 2, 2, 0, CPU::OP_SBC },
			{ 0xE5, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_SBC },
			{ 0xF5, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_SBC },
			{ 0xED, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_SBC },
			{ 0xFD, OpAddressingMode::     AbsoluteX, 3, 4, 1, CPU::OP_SBC },
			{ 0xF9, OpAddressingMode::     AbsoluteY, 3, 4, 1, CPU::OP_SBC },
			{ 0xE1, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_SBC },
			{ 0xF1, OpAddressingMode:: IndirectIndex, 2, 5, 1, CPU::OP_SBC },
			{ 0x38, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_SEC },
			{ 0xF8, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_SED },
			{ 0x78, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_SEI },
			{ 0x85, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_STA },
			{ 0x95, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_STA },
			{ 0x8D, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_STA },
			{ 0x9D, OpAddressingMode::     AbsoluteX, 3, 5, 0, CPU::OP_STA },
			{ 0x99, OpAddressingMode::     AbsoluteY, 3, 5, 0, CPU::OP_STA },
			{ 0x81, OpAddressingMode:: IndexIndirect, 2, 6, 0, CPU::OP_STA },
			{ 0x91, OpAddressingMode:: IndirectIndex, 2, 6, 0, CPU::OP_STA },
			{ 0x86, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_STX },
			{ 0x96, OpAddressingMode::     ZeroPageY, 2, 4, 0, CPU::OP_STX },
			{ 0x8E, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_STX },
			{ 0x84, OpAddressingMode::      ZeroPage, 2, 3, 0, CPU::OP_STY },
			{ 0x94, OpAddressingMode::     ZeroPageX, 2, 4, 0, CPU::OP_STY },
			{ 0x8C, OpAddressingMode::      Absolute, 3, 4, 0, CPU::OP_STY },
			{ 0xAA, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_TAX },
			{ 0xA8, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_TAY },
			{ 0xBA, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_TSX },
			{ 0x8A, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_TXA },
			{ 0x9A, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_TXS },
			{ 0x98, OpAddressingMode::      Implicit, 1, 2, 0, CPU::OP_TYA }
	};

	static_assert(LEN(op_entity) == 151, "instruction count should be 151.");
	static const Operation *optable[0xff];
	for(const auto & op: op_entity) {
		assert(optable[op.code] == nullptr);
		optable[op.code] = &op;
	}
	return optable;
}

void CPU::FetchOperands(CPU::OpAddressingMode addressing_mode, uint8_t *& oprand, bool& crossed_page) {
	static uint8_t tempOprand = 0;
	uint16_t addr, pointer;
	switch (addressing_mode) {
		case OpAddressingMode::Implicit:
		case OpAddressingMode::Accumulator:
			oprand = nullptr;
			break;
		case OpAddressingMode::Immediate:
			tempOprand = Read8(PC + 1);
			oprand = &tempOprand;
			break;

		case OpAddressingMode::Absolute:
			addr = Read16(PC + 1);
			oprand = &mem[addr];
			break;

		case OpAddressingMode::AbsoluteX:
			addr = Read16(PC + 1);
			if( ((addr + X) & 0xff00) != (addr & 0xff00) ) crossed_page = true;
			addr += X;
			oprand = &mem[addr];
			break;
		case OpAddressingMode::AbsoluteY:
			addr = Read16(PC + 1);
			if( ((addr + Y) & 0xff00) != (addr & 0xff00) ) crossed_page = true;
			addr += Y;
			oprand = &mem[addr];
			break;

		case OpAddressingMode::Relative: // 分支指令专用
			tempOprand = Read8(PC + 1);
			oprand = &tempOprand;
			break;
		case OpAddressingMode::IndexIndirect: // IDX
			pointer = Read8(PC + 1);
			addr = Read8((pointer + X) & 0xff) | Read8((pointer + X + 1) &0xff) << 0x08;
			oprand = &mem[addr];
			break;

		case OpAddressingMode::IndirectIndex: // IDY
			pointer = Read8(PC + 1);
			addr = Read8(pointer) | Read8((pointer + 1) & 0xff) << 0x08;
			if( ((addr + Y) & 0xff00) != (addr & 0xff00) ) crossed_page = true;
			addr += Y;
			oprand = &mem[addr];
			break;

		case OpAddressingMode::ZeroPage:
			addr = Read8(PC + 1);
			oprand = &mem[addr];
			break;

		case OpAddressingMode::ZeroPageX:
			addr = (Read8(PC + 1) + X) & 0xff;
			oprand = &mem[addr];
			break;

		case OpAddressingMode::ZeroPageY:
			addr = (Read8(PC + 1) + Y) & 0xff;
			oprand = &mem[addr];
			break;

		case OpAddressingMode::Indirect: // JMP专用
		default:
			oprand = nullptr;
			break;
	}
}

uint8_t CPU::Execute() {
	// 执行一条指令，返回执行周期数
	// 取指->译码->执行->更新PC->...

	// 取指
	uint8_t op_code = Read8(PC);
	uint16_t updated_pc = PC + optable[op_code]->bytes;

	// 译码
	uint8_t *oprand = nullptr;
	bool crossed_page = false;
	FetchOperands(optable[op_code]->addressing_mode, oprand, crossed_page);

	// 执行
	uint8_t cycle = ExeFunc(optable[op_code], this, oprand, updated_pc, crossed_page);

	// 更新PC
	PC = updated_pc;
	cycles += cycle;

	return cycle;
}

// 参考手册： http://obelisk.me.uk/6502/reference.html
/**************** 指令实现区Begin ****************/
OpExeFuncDefine(OP_ASL) {
	/**
	 * ASL - Arithmetic Shift Left
	 * A,Z,C,N = M*2 or M,Z,C,N = M*2
	 * This operation shifts all the bits of the accumulator or memory
	 * contents one bit left. Bit 0 is set to 0 and bit 7 is placed in
	 * the carry flag. The effect of this operation is to multiply the
	 * memory contents by 2 (ignoring 2's complement considerations),
	 * setting the carry if the result will not fit in 8 bits.
	 **/
	uint16_t result = (operand ? *operand : cpu->A) << 1;
	(operand ? *operand : cpu->A) <<= 1;

	cpu->P.Zero = ((result & 0xff) == 0);
	cpu->P.Carry = GetBit(result, 0x8);
	cpu->P.Negative = GetBit(result, 0x7);

	return self.cycles;
}

OpExeFuncDefine(OP_CLC) {
	// TODO: wait for implements: CLC
	/**
	 * CLC - Clear Carry Flag
	 * C = 0
	 * Set the carry flag to zero.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_CLD) {
	/**
	 * CLD - Clear Decimal Mode
	 * D = 0
	 * Sets the decimal mode flag to zero.
	 **/
	cpu->P.Decimal = false;

	return self.cycles;
}

OpExeFuncDefine(OP_CLI) {
	// TODO: wait for implements: CLI
	/**
	 * CLI - Clear Interrupt Disable
	 * I = 0
	 * Clears the interrupt disable flag allowing normal interrupt
	 * requests to be serviced.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_CLV) {
	// TODO: wait for implements: CLV
	/**
	 * CLV - Clear Overflow Flag
	 * V = 0
	 * Clears the overflow flag.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_DEX) {
	// TODO: wait for implements: DEX
	/**
	 * DEX - Decrement X Register
	 * X,Z,N = X-1
	 * Subtracts one from the X register setting the zero and negative
	 * flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_DEY) {
	// TODO: wait for implements: DEY
	/**
	 * DEY - Decrement Y Register
	 * Y,Z,N = Y-1
	 * Subtracts one from the Y register setting the zero and negative
	 * flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_INX) {
	// TODO: wait for implements: INX
	/**
	 * INX - Increment X Register
	 * X,Z,N = X+1
	 * Adds one to the X register setting the zero and negative flags
	 * as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_INY) {
	// TODO: wait for implements: INY
	/**
	 * INY - Increment Y Register
	 * Y,Z,N = Y+1
	 * Adds one to the Y register setting the zero and negative flags
	 * as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_LSR) {
	// TODO: wait for implements: LSR
	/**
	 * LSR - Logical Shift Right
	 * A,C,Z,N = A/2 or M,C,Z,N = M/2
	 * Each of the bits in A or M is shift one place to the right.
	 * The bit that was in bit 0 is shifted into the carry flag. Bit
	 * 7 is set to zero.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_NOP) {
	// TODO: wait for implements: NOP
	/**
	 * NOP - No Operation
	 * The NOP instruction causes no changes to the processor other
	 * than the normal incrementing of the program counter to the next
	 * instruction.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ROL) {
	// TODO: wait for implements: ROL
	/**
	 * ROL - Rotate Left
	 * Move each of the bits in either A or M one place to the left.
	 * Bit 0 is filled with the current value of the carry flag whilst
	 * the old bit 7 becomes the new carry flag value.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ROR) {
	// TODO: wait for implements: ROR
	/**
	 * ROR - Rotate Right
	 * Move each of the bits in either A or M one place to the right.
	 * Bit 7 is filled with the current value of the carry flag whilst
	 * the old bit 0 becomes the new carry flag value.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SEC) {
	// TODO: wait for implements: SEC
	/**
	 * SEC - Set Carry Flag
	 * C = 1
	 * Set the carry flag to one.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SED) {
	/**
	 * SED - Set Decimal Flag
	 * D = 1
	 * Set the decimal mode flag to one.
	 **/
	cpu->P.Decimal = true;

	return self.cycles;
}

OpExeFuncDefine(OP_SEI) {
	// TODO: wait for implements: SEI
	/**
	 * SEI - Set Interrupt Disable
	 * I = 1
	 * Set the interrupt disable flag to one.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TAX) {
	// TODO: wait for implements: TAX
	/**
	 * TAX - Transfer Accumulator to X
	 * X = A
	 * Copies the current contents of the accumulator into the X register
	 * and sets the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TAY) {
	// TODO: wait for implements: TAY
	/**
	 * TAY - Transfer Accumulator to Y
	 * Y = A
	 * Copies the current contents of the accumulator into the Y register
	 * and sets the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TSX) {
	// TODO: wait for implements: TSX
	/**
	 * TSX - Transfer Stack Pointer to X
	 * X = S
	 * Copies the current contents of the stack register into the
	 * X register and sets the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TXA) {
	// TODO: wait for implements: TXA
	/**
	 * TXA - Transfer X to Accumulator
	 * A = X
	 * Copies the current contents of the X register into the accumulator
	 * and sets the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TXS) {
	// TODO: wait for implements: TXS
	/**
	 * TXS - Transfer X to Stack Pointer
	 * S = X
	 * Copies the current contents of the X register into the stack
	 * register.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TYA) {
	// TODO: wait for implements: TYA
	/**
	 * TYA - Transfer Y to Accumulator
	 * A = Y
	 * Copies the current contents of the Y register into the accumulator
	 * and sets the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_PHA) {
	// TODO: wait for implements: PHA
	/**
	 * PHA - Push Accumulator
	 * Pushes a copy of the accumulator on to the stack.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_PHP) {
	// TODO: wait for implements: PHP
	/**
	 * PHP - Push Processor Status
	 * Pushes a copy of the status flags on to the stack.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_PLA) {
	// TODO: wait for implements: PLA
	/**
	 * PLA - Pull Accumulator
	 * Pulls an 8 bit value from the stack and into the accumulator.
	 * The zero and negative flags are set as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_PLP) {
	// TODO: wait for implements: PLP
	/**
	 * PLP - Pull Processor Status
	 * Pulls an 8 bit value from the stack and into the processor
	 * flags. The flags will take on new states as determined by the
	 * value pulled.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_RTI) {
	// TODO: wait for implements: RTI
	/**
	 * RTI - Return from Interrupt
	 * The RTI instruction is used at the end of an interrupt processing
	 * routine. It pulls the processor flags from the stack followed
	 * by the program counter.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_RTS) {
	// TODO: wait for implements: RTS
	/**
	 * RTS - Return from Subroutine
	 * The RTS instruction is used at the end of a subroutine to return
	 * to the calling routine. It pulls the program counter (minus one)
	 * from the stack.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BRK) {
	// TODO: wait for implements: BRK
	/**
	 * BRK - Force Interrupt
	 * The BRK instruction forces the generation of an interrupt request.
	 * The program counter and processor status are pushed on the stack
	 * then the IRQ interrupt vector at $FFFE/F is loaded into the PC
	 * and the break flag in the status set to one.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BCC) {
	// TODO: wait for implements: BCC
	/**
	 * BCC - Branch if Carry Clear
	 * If the carry flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BCS) {
	// TODO: wait for implements: BCS
	/**
	 * BCS - Branch if Carry Set
	 * If the carry flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BEQ) {
	// TODO: wait for implements: BEQ
	/**
	 * BEQ - Branch if Equal
	 * If the zero flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BMI) {
	// TODO: wait for implements: BMI
	/**
	 * BMI - Branch if Minus
	 * If the negative flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BNE) {
	// TODO: wait for implements: BNE
	/**
	 * BNE - Branch if Not Equal
	 * If the zero flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BPL) {
	// TODO: wait for implements: BPL
	/**
	 * BPL - Branch if Positive
	 * If the negative flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BVC) {
	// TODO: wait for implements: BVC
	/**
	 * BVC - Branch if Overflow Clear
	 * If the overflow flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BVS) {
	// TODO: wait for implements: BVS
	/**
	 * BVS - Branch if Overflow Set
	 * If the overflow flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ADC) {
	// TODO: wait for implements: ADC
	/**
	 * ADC - Add with Carry
	 * A,Z,C,N = A+M+C
	 * This instruction adds the contents of a memory location to
	 * the accumulator together with the carry bit. If overflow occurs
	 * the carry bit is set, this enables multiple byte addition to be
	 * performed.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_AND) {
	// TODO: wait for implements: AND
	/**
	 * AND - Logical AND
	 * A,Z,N = A&M
	 * A logical AND is performed, bit by bit, on the accumulator
	 * contents using the contents of a byte of memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_CMP) {
	// TODO: wait for implements: CMP
	/**
	 * CMP - Compare
	 * Z,C,N = A-M
	 * This instruction compares the contents of the accumulator with
	 * another memory held value and sets the zero and carry flags as
	 * appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_CPX) {
	// TODO: wait for implements: CPX
	/**
	 * CPX - Compare X Register
	 * Z,C,N = X-M
	 * This instruction compares the contents of the X register with
	 * another memory held value and sets the zero and carry flags as
	 * appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_CPY) {
	// TODO: wait for implements: CPY
	/**
	 * CPY - Compare Y Register
	 * Z,C,N = Y-M
	 * This instruction compares the contents of the Y register with
	 * another memory held value and sets the zero and carry flags as
	 * appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_EOR) {
	// TODO: wait for implements: EOR
	/**
	 * EOR - Exclusive OR
	 * A,Z,N = A^M
	 * An exclusive OR is performed, bit by bit, on the accumulator
	 * contents using the contents of a byte of memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_LDA) {
	// TODO: wait for implements: LDA
	/**
	 * LDA - Load Accumulator
	 * A,Z,N = M
	 * Loads a byte of memory into the accumulator setting the zero
	 * and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_LDX) {
	// TODO: wait for implements: LDX
	/**
	 * LDX - Load X Register
	 * X,Z,N = M
	 * Loads a byte of memory into the X register setting the zero
	 * and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_LDY) {
	// TODO: wait for implements: LDY
	/**
	 * LDY - Load Y Register
	 * Y,Z,N = M
	 * Loads a byte of memory into the Y register setting the zero
	 * and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ORA) {
	// TODO: wait for implements: ORA
	/**
	 * ORA - Logical Inclusive OR
	 * A,Z,N = A|M
	 * An inclusive OR is performed, bit by bit, on the accumulator
	 * contents using the contents of a byte of memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SBC) {
	// TODO: wait for implements: SBC
	/**
	 * SBC - Subtract with Carry
	 * A,Z,C,N = A-M-(1-C)
	 * This instruction subtracts the contents of a memory location
	 * to the accumulator together with the not of the carry bit. If
	 * overflow occurs the carry bit is clear, this enables multiple
	 * byte subtraction to be performed.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_BIT) {
	// TODO: wait for implements: BIT
	/**
	 * BIT - Bit Test
	 * A & M, N = M7, V = M6
	 * This instructions is used to test if one or more bits are set
	 * in a target memory location. The mask pattern in A is ANDed with
	 * the value in memory to set or clear the zero flag, but the result
	 * is not kept. Bits 7 and 6 of the value from memory are copied
	 * into the N and V flags.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_STA) {
	// TODO: wait for implements: STA
	/**
	 * STA - Store Accumulator
	 * M = A
	 * Stores the contents of the accumulator into memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_STX) {
	// TODO: wait for implements: STX
	/**
	 * STX - Store X Register
	 * M = X
	 * Stores the contents of the X register into memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_STY) {
	// TODO: wait for implements: STY
	/**
	 * STY - Store Y Register
	 * M = Y
	 * Stores the contents of the Y register into memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_DEC) {
	// TODO: wait for implements: DEC
	/**
	 * DEC - Decrement Memory
	 * M,Z,N = M-1
	 * Subtracts one from the value held at a specified memory location
	 * setting the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_INC) {
	// TODO: wait for implements: INC
	/**
	 * INC - Increment Memory
	 * M,Z,N = M+1
	 * Adds one to the value held at a specified memory location setting
	 * the zero and negative flags as appropriate.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_JMP) {
	// TODO: wait for implements: JMP
	/**
	 * JMP - Jump
	 * Sets the program counter to the address specified by the operand.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_JSR) {
	// TODO: wait for implements: JSR
	/**
	 * JSR - Jump to Subroutine
	 * The JSR instruction pushes the address (minus one) of the return
	 * point on to the stack and then sets the program counter to the
	 * target memory address.
	 **/

	return self.cycles;
}

/****************  指令实现区End  ****************/


