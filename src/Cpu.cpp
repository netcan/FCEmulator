/****************************************************************************
 > File Name: cpu.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/
#include "Cpu.h"

ProcessorStatus::ProcessorStatus(uint8_t value): Negative(this->P), Overflow(this->P),
                                    BrkExecuted(this->P), Invalid(this->P), Decimal(this->P),
                                    IrqDisabled(this->P), Zero(this->P),
                                    Carry(this->P) {
	P = value; // 初值
}


const Operation **CPU::optable = CPU::InitOptable();


const Operation ** CPU::InitOptable() {
	static const Operation op_entity[] = {
			{ 0x69, OpAddressingMode::    Immediate, 2, 2, CPU::OP_ADC },
			{ 0x65, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_ADC },
			{ 0x75, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_ADC },
			{ 0x6D, OpAddressingMode::     Absolute, 3, 4, CPU::OP_ADC },
			{ 0x7D, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_ADC },
			{ 0x79, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_ADC },
			{ 0x61, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_ADC },
			{ 0x71, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_ADC },
			{ 0x29, OpAddressingMode::    Immediate, 2, 2, CPU::OP_AND },
			{ 0x25, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_AND },
			{ 0x35, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_AND },
			{ 0x2D, OpAddressingMode::     Absolute, 3, 4, CPU::OP_AND },
			{ 0x3D, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_AND },
			{ 0x39, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_AND },
			{ 0x21, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_AND },
			{ 0x31, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_AND },
			{ 0x0A, OpAddressingMode::  Accumulator, 1, 2, CPU::OP_ASL },
			{ 0x06, OpAddressingMode::     ZeroPage, 2, 5, CPU::OP_ASL },
			{ 0x16, OpAddressingMode::    ZeroPageX, 2, 6, CPU::OP_ASL },
			{ 0x0E, OpAddressingMode::     Absolute, 3, 6, CPU::OP_ASL },
			{ 0x1E, OpAddressingMode::    AbsoluteX, 3, 7, CPU::OP_ASL },
			{ 0x90, OpAddressingMode::     Relative, 2, 2, CPU::OP_BCC },
			{ 0xB0, OpAddressingMode::     Relative, 2, 2, CPU::OP_BCS },
			{ 0xF0, OpAddressingMode::     Relative, 2, 2, CPU::OP_BEQ },
			{ 0x24, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_BIT },
			{ 0x2C, OpAddressingMode::     Absolute, 3, 4, CPU::OP_BIT },
			{ 0x30, OpAddressingMode::     Relative, 2, 2, CPU::OP_BMI },
			{ 0xD0, OpAddressingMode::     Relative, 2, 2, CPU::OP_BNE },
			{ 0x10, OpAddressingMode::     Relative, 2, 2, CPU::OP_BPL },
			{ 0x00, OpAddressingMode::     Implicit, 1, 7, CPU::OP_BRK },
			{ 0x50, OpAddressingMode::     Relative, 2, 2, CPU::OP_BVC },
			{ 0x70, OpAddressingMode::     Relative, 2, 2, CPU::OP_BVS },
			{ 0x18, OpAddressingMode::     Implicit, 1, 2, CPU::OP_CLC },
			{ 0xD8, OpAddressingMode::     Implicit, 1, 2, CPU::OP_CLD },
			{ 0x58, OpAddressingMode::     Implicit, 1, 2, CPU::OP_CLI },
			{ 0xB8, OpAddressingMode::     Implicit, 1, 2, CPU::OP_CLV },
			{ 0xC9, OpAddressingMode::    Immediate, 2, 2, CPU::OP_CMP },
			{ 0xC5, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_CMP },
			{ 0xD5, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_CMP },
			{ 0xCD, OpAddressingMode::     Absolute, 3, 4, CPU::OP_CMP },
			{ 0xDD, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_CMP },
			{ 0xD9, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_CMP },
			{ 0xC1, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_CMP },
			{ 0xD1, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_CMP },
			{ 0xE0, OpAddressingMode::    Immediate, 2, 2, CPU::OP_CPX },
			{ 0xE4, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_CPX },
			{ 0xEC, OpAddressingMode::     Absolute, 3, 4, CPU::OP_CPX },
			{ 0xC0, OpAddressingMode::    Immediate, 2, 2, CPU::OP_CPY },
			{ 0xC4, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_CPY },
			{ 0xCC, OpAddressingMode::     Absolute, 3, 4, CPU::OP_CPY },
			{ 0xC6, OpAddressingMode::     ZeroPage, 2, 5, CPU::OP_DEC },
			{ 0xD6, OpAddressingMode::    ZeroPageX, 2, 6, CPU::OP_DEC },
			{ 0xCE, OpAddressingMode::     Absolute, 3, 6, CPU::OP_DEC },
			{ 0xDE, OpAddressingMode::    AbsoluteX, 3, 7, CPU::OP_DEC },
			{ 0xCA, OpAddressingMode::     Implicit, 1, 2, CPU::OP_DEX },
			{ 0x88, OpAddressingMode::     Implicit, 1, 2, CPU::OP_DEY },
			{ 0x49, OpAddressingMode::    Immediate, 2, 2, CPU::OP_EOR },
			{ 0x45, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_EOR },
			{ 0x55, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_EOR },
			{ 0x4D, OpAddressingMode::     Absolute, 3, 4, CPU::OP_EOR },
			{ 0x5D, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_EOR },
			{ 0x59, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_EOR },
			{ 0x41, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_EOR },
			{ 0x51, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_EOR },
			{ 0xE6, OpAddressingMode::     ZeroPage, 2, 5, CPU::OP_INC },
			{ 0xF6, OpAddressingMode::    ZeroPageX, 2, 6, CPU::OP_INC },
			{ 0xEE, OpAddressingMode::     Absolute, 3, 6, CPU::OP_INC },
			{ 0xFE, OpAddressingMode::    AbsoluteX, 3, 7, CPU::OP_INC },
			{ 0xE8, OpAddressingMode::     Implicit, 1, 2, CPU::OP_INX },
			{ 0xC8, OpAddressingMode::     Implicit, 1, 2, CPU::OP_INY },
			{ 0x4C, OpAddressingMode::     Absolute, 3, 3, CPU::OP_JMP },
			{ 0x6C, OpAddressingMode::     Indirect, 3, 5, CPU::OP_JMP },
			{ 0x20, OpAddressingMode::     Absolute, 3, 6, CPU::OP_JSR },
			{ 0xA9, OpAddressingMode::    Immediate, 2, 2, CPU::OP_LDA },
			{ 0xA5, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_LDA },
			{ 0xB5, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_LDA },
			{ 0xAD, OpAddressingMode::     Absolute, 3, 4, CPU::OP_LDA },
			{ 0xBD, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_LDA },
			{ 0xB9, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_LDA },
			{ 0xA1, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_LDA },
			{ 0xB1, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_LDA },
			{ 0xA2, OpAddressingMode::    Immediate, 2, 2, CPU::OP_LDX },
			{ 0xA6, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_LDX },
			{ 0xB6, OpAddressingMode::    ZeroPageY, 2, 4, CPU::OP_LDX },
			{ 0xAE, OpAddressingMode::     Absolute, 3, 4, CPU::OP_LDX },
			{ 0xBE, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_LDX },
			{ 0xA0, OpAddressingMode::    Immediate, 2, 2, CPU::OP_LDY },
			{ 0xA4, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_LDY },
			{ 0xB4, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_LDY },
			{ 0xAC, OpAddressingMode::     Absolute, 3, 4, CPU::OP_LDY },
			{ 0xBC, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_LDY },
			{ 0x4A, OpAddressingMode::  Accumulator, 1, 2, CPU::OP_LSR },
			{ 0x46, OpAddressingMode::     ZeroPage, 2, 5, CPU::OP_LSR },
			{ 0x56, OpAddressingMode::    ZeroPageX, 2, 6, CPU::OP_LSR },
			{ 0x4E, OpAddressingMode::     Absolute, 3, 6, CPU::OP_LSR },
			{ 0x5E, OpAddressingMode::    AbsoluteX, 3, 7, CPU::OP_LSR },
			{ 0xEA, OpAddressingMode::     Implicit, 1, 2, CPU::OP_NOP },
			{ 0x09, OpAddressingMode::    Immediate, 2, 2, CPU::OP_ORA },
			{ 0x05, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_ORA },
			{ 0x15, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_ORA },
			{ 0x0D, OpAddressingMode::     Absolute, 3, 4, CPU::OP_ORA },
			{ 0x1D, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_ORA },
			{ 0x19, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_ORA },
			{ 0x01, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_ORA },
			{ 0x11, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_ORA },
			{ 0x48, OpAddressingMode::     Implicit, 1, 3, CPU::OP_PHA },
			{ 0x08, OpAddressingMode::     Implicit, 1, 3, CPU::OP_PHP },
			{ 0x68, OpAddressingMode::     Implicit, 1, 4, CPU::OP_PLA },
			{ 0x28, OpAddressingMode::     Implicit, 1, 4, CPU::OP_PLP },
			{ 0x2A, OpAddressingMode::  Accumulator, 1, 2, CPU::OP_ROL },
			{ 0x26, OpAddressingMode::     ZeroPage, 2, 5, CPU::OP_ROL },
			{ 0x36, OpAddressingMode::    ZeroPageX, 2, 6, CPU::OP_ROL },
			{ 0x2E, OpAddressingMode::     Absolute, 3, 6, CPU::OP_ROL },
			{ 0x3E, OpAddressingMode::    AbsoluteX, 3, 7, CPU::OP_ROL },
			{ 0x6A, OpAddressingMode::  Accumulator, 1, 2, CPU::OP_ROR },
			{ 0x66, OpAddressingMode::     ZeroPage, 2, 5, CPU::OP_ROR },
			{ 0x76, OpAddressingMode::    ZeroPageX, 2, 6, CPU::OP_ROR },
			{ 0x6E, OpAddressingMode::     Absolute, 3, 6, CPU::OP_ROR },
			{ 0x7E, OpAddressingMode::    AbsoluteX, 3, 7, CPU::OP_ROR },
			{ 0x40, OpAddressingMode::     Implicit, 1, 6, CPU::OP_RTI },
			{ 0x60, OpAddressingMode::     Implicit, 1, 6, CPU::OP_RTS },
			{ 0xE9, OpAddressingMode::    Immediate, 2, 2, CPU::OP_SBC },
			{ 0xE5, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_SBC },
			{ 0xF5, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_SBC },
			{ 0xED, OpAddressingMode::     Absolute, 3, 4, CPU::OP_SBC },
			{ 0xFD, OpAddressingMode::    AbsoluteX, 3, 4, CPU::OP_SBC },
			{ 0xF9, OpAddressingMode::    AbsoluteY, 3, 4, CPU::OP_SBC },
			{ 0xE1, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_SBC },
			{ 0xF1, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_SBC },
			{ 0x38, OpAddressingMode::     Implicit, 1, 2, CPU::OP_SEC },
			{ 0xF8, OpAddressingMode::     Implicit, 1, 2, CPU::OP_SED },
			{ 0x78, OpAddressingMode::     Implicit, 1, 2, CPU::OP_SEI },
			{ 0x85, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_STA },
			{ 0x95, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_STA },
			{ 0x8D, OpAddressingMode::     Absolute, 3, 4, CPU::OP_STA },
			{ 0x9D, OpAddressingMode::    AbsoluteX, 3, 5, CPU::OP_STA },
			{ 0x99, OpAddressingMode::    AbsoluteY, 3, 5, CPU::OP_STA },
			{ 0x81, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_STA },
			{ 0x91, OpAddressingMode::IndirectIndex, 2, 6, CPU::OP_STA },
			{ 0x86, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_STX },
			{ 0x96, OpAddressingMode::    ZeroPageY, 2, 4, CPU::OP_STX },
			{ 0x8E, OpAddressingMode::     Absolute, 3, 4, CPU::OP_STX },
			{ 0x84, OpAddressingMode::     ZeroPage, 2, 3, CPU::OP_STY },
			{ 0x94, OpAddressingMode::    ZeroPageX, 2, 4, CPU::OP_STY },
			{ 0x8C, OpAddressingMode::     Absolute, 3, 4, CPU::OP_STY },
			{ 0xAA, OpAddressingMode::     Implicit, 1, 2, CPU::OP_TAX },
			{ 0xA8, OpAddressingMode::     Implicit, 1, 2, CPU::OP_TAY },
			{ 0xBA, OpAddressingMode::     Implicit, 1, 2, CPU::OP_TSX },
			{ 0x8A, OpAddressingMode::     Implicit, 1, 2, CPU::OP_TXA },
			{ 0x9A, OpAddressingMode::     Implicit, 1, 2, CPU::OP_TXS },
			{ 0x98, OpAddressingMode::     Implicit, 1, 2, CPU::OP_TYA },

			// Undocumented op
			{ 0x0B, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_AAC },
			{ 0x2B, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_AAC },
			{ 0x87, OpAddressingMode::  ZeroPage   , 2, 3, CPU::OP_AAX },
			{ 0x97, OpAddressingMode::   ZeroPageY , 2, 4, CPU::OP_AAX },
			{ 0x83, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_AAX },
			{ 0x8F, OpAddressingMode:: Absolute    , 3, 4, CPU::OP_AAX },
			{ 0x6B, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_ARR },
			{ 0x4B, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_ASR },
			{ 0xAB, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_ATX },
			{ 0x9F, OpAddressingMode::  AbsoluteY  , 3, 5, CPU::OP_AXA },
			{ 0x93, OpAddressingMode::IndirectIndex, 2, 6, CPU::OP_AXA },
			{ 0xCB, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_AXS },
			{ 0xC7, OpAddressingMode::  ZeroPage   , 2, 5, CPU::OP_DCP },
			{ 0xD7, OpAddressingMode::   ZeroPageX , 2, 6, CPU::OP_DCP },
			{ 0xCF, OpAddressingMode:: Absolute    , 3, 6, CPU::OP_DCP },
			{ 0xDF, OpAddressingMode::  AbsoluteX  , 3, 7, CPU::OP_DCP },
			{ 0xDB, OpAddressingMode::  AbsoluteY  , 3, 7, CPU::OP_DCP },
			{ 0xC3, OpAddressingMode::IndexIndirect, 2, 8, CPU::OP_DCP },
			{ 0xD3, OpAddressingMode::IndirectIndex, 2, 8, CPU::OP_DCP },
			{ 0x04, OpAddressingMode::  ZeroPage   , 2, 3, CPU::OP_DOP },
			{ 0x14, OpAddressingMode::   ZeroPageX , 2, 4, CPU::OP_DOP },
			{ 0x34, OpAddressingMode::   ZeroPageX , 2, 4, CPU::OP_DOP },
			{ 0x44, OpAddressingMode::  ZeroPage   , 2, 3, CPU::OP_DOP },
			{ 0x54, OpAddressingMode::   ZeroPageX , 2, 4, CPU::OP_DOP },
			{ 0x64, OpAddressingMode::  ZeroPage   , 2, 3, CPU::OP_DOP },
			{ 0x74, OpAddressingMode::   ZeroPageX , 2, 4, CPU::OP_DOP },
			{ 0x80, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_DOP },
			{ 0x82, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_DOP },
			{ 0x89, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_DOP },
			{ 0xC2, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_DOP },
			{ 0xD4, OpAddressingMode::   ZeroPageX , 2, 4, CPU::OP_DOP },
			{ 0xE2, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_DOP },
			{ 0xF4, OpAddressingMode::   ZeroPageX , 2, 4, CPU::OP_DOP },
			{ 0xE7, OpAddressingMode::  ZeroPage   , 2, 5, CPU::OP_ISC },
			{ 0xF7, OpAddressingMode::   ZeroPageX , 2, 6, CPU::OP_ISC },
			{ 0xEF, OpAddressingMode:: Absolute    , 3, 6, CPU::OP_ISC },
			{ 0xFF, OpAddressingMode::  AbsoluteX  , 3, 7, CPU::OP_ISC },
			{ 0xFB, OpAddressingMode::  AbsoluteY  , 3, 7, CPU::OP_ISC },
			{ 0xE3, OpAddressingMode::IndexIndirect, 2, 8, CPU::OP_ISC },
			{ 0xF3, OpAddressingMode::IndirectIndex, 2, 8, CPU::OP_ISC },
			{ 0x02, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x12, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x22, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x32, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x42, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x52, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x62, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x72, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0x92, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0xB2, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0xD2, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0xF2, OpAddressingMode::Implicit     , 1, 0, CPU::OP_KIL },
			{ 0xBB, OpAddressingMode::  AbsoluteY  , 3, 4, CPU::OP_LAR },
			{ 0xA7, OpAddressingMode::  ZeroPage   , 2, 3, CPU::OP_LAX },
			{ 0xB7, OpAddressingMode::   ZeroPageY , 2, 4, CPU::OP_LAX },
			{ 0xAF, OpAddressingMode:: Absolute    , 3, 4, CPU::OP_LAX },
			{ 0xBF, OpAddressingMode::  AbsoluteY  , 3, 4, CPU::OP_LAX },
			{ 0xA3, OpAddressingMode::IndexIndirect, 2, 6, CPU::OP_LAX },
			{ 0xB3, OpAddressingMode::IndirectIndex, 2, 5, CPU::OP_LAX },
			{ 0x1A, OpAddressingMode::Implicit     , 1, 2, CPU::OP_NOP },
			{ 0x3A, OpAddressingMode::Implicit     , 1, 2, CPU::OP_NOP },
			{ 0x5A, OpAddressingMode::Implicit     , 1, 2, CPU::OP_NOP },
			{ 0x7A, OpAddressingMode::Implicit     , 1, 2, CPU::OP_NOP },
			{ 0xDA, OpAddressingMode::Implicit     , 1, 2, CPU::OP_NOP },
			{ 0xFA, OpAddressingMode::Implicit     , 1, 2, CPU::OP_NOP },
			{ 0x27, OpAddressingMode::  ZeroPage   , 2, 5, CPU::OP_RLA },
			{ 0x37, OpAddressingMode::   ZeroPageX , 2, 6, CPU::OP_RLA },
			{ 0x2F, OpAddressingMode:: Absolute    , 3, 6, CPU::OP_RLA },
			{ 0x3F, OpAddressingMode::  AbsoluteX  , 3, 7, CPU::OP_RLA },
			{ 0x3B, OpAddressingMode::  AbsoluteY  , 3, 7, CPU::OP_RLA },
			{ 0x23, OpAddressingMode::IndexIndirect, 2, 8, CPU::OP_RLA },
			{ 0x33, OpAddressingMode::IndirectIndex, 2, 8, CPU::OP_RLA },
			{ 0x67, OpAddressingMode::  ZeroPage   , 2, 5, CPU::OP_RRA },
			{ 0x77, OpAddressingMode::   ZeroPageX , 2, 6, CPU::OP_RRA },
			{ 0x6F, OpAddressingMode:: Absolute    , 3, 6, CPU::OP_RRA },
			{ 0x7F, OpAddressingMode::  AbsoluteX  , 3, 7, CPU::OP_RRA },
			{ 0x7B, OpAddressingMode::  AbsoluteY  , 3, 7, CPU::OP_RRA },
			{ 0x63, OpAddressingMode::IndexIndirect, 2, 8, CPU::OP_RRA },
			{ 0x73, OpAddressingMode::IndirectIndex, 2, 8, CPU::OP_RRA },
			{ 0xEB, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_SBC },
			{ 0x07, OpAddressingMode::  ZeroPage   , 2, 5, CPU::OP_SLO },
			{ 0x17, OpAddressingMode::   ZeroPageX , 2, 6, CPU::OP_SLO },
			{ 0x0F, OpAddressingMode:: Absolute    , 3, 6, CPU::OP_SLO },
			{ 0x1F, OpAddressingMode::  AbsoluteX  , 3, 7, CPU::OP_SLO },
			{ 0x1B, OpAddressingMode::  AbsoluteY  , 3, 7, CPU::OP_SLO },
			{ 0x03, OpAddressingMode::IndexIndirect, 2, 8, CPU::OP_SLO },
			{ 0x13, OpAddressingMode::IndirectIndex, 2, 8, CPU::OP_SLO },
			{ 0x47, OpAddressingMode::  ZeroPage   , 2, 5, CPU::OP_SRE },
			{ 0x57, OpAddressingMode::   ZeroPageX , 2, 6, CPU::OP_SRE },
			{ 0x4F, OpAddressingMode:: Absolute    , 3, 6, CPU::OP_SRE },
			{ 0x5F, OpAddressingMode::  AbsoluteX  , 3, 7, CPU::OP_SRE },
			{ 0x5B, OpAddressingMode::  AbsoluteY  , 3, 7, CPU::OP_SRE },
			{ 0x43, OpAddressingMode::IndexIndirect, 2, 8, CPU::OP_SRE },
			{ 0x53, OpAddressingMode::IndirectIndex, 2, 8, CPU::OP_SRE },
			{ 0x9E, OpAddressingMode::  AbsoluteY  , 3, 5, CPU::OP_SXA },
			{ 0x9C, OpAddressingMode::  AbsoluteX  , 3, 5, CPU::OP_SYA },
			{ 0x0C, OpAddressingMode:: Absolute    , 3, 4, CPU::OP_TOP },
			{ 0x1C, OpAddressingMode::  AbsoluteX  , 3, 4, CPU::OP_TOP },
			{ 0x3C, OpAddressingMode::  AbsoluteX  , 3, 4, CPU::OP_TOP },
			{ 0x5C, OpAddressingMode::  AbsoluteX  , 3, 4, CPU::OP_TOP },
			{ 0x7C, OpAddressingMode::  AbsoluteX  , 3, 4, CPU::OP_TOP },
			{ 0xDC, OpAddressingMode::  AbsoluteX  , 3, 4, CPU::OP_TOP },
			{ 0xFC, OpAddressingMode::  AbsoluteX  , 3, 4, CPU::OP_TOP },
			{ 0x8B, OpAddressingMode:: Immediate   , 2, 2, CPU::OP_XAA },
			{ 0x9B, OpAddressingMode::  AbsoluteY  , 3, 5, CPU::OP_XAS }
	};


	static_assert(LEN(op_entity) == 256, "instruction count should be 256.");
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
			addr = (mem[(pointer + 1 + X) & 0xff] << 8) | mem[(pointer + X) & 0xff];
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
	uint8_t *operand = nullptr;
	bool crossed_page = false;
	FetchOperands(optable[op_code]->addressing_mode, operand, crossed_page);

	// 执行
	uint8_t cycle = ExeFunc(optable[op_code], this, operand, updated_pc, crossed_page);

	// 更新PC
	PC = updated_pc;
	cycles += cycle;

	return cycle;
}

// 参考手册： http://obelisk.me.uk/6502/reference.html
/**************** 指令实现区Begin ****************/
#define MorA (operand ? *operand:cpu->A)
#define Branch(field, value)    if (cpu->P.field == value) { \
									updated_pc += (int8_t) *operand; \
									return uint8_t(self.cycles + crossed_page + 1); \
								} \
								return self.cycles + crossed_page;
