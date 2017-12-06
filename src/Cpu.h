/****************************************************************************
 > File Name: cpu.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/

#ifndef FCEMU_CPU_H
#define FCEMU_CPU_H
#include "Base.h"

class ProcessorStatus;

template <size_t pos>
class PSField {
public:
	PSField(uint8_t& P) : P(P) {}
	inline PSField &operator=(bool b) {
		assert(pos >= 0 && pos < 8);
		b ? SetBit() : ClearBit();
		return *this;
	}
	inline operator bool() const { return static_cast<bool>((P >> pos) & 1); };
	inline void SetBit() { P |= 1u << pos; }
	inline void ClearBit() { P &= ~1u << pos; }

private:
	uint8_t &P;
};

class ProcessorStatus {
public:
	ProcessorStatus(uint8_t value = 0x34);
	inline operator uint8_t() const { return P; }
	inline ProcessorStatus&operator=(uint8_t value) {
		P = value;
		return *this;
	}

	PSField<7> Negative;      // 当运算结果最高位第7位为1的时候置位，表明负数
	PSField<6> Overflow;      // 当两个补码运算产生非法的结果置位，例如正+正为负的时候
	PSField<4> BrkExecuted;   // 用于标记当BRK指令执行后，产生的IRQ中断（软件中断）
	PSField<3> Decimal;       // 6502通过设置该标志位切换到BCD模式，由于2A03不支持BCD，所以这位是无效的
	PSField<2> IrqDisabled;   // 通过设置该位可以屏蔽IRQ中断
	PSField<1> Zero;          // 当运算结果为0的时候置位
	PSField<0> Carry;         // 当运算结果最高位第7位符号翻转的时候置位
private:
	uint8_t P;
};


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
		implicit,                       // 没有操作数
		accumulator,                    // A寄存器作为操作数
		immediate,                      // 8位立即数寻址
		zeroPage, zeroPageX, zeroPageY, // 零页寻址，+X后寻址，+Y后寻址，范围$0000-$00ff，高字节不进位
		relative,                       // 相对寻址，用于分支指令，PC += range(-128, 127)
		absolute, absoluteX, absoluteY, // 16位寻址，+X后寻址，+Y后寻址
		indirect,                       // 16位地址间接寻址，JMP指令
		indexIndirect,                  // 零页地址+X后（高字节不进位）间接寻址
		indirectIndex                   // 零页间接寻址+Y得到新地址
	};

	CPU(): P(0x34), A(0), X(0), Y(0), SP(0xfd) {}; // Power Up

	inline uint8_t Read8(uint16_t addr) const { // 读取一个字节
		return Mem[addr];
	}
	inline uint16_t Read16(uint16_t addr) const { // 读取一个字
		return (Mem[addr + 1] << 8) | Mem[addr];
	}
	inline void Write(uint16_t addr, uint8_t value) { // 写一个字节
		Mem[addr] = value;
	}

private:
	uint16_t PC; // 程序计数器
	uint8_t SP; // 栈指针，$0100-$01ff
	uint8_t A, X, Y; // 累加器，X，Y寄存器
	ProcessorStatus P; // 状态寄存器
	struct __CPUMem__ {
		uint8_t Ram[0x800]; // 0x0000-0x07ff
		uint8_t PPURegister[0x8]; // 0x2000-0x2007
		uint8_t APUIORegister[0x20]; // 0x4000-0x401f
		uint8_t ExpansionRom[0x1fe0]; // 0x4020-0x5fff
		uint8_t SRam[0x2000]; // 0x6000-0x7fff
		uint8_t LowerPRGRom[0x4000]; // 0x8000-0xbfff
		uint8_t UpperPRGRom[0x4000]; // 0xc000-0xffff

#define AT(addr) (addr < 0x2000 ? Ram[addr % 0x800]:        \
		addr < 0x4000 ? PPURegister[(addr - 0x2000) % 0x8]: \
		addr < 0x4020 ? APUIORegister[(addr - 0x4000)]:     \
		addr < 0x6000 ? ExpansionRom[addr - 0x4020]:        \
		addr < 0x8000 ? SRam[addr - 0x6000]:                \
		addr < 0xc000 ? LowerPRGRom[addr - 0x8000]:         \
		UpperPRGRom[addr - 0xc000]);

		inline uint8_t &operator[](uint16_t addr) { // 写
			return AT(addr);
		}

		inline const uint8_t &operator[](uint16_t addr) const { // 读
			return AT(addr);
		}
	} Mem;


};

#endif //FCEMU_CPU_H
