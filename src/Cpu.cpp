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

uint8_t CPU::Execute() {
	// 执行一条指令，返回执行周期数
	// 取指->译码->执行->更新PC->...

	// 取指
	uint8_t op_code = Read8(PC);
	PC += optable[op_code]->bytes;

	// 译码
	uint8_t *oprand1, *oprand2;

	// 执行
	return ExeFunc(optable[op_code], P, oprand1, oprand2);

}


/**************** 指令实现区Begin ****************/
OpExeFuncDefine(OP_ASL) {
	// TODO: wait for implements: ASL

	return self.cycles;
}

OpExeFuncDefine(OP_CLC) {
	// TODO: wait for implements: CLC

	return self.cycles;
}

OpExeFuncDefine(OP_CLD) {
	// Sets the decimal mode flag to zero.
	P.Decimal = false;
	return self.cycles;
}

OpExeFuncDefine(OP_CLI) {
	// TODO: wait for implements: CLI

	return self.cycles;
}

OpExeFuncDefine(OP_CLV) {
	// TODO: wait for implements: CLV

	return self.cycles;
}

OpExeFuncDefine(OP_DEX) {
	// TODO: wait for implements: DEX

	return self.cycles;
}

OpExeFuncDefine(OP_DEY) {
	// TODO: wait for implements: DEY

	return self.cycles;
}

OpExeFuncDefine(OP_INX) {
	// TODO: wait for implements: INX

	return self.cycles;
}

OpExeFuncDefine(OP_INY) {
	// TODO: wait for implements: INY

	return self.cycles;
}

OpExeFuncDefine(OP_LSR) {
	// TODO: wait for implements: LSR

	return self.cycles;
}

OpExeFuncDefine(OP_NOP) {
	// TODO: wait for implements: NOP

	return self.cycles;
}

OpExeFuncDefine(OP_ROL) {
	// TODO: wait for implements: ROL

	return self.cycles;
}

OpExeFuncDefine(OP_ROR) {
	// TODO: wait for implements: ROR

	return self.cycles;
}

OpExeFuncDefine(OP_SEC) {
	// TODO: wait for implements: SEC

	return self.cycles;
}

OpExeFuncDefine(OP_SED) {
	// Set the decimal mode flag to one.
	P.Decimal = true;
	return self.cycles;
}

OpExeFuncDefine(OP_SEI) {
	// TODO: wait for implements: SEI

	return self.cycles;
}

OpExeFuncDefine(OP_TAX) {
	// TODO: wait for implements: TAX

	return self.cycles;
}

OpExeFuncDefine(OP_TAY) {
	// TODO: wait for implements: TAY

	return self.cycles;
}

OpExeFuncDefine(OP_TSX) {
	// TODO: wait for implements: TSX

	return self.cycles;
}

OpExeFuncDefine(OP_TXA) {
	// TODO: wait for implements: TXA

	return self.cycles;
}

OpExeFuncDefine(OP_TXS) {
	// TODO: wait for implements: TXS

	return self.cycles;
}

OpExeFuncDefine(OP_TYA) {
	// TODO: wait for implements: TYA

	return self.cycles;
}

OpExeFuncDefine(OP_PHA) {
	// TODO: wait for implements: PHA

	return self.cycles;
}

OpExeFuncDefine(OP_PHP) {
	// TODO: wait for implements: PHP

	return self.cycles;
}

OpExeFuncDefine(OP_PLA) {
	// TODO: wait for implements: PLA

	return self.cycles;
}

OpExeFuncDefine(OP_PLP) {
	// TODO: wait for implements: PLP

	return self.cycles;
}

OpExeFuncDefine(OP_RTI) {
	// TODO: wait for implements: RTI

	return self.cycles;
}

OpExeFuncDefine(OP_RTS) {
	// TODO: wait for implements: RTS

	return self.cycles;
}

OpExeFuncDefine(OP_BRK) {
	// TODO: wait for implements: BRK

	return self.cycles;
}

OpExeFuncDefine(OP_BCC) {
	// TODO: wait for implements: BCC

	return self.cycles;
}

OpExeFuncDefine(OP_BCS) {
	// TODO: wait for implements: BCS

	return self.cycles;
}

OpExeFuncDefine(OP_BEQ) {
	// TODO: wait for implements: BEQ

	return self.cycles;
}

OpExeFuncDefine(OP_BMI) {
	// TODO: wait for implements: BMI

	return self.cycles;
}

OpExeFuncDefine(OP_BNE) {
	// TODO: wait for implements: BNE

	return self.cycles;
}

OpExeFuncDefine(OP_BPL) {
	// TODO: wait for implements: BPL

	return self.cycles;
}

OpExeFuncDefine(OP_BVC) {
	// TODO: wait for implements: BVC

	return self.cycles;
}

OpExeFuncDefine(OP_BVS) {
	// TODO: wait for implements: BVS

	return self.cycles;
}

OpExeFuncDefine(OP_ADC) {
	// TODO: wait for implements: ADC

	return self.cycles;
}

OpExeFuncDefine(OP_AND) {
	// TODO: wait for implements: AND

	return self.cycles;
}

OpExeFuncDefine(OP_CMP) {
	// TODO: wait for implements: CMP

	return self.cycles;
}

OpExeFuncDefine(OP_CPX) {
	// TODO: wait for implements: CPX

	return self.cycles;
}

OpExeFuncDefine(OP_CPY) {
	// TODO: wait for implements: CPY

	return self.cycles;
}

OpExeFuncDefine(OP_EOR) {
	// TODO: wait for implements: EOR

	return self.cycles;
}

OpExeFuncDefine(OP_LDA) {
	// TODO: wait for implements: LDA

	return self.cycles;
}

OpExeFuncDefine(OP_LDX) {
	// TODO: wait for implements: LDX

	return self.cycles;
}

OpExeFuncDefine(OP_LDY) {
	// TODO: wait for implements: LDY

	return self.cycles;
}

OpExeFuncDefine(OP_ORA) {
	// TODO: wait for implements: ORA

	return self.cycles;
}

OpExeFuncDefine(OP_SBC) {
	// TODO: wait for implements: SBC

	return self.cycles;
}

OpExeFuncDefine(OP_BIT) {
	// TODO: wait for implements: BIT

	return self.cycles;
}

OpExeFuncDefine(OP_STA) {
	// TODO: wait for implements: STA

	return self.cycles;
}

OpExeFuncDefine(OP_STX) {
	// TODO: wait for implements: STX

	return self.cycles;
}

OpExeFuncDefine(OP_STY) {
	// TODO: wait for implements: STY

	return self.cycles;
}

OpExeFuncDefine(OP_DEC) {
	// TODO: wait for implements: DEC

	return self.cycles;
}

OpExeFuncDefine(OP_INC) {
	// TODO: wait for implements: INC

	return self.cycles;
}

OpExeFuncDefine(OP_JMP) {
	// TODO: wait for implements: JMP

	return self.cycles;
}

OpExeFuncDefine(OP_JSR) {
	// TODO: wait for implements: JSR

	return self.cycles;
}

/****************  指令实现区End  ****************/