#define FixCycle self.cycles + crossed_page;
#define OpRM(R, op) cpu->R op *operand; \
					cpu->P.Negative = Sign(cpu->R); \
					cpu->P.Zero = cpu->R == 0; \
					return FixCycle;
#define TRNS(T,F)   T = F; \
					cpu->P.Negative = Sign(T); \
					cpu->P.Zero = T == 0; \
					return self.cycles;

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
	uint16_t result = MorA << 1;
	MorA <<= 1;

	cpu->P.Zero = ((result & 0xff) == 0);
	cpu->P.Carry = GetBit(result, 0x8);
	cpu->P.Negative = Sign(result);

	return self.cycles;
}

OpExeFuncDefine(OP_CLC) {
	/**
	 * CLC - Clear Carry Flag
	 * C = 0
	 * Set the carry flag to zero.
	 **/
	cpu->P.Carry = false;

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
	/**
	 * CLI - Clear Interrupt Disable
	 * I = 0
	 * Clears the interrupt disable flag allowing normal interrupt
	 * requests to be serviced.
	 **/

	cpu->P.IrqDisabled = false;
	return self.cycles;
}

OpExeFuncDefine(OP_CLV) {
	/**
	 * CLV - Clear Overflow Flag
	 * V = 0
	 * Clears the overflow flag.
	 **/

	cpu->P.Overflow = false;
	return self.cycles;
}

