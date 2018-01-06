/****************************************************************************
 > File Name: cpu.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/
#pragma once
#include "Base.h"
#include "Ppu.h"

#define OpExeFuncArgs const Operation& self, CPU *cpu, uint8_t *operand, uint16_t& updated_pc, bool &crossed_page
#define OpExeFuncDecl(func_name) static uint8_t func_name(OpExeFuncArgs)
#define OpExeFuncDefine(func_name) uint8_t CPU::func_name(OpExeFuncArgs)
#define ExeFunc(op_code_entity, cpu, oprand, updated_pc, crossed_page) op_code_entity->exe(*op_code_entity, cpu, oprand, updated_pc, crossed_page)
inline bool Sign(uint8_t x) { return GetBit(x, 7); };

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
	inline void ClearBit() { P &= ~(1u << pos); }

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
	PSField<5> Invalid;       // 无效位
	PSField<3> Decimal;       // 6502通过设置该标志位切换到BCD模式，由于2A03不支持BCD，所以这位是无效的
	PSField<2> IrqDisabled;   // 通过设置该位可以屏蔽IRQ中断
	PSField<1> Zero;          // 当运算结果为0的时候置位
	PSField<0> Carry;         // 当运算结果最高位第7位符号翻转的时候置位
private:
	uint8_t P;
};


class __CPUMem__ {
private:
	uint8_t tmp;
	uint8_t           Ram[0x800]; // 0x0000-0x07ff
	uint8_t    *PPURegister[0x8]; // 0x2000-0x2007
	uint8_t    *IORegister[0x20]; // 0x4000-0x401f, many for register of pAPU
	uint8_t ExpansionRom[0x1fe0]; // 0x4020-0x5fff
	uint8_t         SRam[0x2000]; // 0x6000-0x7fff
	uint8_t  LowerPRGRom[0x4000]; // 0x8000-0xbfff
	uint8_t  UpperPRGRom[0x4000]; // 0xc000-0xffff

	inline const uint8_t &AT(uint16_t addr) const {
			return  addr < 0x2000 ? Ram[addr & 0x7ff]:
					addr < 0x4000 ? (PPURegister[addr & 0x7] ? *PPURegister[addr & 0x7] : tmp):
					addr < 0x4020 ? (IORegister[(addr - 0x4000)] ? *IORegister[(addr - 0x4000)] : tmp):
					addr < 0x6000 ? ExpansionRom[addr - 0x4020]:
					addr < 0x8000 ? SRam[addr - 0x6000]:
					addr < 0xc000 ? LowerPRGRom[addr - 0x8000]:
									UpperPRGRom[addr - 0xc000];
	}

	inline uint8_t &AT(uint16_t addr) { // 复用const版本
		return const_cast<uint8_t &>(
				static_cast<const __CPUMem__&>(*this).AT(addr)
		);
	}

public:
	uint8_t &operator[](uint16_t addr) { return AT(addr); }
	const uint8_t &operator[](uint16_t addr) const { return AT(addr); }

	using iterator = MemIterator<__CPUMem__>;
	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, 0x10000); }

	void PPURegisterMapping(PPU &ppu) {
		PPURegister[0x00] = &ppu.PPUCTRL;   // 0x2000, Write
		PPURegister[0x01] = &ppu.PPUMASK;   // 0x2001, Write
		PPURegister[0x02] = &ppu.PPUSTATUS; // 0x2002, Read
		PPURegister[0x03] = &ppu.OAMADDR;   // 0x2003, Write
		PPURegister[0x04] = &ppu.OAMDATA;   // 0x2004, Read/Write
		PPURegister[0x05] = &ppu.PPUSCROLL; // 0x2005, Write twice for x and y
		PPURegister[0x06] = &ppu.PPUADDR;   // 0x2006, Write twice for upper and lower address
		PPURegister[0x07] = &ppu.PPUDATA;   // 0x2007, Read/Write, read buffer(post-fetch)
		 IORegister[0x14] = &ppu.OAMDMA;    // 0x4014, Write
	}
};

struct Operation;

class CPU {
public:
	friend class Cartridge;
	friend struct Operation;

	CPU() : P(0x34), A(0), X(0), Y(0), SP(0xfd), cycles(0) { }; // Power Up
	CPU(PPU &ppu) : CPU() { mem.PPURegisterMapping(ppu); }
	// 读取一个字节
	inline uint8_t Read8(uint16_t addr) const { return mem[addr]; }
	// 读取一个字
	inline uint16_t Read16(uint16_t addr) const { return (mem[addr + 1] << 8) | mem[addr]; }
	// 写一个字节
	inline void Write(uint16_t addr, uint8_t value) { mem[addr] = value; }
	// 入栈
	inline void Push(uint8_t value) { mem[0x100 | (SP--)] = value; }
	// 出栈
	inline uint8_t Pop() { return mem[0x100 | (++SP)]; }

	inline void Reset() { PC = Read16(static_cast<uint16_t>(InterruptVector::Reset)); }
	uint8_t Execute();
#ifndef NDEBUG
	ProcessorStatus &getP() { return P; }
	uint8_t &getA() { return A; }
	uint8_t &getX() { return X; }
	uint8_t &getY() { return Y; }
	uint8_t &getSP() { return SP; }
	uint16_t &getPC() { return PC; }
	uint32_t getCycles() { return cycles; }
#endif
private:
	uint16_t PC; // 程序计数器
	uint8_t SP; // 栈指针，$0100-$01ff
	uint8_t A, X, Y; // 累加器，X，Y寄存器
	ProcessorStatus P; // 状态寄存器
	__CPUMem__ mem;
	uint32_t cycles; // 累计执行周期
	static const Operation** InitOptable();
	static const Operation **optable;

	enum class OpAddressingMode {
		Implicit,                       // 没有操作数
		Accumulator,                    // A寄存器作为操作数
		Immediate,                      // 8位立即数寻址
		ZeroPage, ZeroPageX, ZeroPageY, // 零页寻址，+X后寻址，+Y后寻址，范围$0000-$00ff，高字节不进位
		Relative,                       // 相对寻址，用于分支指令，PC += range(-128, 127)
		Absolute, AbsoluteX, AbsoluteY, // 16位寻址，+X后寻址，+Y后寻址
		Indirect,                       // 16位地址间接寻址，JMP指令
		IndexIndirect,                  // 零页地址+X后（高字节不进位）间接寻址（IDX）
		IndirectIndex                   // 零页间接寻址，+Y得到新地址（IDY）
	};

	enum class InterruptVector: uint16_t {
		NMI = 0xfffa,
		Reset = 0xfffc,
		IRQ = 0xfffe
	};

	void FetchOperands(OpAddressingMode addressing_mode, uint8_t *& oprand, bool &crossed_page);

/**************** 指令声明区Begin ****************/
	OpExeFuncDecl(OP_ASL); // ASL - Arithmetic Shift Left
	OpExeFuncDecl(OP_CLC); // CLC - Clear Carry Flag
	OpExeFuncDecl(OP_CLD); // CLD - Clear Decimal Mode
	OpExeFuncDecl(OP_CLI); // CLI - Clear Interrupt Disable
	OpExeFuncDecl(OP_CLV); // CLV - Clear Overflow Flag
	OpExeFuncDecl(OP_DEX); // DEX - Decrement X Register
	OpExeFuncDecl(OP_DEY); // DEY - Decrement Y Register
	OpExeFuncDecl(OP_INX); // INX - Increment X Register
	OpExeFuncDecl(OP_INY); // INY - Increment Y Register
	OpExeFuncDecl(OP_LSR); // LSR - Logical Shift Right
	OpExeFuncDecl(OP_NOP); // NOP - No Operation
	OpExeFuncDecl(OP_ROL); // ROL - Rotate Left
	OpExeFuncDecl(OP_ROR); // ROR - Rotate Right
	OpExeFuncDecl(OP_SEC); // SEC - Set Carry Flag
	OpExeFuncDecl(OP_SED); // SED - Set Decimal Flag
	OpExeFuncDecl(OP_SEI); // SEI - Set Interrupt Disable
	OpExeFuncDecl(OP_TAX); // TAX - Transfer Accumulator to X
	OpExeFuncDecl(OP_TAY); // TAY - Transfer Accumulator to Y
	OpExeFuncDecl(OP_TSX); // TSX - Transfer Stack Pointer to X
	OpExeFuncDecl(OP_TXA); // TXA - Transfer X to Accumulator
	OpExeFuncDecl(OP_TXS); // TXS - Transfer X to Stack Pointer
	OpExeFuncDecl(OP_TYA); // TYA - Transfer Y to Accumulator
	OpExeFuncDecl(OP_PHA); // PHA - Push Accumulator
	OpExeFuncDecl(OP_PHP); // PHP - Push Processor Status
	OpExeFuncDecl(OP_PLA); // PLA - Pull Accumulator
	OpExeFuncDecl(OP_PLP); // PLP - Pull Processor Status
	OpExeFuncDecl(OP_RTI); // RTI - Return from Interrupt
	OpExeFuncDecl(OP_RTS); // RTS - Return from Subroutine
	OpExeFuncDecl(OP_BRK); // BRK - Force Interrupt
	OpExeFuncDecl(OP_BCC); // BCC - Branch if Carry Clear
	OpExeFuncDecl(OP_BCS); // BCS - Branch if Carry Set
	OpExeFuncDecl(OP_BEQ); // BEQ - Branch if Equal
	OpExeFuncDecl(OP_BMI); // BMI - Branch if Minus
	OpExeFuncDecl(OP_BNE); // BNE - Branch if Not Equal
	OpExeFuncDecl(OP_BPL); // BPL - Branch if Positive
	OpExeFuncDecl(OP_BVC); // BVC - Branch if Overflow Clear
	OpExeFuncDecl(OP_BVS); // BVS - Branch if Overflow Set
	OpExeFuncDecl(OP_ADC); // ADC - Add with Carry
	OpExeFuncDecl(OP_AND); // AND - Logical AND
	OpExeFuncDecl(OP_CMP); // CMP - Compare
	OpExeFuncDecl(OP_CPX); // CPX - Compare X Register
	OpExeFuncDecl(OP_CPY); // CPY - Compare Y Register
	OpExeFuncDecl(OP_EOR); // EOR - Exclusive OR
	OpExeFuncDecl(OP_LDA); // LDA - Load Accumulator
	OpExeFuncDecl(OP_LDX); // LDX - Load X Register
	OpExeFuncDecl(OP_LDY); // LDY - Load Y Register
	OpExeFuncDecl(OP_ORA); // ORA - Logical Inclusive OR
	OpExeFuncDecl(OP_SBC); // SBC - Subtract with Carry
	OpExeFuncDecl(OP_BIT); // BIT - Bit Test
	OpExeFuncDecl(OP_STA); // STA - Store Accumulator
	OpExeFuncDecl(OP_STX); // STX - Store X Register
	OpExeFuncDecl(OP_STY); // STY - Store Y Register
	OpExeFuncDecl(OP_DEC); // DEC - Decrement Memory
	OpExeFuncDecl(OP_INC); // INC - Increment Memory
	OpExeFuncDecl(OP_JMP); // JMP - Jump
	OpExeFuncDecl(OP_JSR); // JSR - Jump to Subroutine

	// Undocumented op
	OpExeFuncDecl(OP_KIL); // KIL
	OpExeFuncDecl(OP_AAC); // AAC
	OpExeFuncDecl(OP_ARR); // ARR
	OpExeFuncDecl(OP_ASR); // ASR
	OpExeFuncDecl(OP_ATX); // ATX
	OpExeFuncDecl(OP_AXS); // AXS
	OpExeFuncDecl(OP_DOP); // DOP
	OpExeFuncDecl(OP_XAA); // XAA
	OpExeFuncDecl(OP_AAX); // AAX
	OpExeFuncDecl(OP_LAX); // LAX
	OpExeFuncDecl(OP_DCP); // DCP
	OpExeFuncDecl(OP_ISC); // ISC
	OpExeFuncDecl(OP_RLA); // RLA
	OpExeFuncDecl(OP_RRA); // RRA
	OpExeFuncDecl(OP_SLO); // SLO
	OpExeFuncDecl(OP_SRE); // SRE
	OpExeFuncDecl(OP_AXA); // AXA
	OpExeFuncDecl(OP_LAR); // LAR
	OpExeFuncDecl(OP_TOP); // TOP
	OpExeFuncDecl(OP_SXA); // SXA
	OpExeFuncDecl(OP_SYA); // SYA
	OpExeFuncDecl(OP_XAS); // XAS
/****************  指令声明区End  ****************/


};

struct Operation { // 指令
	using ExeFunc = uint8_t (*)(OpExeFuncArgs);
	uint8_t code;
	CPU::OpAddressingMode addressing_mode;
	uint8_t bytes, cycles;
	ExeFunc exe; // 返回具体执行的cycles数目
};

