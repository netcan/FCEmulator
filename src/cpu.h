/****************************************************************************
 > File Name: cpu.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/

#ifndef FCEMU_CPU_H
#define FCEMU_CPU_H
#include <cstdint>

class CPU {
public:
	enum class opcodeType {
		// 56个操作码
		ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
		CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
		JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
		RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
	};
	enum class adressingMode {
		implicit,						// 没有操作数
		accumulator, 					// A寄存器作为操作数
		immediate,						// 8位立即数寻址
		zeroPage, zeroPageX, zeroPageY, // 零页寻址，+X后寻址，+Y后寻址，范围$0000-$00ff，高字节不进位
		relative, 						// 相对寻址，用于分支指令，PC += range(-128, 127)
		absolute, absoluteX, absoluteY,	// 16位寻址，+X后寻址，+Y后寻址
		indirect, 						// 16位地址间接寻址，JMP指令
		indexIndirect,					// 零页地址+X后（高字节不进位）间接寻址
		indirectIndex					// 零页间接寻址+Y得到新地址
	};
private:
	uint16_t PC; // 程序计数器
	uint8_t SP; // 栈指针，$0100-$01ff
	uint8_t A, X, Y; // 累加器，X，Y寄存器

};

#endif //FCEMU_CPU_H