OpExeFuncDefine(OP_DEX) {
	/**
	 * DEX - Decrement X Register
	 * X,Z,N = X-1
	 * Subtracts one from the X register setting the zero and negative
	 * flags as appropriate.
	 **/

	--cpu->X;
	cpu->P.Zero = cpu->X == 0;
	cpu->P.Negative = Sign(cpu->X);

	return self.cycles;
}

OpExeFuncDefine(OP_DEY) {
	/**
	 * DEY - Decrement Y Register
	 * Y,Z,N = Y-1
	 * Subtracts one from the Y register setting the zero and negative
	 * flags as appropriate.
	 **/
	--cpu->Y;
	cpu->P.Zero = cpu->Y == 0;
	cpu->P.Negative = Sign(cpu->Y);

	return self.cycles;
}

OpExeFuncDefine(OP_INX) {
	/**
	 * INX - Increment X Register
	 * X,Z,N = X+1
	 * Adds one to the X register setting the zero and negative flags
	 * as appropriate.
	 **/
	++cpu->X;
	cpu->P.Zero = cpu->X == 0;
	cpu->P.Negative = Sign(cpu->X);

	return self.cycles;
}

OpExeFuncDefine(OP_INY) {
	/**
	 * INY - Increment Y Register
	 * Y,Z,N = Y+1
	 * Adds one to the Y register setting the zero and negative flags
	 * as appropriate.
	 **/
	++cpu->Y;
	cpu->P.Zero = cpu->Y == 0;
	cpu->P.Negative = Sign(cpu->Y);

	return self.cycles;
}

OpExeFuncDefine(OP_LSR) {
	/**
	 * LSR - Logical Shift Right
	 * A,C,Z,N = A/2 or M,C,Z,N = M/2
	 * Each of the bits in A or M is shift one place to the right.
	 * The bit that was in bit 0 is shifted into the carry flag. Bit
	 * 7 is set to zero.
	 **/
	auto result = uint8_t(MorA >> 1);
	cpu->P.Carry = GetBit(MorA, 0x0);
	cpu->P.Zero = result == 0;
	cpu->P.Negative = false;
	MorA = result;

	return self.cycles;
}

OpExeFuncDefine(OP_NOP) {
	/**
	 * NOP - No Operation
	 * The NOP instruction causes no changes to the processor other
	 * than the normal incrementing of the program counter to the next
	 * instruction.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ROL) {
	/**
	 * ROL - Rotate Left
	 * Move each of the bits in either A or M one place to the left.
	 * Bit 0 is filled with the current value of the carry flag whilst
	 * the old bit 7 becomes the new carry flag value.
	 **/
	auto result = uint8_t(MorA << 1) | cpu->P.Carry;
	cpu->P.Carry = GetBit(MorA, 0x7);
	cpu->P.Zero = result == 0;
	cpu->P.Negative = Sign(result);
	MorA = result;

	return self.cycles;
}

OpExeFuncDefine(OP_ROR) {
	/**
	 * ROR - Rotate Right
	 * Move each of the bits in either A or M one place to the right.
	 * Bit 7 is filled with the current value of the carry flag whilst
	 * the old bit 0 becomes the new carry flag value.
	 **/
	auto result = uint8_t(MorA >> 1) | (cpu->P.Carry) << 0x7;
	cpu->P.Carry = GetBit(MorA, 0);
	cpu->P.Zero = result == 0;
	cpu->P.Negative = Sign(result);
	MorA = result;

	return self.cycles;
}

OpExeFuncDefine(OP_SEC) {
	/**
	 * SEC - Set Carry Flag
	 * C = 1
	 * Set the carry flag to one.
	 **/
	cpu->P.Carry = true;

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
	/**
	 * SEI - Set Interrupt Disable
	 * I = 1
	 * Set the interrupt disable flag to one.
	 **/
	cpu->P.IrqDisabled = true;

	return self.cycles;
}

OpExeFuncDefine(OP_TAX) {
	/**
	 * TAX - Transfer Accumulator to X
	 * X = A
	 * Copies the current contents of the accumulator into the X register
	 * and sets the zero and negative flags as appropriate.
	 **/
	TRNS(cpu->X, cpu->A);
}

OpExeFuncDefine(OP_TAY) {
	/**
	 * TAY - Transfer Accumulator to Y
	 * Y = A
	 * Copies the current contents of the accumulator into the Y register
	 * and sets the zero and negative flags as appropriate.
	 **/
	TRNS(cpu->Y, cpu->A);
}

OpExeFuncDefine(OP_TSX) {
	/**
	 * TSX - Transfer Stack Pointer to X
	 * X = S
	 * Copies the current contents of the stack register into the
	 * X register and sets the zero and negative flags as appropriate.
	 **/
	TRNS(cpu->X, cpu->SP);
}

OpExeFuncDefine(OP_TXA) {
	/**
	 * TXA - Transfer X to Accumulator
	 * A = X
	 * Copies the current contents of the X register into the accumulator
	 * and sets the zero and negative flags as appropriate.
	 **/
	TRNS(cpu->A, cpu->X);
}

OpExeFuncDefine(OP_TXS) {
	/**
	 * TXS - Transfer X to Stack Pointer
	 * S = X
	 * Copies the current contents of the X register into the stack
	 * register.
	 **/
	// Note that TXS (Transfer X to S) is not an arithmetic operation
	cpu->SP = cpu->X;
	cpu->P.Negative = true;
	cpu->P.Zero = cpu->X == 0;
	return self.cycles;
}

OpExeFuncDefine(OP_TYA) {
	/**
	 * TYA - Transfer Y to Accumulator
	 * A = Y
	 * Copies the current contents of the Y register into the accumulator
	 * and sets the zero and negative flags as appropriate.
	 **/
	TRNS(cpu->A, cpu->Y);
}

OpExeFuncDefine(OP_PHA) {
	/**
	 * PHA - Push Accumulator
	 * Pushes a copy of the accumulator on to the stack.
	 **/
	cpu->Push(cpu->A);

	return self.cycles;
}

OpExeFuncDefine(OP_PHP) {
	/**
	 * PHP - Push Processor Status
	 * Pushes a copy of the status flags on to the stack.
	 **/
	cpu->Push(cpu->P | 0x10);

	return self.cycles;
}

OpExeFuncDefine(OP_PLA) {
	/**
	 * PLA - Pull Accumulator
	 * Pulls an 8 bit value from the stack and into the accumulator.
	 * The zero and negative flags are set as appropriate.
	 **/
	cpu->A = cpu->Pop();
	cpu->P.Negative = Sign(cpu->A);
	cpu->P.Zero = cpu->A == 0;

	return self.cycles;
}

OpExeFuncDefine(OP_PLP) {
	/**
	 * PLP - Pull Processor Status
	 * Pulls an 8 bit value from the stack and into the processor
	 * flags. The flags will take on new states as determined by the
	 * value pulled.
	 **/
	// Two instructions (PLP and RTI) pull a byte from the stack and set all the flags. They ignore bits 5 and 4.
	bool brk = cpu->P.BrkExecuted;
	cpu->P = cpu->Pop();
	cpu->P.BrkExecuted = brk;
	cpu->P.Invalid = true;

	return self.cycles;
}

OpExeFuncDefine(OP_RTI) {
	/**
	 * RTI - Return from Interrupt
	 * The RTI instruction is used at the end of an interrupt processing
	 * routine. It pulls the processor flags from the stack followed
	 * by the program counter.
	 **/
	// Two instructions (PLP and RTI) pull a byte from the stack and set all the flags. They ignore bits 5 and 4.
	bool brk = cpu->P.BrkExecuted;
	cpu->P = cpu->Pop();
	cpu->P.BrkExecuted = brk;
	cpu->P.Invalid = true;

	uint8_t PCL = cpu->Pop(), PCH = cpu->Pop();
	updated_pc = (PCH << 0x8) | PCL;

	return self.cycles;
}

OpExeFuncDefine(OP_RTS) {
	/**
	 * RTS - Return from Subroutine
	 * The RTS instruction is used at the end of a subroutine to return
	 * to the calling routine. It pulls the program counter (minus one)
	 * from the stack.
	 **/
	uint8_t PCL = cpu->Pop(), PCH = cpu->Pop();
	updated_pc = (PCH << 0x8) | PCL;
	++updated_pc;


	return self.cycles;
}

OpExeFuncDefine(OP_BRK) {
	/**
	 * BRK - Force Interrupt
	 * The BRK instruction forces the generation of an interrupt request.
	 * The program counter and processor status are pushed on the stack
	 * then the IRQ interrupt vector at $FFFE/F is loaded into the PC
	 * and the break flag in the status set to one.
	 **/
	auto    PCH = uint8_t( ( (updated_pc) >> 0x8) & 0xff),
			PCL = uint8_t((updated_pc) & 0xff);
	cpu->Push(PCH);
	cpu->Push(PCL);
	cpu->P.BrkExecuted = true;
	cpu->Push(cpu->P);
	cpu->P.IrqDisabled = true;
	updated_pc = cpu->Read16(static_cast<uint16_t>(CPU::InterruptVector::IRQ));

	return self.cycles;
}

OpExeFuncDefine(OP_BCC) {
	/**
	 * BCC - Branch if Carry Clear
	 * If the carry flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Carry, false);
}

OpExeFuncDefine(OP_BCS) {
	/**
	 * BCS - Branch if Carry Set
	 * If the carry flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Carry, true);
}

OpExeFuncDefine(OP_BEQ) {
	/**
	 * BEQ - Branch if Equal
	 * If the zero flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Zero, true);
}

OpExeFuncDefine(OP_BMI) {
	/**
	 * BMI - Branch if Minus
	 * If the negative flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Negative, true);
}

OpExeFuncDefine(OP_BNE) {
	/**
	 * BNE - Branch if Not Equal
	 * If the zero flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Zero, false);
}

OpExeFuncDefine(OP_BPL) {
	/**
	 * BPL - Branch if Positive
	 * If the negative flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Negative, false);
}

OpExeFuncDefine(OP_BVC) {
	/**
	 * BVC - Branch if Overflow Clear
	 * If the overflow flag is clear then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Overflow, false);
}

OpExeFuncDefine(OP_BVS) {
	/**
	 * BVS - Branch if Overflow Set
	 * If the overflow flag is set then add the relative displacement
	 * to the program counter to cause a branch to a new location.
	 **/

	Branch(Overflow, true);
}

OpExeFuncDefine(OP_ADC) {
	/**
	 * ADC - Add with Carry
	 * A,Z,C,N = A+M+C
	 * This instruction adds the contents of a memory location to
	 * the accumulator together with the carry bit. If overflow occurs
	 * the carry bit is set, this enables multiple byte addition to be
	 * performed.
	 **/
	uint16_t result = uint16_t(cpu->A) + uint16_t(*operand) + cpu->P.Carry;
	cpu->P.Overflow = GetBit(result, 0x8) ^
	                  GetBit( (cpu->A & uint8_t(0x7f)) + (*operand & uint8_t(0x7f)) + cpu->P.Carry, 0x7); // for signed number
	cpu->P.Carry = GetBit(result, 0x8); // for usigned number
	cpu->P.Zero = (result & 0xff) == 0;
	cpu->P.Negative = Sign(uint8_t(result));

	cpu->A = uint8_t(result);

	return FixCycle;
}

OpExeFuncDefine(OP_AND) {
	/**
	 * AND - Logical AND
	 * A,Z,N = A&M
	 * A logical AND is performed, bit by bit, on the accumulator
	 * contents using the contents of a byte of memory.
	 **/
	OpRM(A, &=);
}

OpExeFuncDefine(OP_CMP) {
	/**
	 * CMP - Compare
	 * Z,C,N = A-M
	 * This instruction compares the contents of the accumulator with
	 * another memory held value and sets the zero and carry flags as
	 * appropriate.
	 **/
	cpu->P.Carry = cpu->A >= *operand;
	cpu->P.Zero = cpu->A == *operand;
	cpu->P.Negative = Sign(cpu->A - *operand);

	return FixCycle;
}

OpExeFuncDefine(OP_CPX) {
	/**
	 * CPX - Compare X Register
	 * Z,C,N = X-M
	 * This instruction compares the contents of the X register with
	 * another memory held value and sets the zero and carry flags as
	 * appropriate.
	 **/
	cpu->P.Carry = cpu->X >= *operand;
	cpu->P.Zero = cpu->X == *operand;
	cpu->P.Negative = Sign(cpu->X - *operand);

	return self.cycles;
}

OpExeFuncDefine(OP_CPY) {
	/**
	 * CPY - Compare Y Register
	 * Z,C,N = Y-M
	 * This instruction compares the contents of the Y register with
	 * another memory held value and sets the zero and carry flags as
	 * appropriate.
	 **/
	cpu->P.Carry = cpu->Y >= *operand;
	cpu->P.Zero = cpu->Y == *operand;
	cpu->P.Negative = Sign(cpu->Y - *operand);

	return self.cycles;
}

OpExeFuncDefine(OP_EOR) {
	/**
	 * EOR - Exclusive OR
	 * A,Z,N = A^M
	 * An exclusive OR is performed, bit by bit, on the accumulator
	 * contents using the contents of a byte of memory.
	 **/
	OpRM(A, ^=);
}

OpExeFuncDefine(OP_LDA) {
	/**
	 * LDA - Load Accumulator
	 * A,Z,N = M
	 * Loads a byte of memory into the accumulator setting the zero
	 * and negative flags as appropriate.
	 **/
	OpRM(A, =);
}

OpExeFuncDefine(OP_LDX) {
	/**
	 * LDX - Load X Register
	 * X,Z,N = M
	 * Loads a byte of memory into the X register setting the zero
	 * and negative flags as appropriate.
	 **/

	OpRM(X, =);
}

OpExeFuncDefine(OP_LDY) {
	/**
	 * LDY - Load Y Register
	 * Y,Z,N = M
	 * Loads a byte of memory into the Y register setting the zero
	 * and negative flags as appropriate.
	 **/
	OpRM(Y, =);

}

OpExeFuncDefine(OP_ORA) {
	/**
	 * ORA - Logical Inclusive OR
	 * A,Z,N = A|M
	 * An inclusive OR is performed, bit by bit, on the accumulator
	 * contents using the contents of a byte of memory.
	 **/

	OpRM(A, |=);
}

OpExeFuncDefine(OP_SBC) {
	/**
	 * SBC - Subtract with Carry
	 * A,Z,C,N = A-M-(1-C)
	 * This instruction subtracts the contents of a memory location
	 * to the accumulator together with the not of the carry bit. If
	 * overflow occurs the carry bit is clear, this enables multiple
	 * byte subtraction to be performed.
	 **/

	// 不能直接A-M-(1-C)，直接减有坑，需要把减法换成加法运算
	// A-M-(1-C) = A-M-1+C = A-(M+1)+C = A + [~(M+1) + 1] + C = A + ~M + C

	auto tmpOperand = uint8_t(~*operand); // 将tmpOperand取-(*operand+1)
	uint16_t result = uint16_t(cpu->A) + uint16_t(tmpOperand) + cpu->P.Carry;
	cpu->P.Overflow = GetBit(result, 0x8) ^
	                  GetBit( (cpu->A & uint8_t(0x7f)) + (tmpOperand & uint8_t(0x7f)) + cpu->P.Carry, 0x7); // for signed number
	cpu->P.Carry = GetBit(result,  0x8); // for usigned number
	cpu->P.Zero = (result & 0xff) == 0;
	cpu->P.Negative = Sign(uint8_t(result));
	cpu->A = uint8_t(result);

	return FixCycle;

}

OpExeFuncDefine(OP_BIT) {
	/**
	 * BIT - Bit Test
	 * A & M, N = M7, V = M6
	 * This instructions is used to test if one or more bits are set
	 * in a target memory location. The mask pattern in A is ANDed with
	 * the value in memory to set or clear the zero flag, but the result
	 * is not kept. Bits 7 and 6 of the value from memory are copied
	 * into the N and V flags.
	 **/
	cpu->P.Negative = GetBit(*operand, 7);
	cpu->P.Overflow = GetBit(*operand, 6);
	cpu->P.Zero = (cpu->A & *operand) == 0;

	return self.cycles;
}

OpExeFuncDefine(OP_STA) {
	/**
	 * STA - Store Accumulator
	 * M = A
	 * Stores the contents of the accumulator into memory.
	 **/

	*operand = cpu->A;
	return self.cycles;
}

OpExeFuncDefine(OP_STX) {
	/**
	 * STX - Store X Register
	 * M = X
	 * Stores the contents of the X register into memory.
	 **/

	*operand = cpu->X;
	return self.cycles;
}

OpExeFuncDefine(OP_STY) {
	/**
	 * STY - Store Y Register
	 * M = Y
	 * Stores the contents of the Y register into memory.
	 **/

	*operand = cpu->Y;
	return self.cycles;
}

OpExeFuncDefine(OP_DEC) {
	/**
	 * DEC - Decrement Memory
	 * M,Z,N = M-1
	 * Subtracts one from the value held at a specified memory location
	 * setting the zero and negative flags as appropriate.
	 **/

	--*operand;
	cpu->P.Zero = *operand == 0;
	cpu->P.Negative = Sign(*operand);

	return self.cycles;
}

OpExeFuncDefine(OP_INC) {
	/**
	 * INC - Increment Memory
	 * M,Z,N = M+1
	 * Adds one to the value held at a specified memory location setting
	 * the zero and negative flags as appropriate.
	 **/
	++*operand;
	cpu->P.Zero = *operand == 0;
	cpu->P.Negative = Sign(*operand);

	return self.cycles;
}

OpExeFuncDefine(OP_JMP) {
	/**
	 * JMP - Jump
	 * Sets the program counter to the address specified by the operand.
	 **/

	if(operand) // Absolute addressing
		updated_pc = cpu->Read16(uint16_t(cpu->PC+1));
	else { // Indirect addressing
		uint16_t indirect_address = cpu->Read16(uint16_t(cpu->PC+1));
		auto PCH = uint8_t((indirect_address & 0xff00) >> 0x8),
				PCL = uint8_t(indirect_address & 0xff);
		updated_pc = (cpu->Read8(indirect_address)) |
					 (cpu->Read8(PCH << 0x8 | uint8_t(PCL + 1) ) << 0x8);
	}

	return self.cycles;
}

OpExeFuncDefine(OP_JSR) {
	/**
	 * JSR - Jump to Subroutine
	 * The JSR instruction pushes the address (minus one) of the return
	 * point on to the stack and then sets the program counter to the
	 * target memory address.
	 **/
	auto    PCH = uint8_t( ( (updated_pc - 1) >> 0x8) & 0xff),
			PCL = uint8_t( (updated_pc - 1) & 0xff);
	cpu->Push(PCH);
	cpu->Push(PCL);
	updated_pc = cpu->Read16(uint16_t(cpu->PC + 1));

	return self.cycles;
}

// Undocumented OP
OpExeFuncDefine(OP_KIL) {
	// TODO: wait for implements: KIL
	/**
	 * KIL
	 * Stop program counter (processor lock up).
	 * Status flags: -
	 **/

	return self.cycles;
}


OpExeFuncDefine(OP_AAC) {
	// TODO: wait for implements: AAC
	/**
	 * AAC
	 * AND byte with accumulator. If result is negative then carry is set.
	 * Status flags: N,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ARR) {
	// TODO: wait for implements: ARR
	/**
	 * ARR
	 * AND byte with accumulator, then rotate one bit right in accu-mulator and
	 * check bit 5 and 6:
	 * If both bits are 1: set C, clear V.
	 * If both bits are 0: clear C and V.
	 * If only bit 5 is 1: set V, clear C.
	 * If only bit 6 is 1: set C and V.
	 * Status flags: N,V,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ASR) {
	// TODO: wait for implements: ASR
	/**
	 * ASR
	 * AND byte with accumulator, then shift right one bit in accumu-lator.
	 * Status flags: N,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ATX) {
	// TODO: wait for implements: ATX
	/**
	 * ATX
	 * AND byte with accumulator, then transfer accumulator to X register.
	 * Status flags: N,Z
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_AXS) {
	// TODO: wait for implements: AXS
	/**
	 * AXS
	 * AND X register with accumulator and store result in X regis-ter, then
	 * subtract byte from X register (without borrow).
	 * Status flags: N,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_DOP) {
	// TODO: wait for implements: DOP
	/**
	 * DOP
	 * No operation (double NOP). The argument has no signifi-cance. Status
	 * flags: -
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_XAA) {
	// TODO: wait for implements: XAA
	/**
	 * XAA
	 * Exact operation unknown. Read the referenced documents for more
	 * information and observations.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_AAX) {
	// TODO: wait for implements: AAX
	/**
	 * AAX
	 * AND X register with accumulator and store result in memory. Status
	 * flags: N,Z
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_LAX) {
	// TODO: wait for implements: LAX
	/**
	 * LAX
	 * Load accumulator and X register with memory.
	 * Status flags: N,Z
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_DCP) {
	// TODO: wait for implements: DCP
	/**
	 * DCP
	 * Subtract 1 from memory (without borrow).
	 * Status flags: C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_ISC) {
	// TODO: wait for implements: ISC
	/**
	 * ISC
	 * Increase memory by one, then subtract memory from accu-mulator (with
	 * borrow). Status flags: N,V,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_RLA) {
	// TODO: wait for implements: RLA
	/**
	 * RLA
	 * Rotate one bit left in memory, then AND accumulator with memory. Status
	 * flags: N,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_RRA) {
	// TODO: wait for implements: RRA
	/**
	 * RRA
	 * Rotate one bit right in memory, then add memory to accumulator (with
	 * carry).
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SLO) {
	// TODO: wait for implements: SLO
	/**
	 * SLO
	 * Shift left one bit in memory, then OR accumulator with memory. =
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SRE) {
	// TODO: wait for implements: SRE
	/**
	 * SRE
	 * Shift right one bit in memory, then EOR accumulator with memory. Status
	 * flags: N,Z,C
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_AXA) {
	// TODO: wait for implements: AXA
	/**
	 * AXA
	 * AND X register with accumulator then AND result with 7 and store in
	 * memory. Status flags: -
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_LAR) {
	// TODO: wait for implements: LAR
	/**
	 * LAR
	 * AND memory with stack pointer, transfer result to accu-mulator, X
	 * register and stack pointer.
	 * Status flags: N,Z
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_TOP) {
	// TODO: wait for implements: TOP
	/**
	 * TOP
	 * No operation (tripple NOP). The argument has no signifi-cance. Status
	 * flags: -
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SXA) {
	// TODO: wait for implements: SXA
	/**
	 * SXA
	 * AND X register with the high byte of the target address of the argument
	 * + 1. Store the result in memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_SYA) {
	// TODO: wait for implements: SYA
	/**
	 * SYA
	 * AND Y register with the high byte of the target address of the argument
	 * + 1. Store the result in memory.
	 **/

	return self.cycles;
}

OpExeFuncDefine(OP_XAS) {
	// TODO: wait for implements: XAS
	/**
	 * XAS
	 * AND X register with accumulator and store result in stack pointer, then
	 * AND stack pointer with the high byte of the target address of the
	 * argument + 1. Store result in memory.
	 **/

	return self.cycles;
}


/****************  指令实现区End  ****************/


