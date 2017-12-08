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
#define OpExeFuncDecl(func_name) static uint8_t func_name(const Operation& self, ProcessorStatus &P, uint8_t *operand1, uint8_t *operand2)
#define OpExeFuncDefine(func_name) uint8_t CPU::func_name(const Operation& self, ProcessorStatus &P, uint8_t *operand1, uint8_t *operand2)
#define ExeFunc(op_code_entity, P, oprand1, oprand2) op_code_entity->exe(*op_code_entity, P, oprand1, oprand2)

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

class __CPUMem__ {
private:
	uint8_t Ram[0x800]; // 0x0000-0x07ff
	uint8_t PPURegister[0x8]; // 0x2000-0x2007
	uint8_t APUIORegister[0x20]; // 0x4000-0x401f
	uint8_t ExpansionRom[0x1fe0]; // 0x4020-0x5fff
	uint8_t SRam[0x2000]; // 0x6000-0x7fff
	uint8_t LowerPRGRom[0x4000]; // 0x8000-0xbfff
	uint8_t UpperPRGRom[0x4000]; // 0xc000-0xffff

#define AT(addr) (addr < 0x2000 ? Ram[addr & 0x7ff]:        \
		addr < 0x4000 ? PPURegister[(addr - 0x2000) & 0x7]: \
		addr < 0x4020 ? APUIORegister[(addr - 0x4000)]:     \
		addr < 0x6000 ? ExpansionRom[addr - 0x4020]:        \
		addr < 0x8000 ? SRam[addr - 0x6000]:                \
		addr < 0xc000 ? LowerPRGRom[addr - 0x8000]:         \
		UpperPRGRom[addr - 0xc000]);

public:
	class iterator: public std::iterator // 内部用32位来表示地址，是因为16位不好判断end()
			<std::random_access_iterator_tag, uint8_t, ptrdiff_t, uint32_t, uint8_t &> {
	public:
		iterator(): parent(NULL), addr(0) {}
		iterator(__CPUMem__ *parent, pointer addr): parent(parent), addr(addr) {}

		reference operator*() { return parent->operator[](addr); }
		uint8_t * get_raw_pointer() { return &parent->operator[](addr); }

		iterator& operator++() { ++addr; return *this; } // ++it
		iterator& operator--() { --addr; return *this; } // --it

		iterator& operator+=(const uint16_t value) { this->addr += value; return *this; }
		iterator& operator-=(const uint16_t value) { this->addr -= value; return *this; }
		friend iterator operator+(iterator lhs, const uint16_t& rhs) { lhs += rhs; return lhs; }
		friend iterator operator-(iterator lhs, const uint16_t& rhs) { lhs -= rhs; return lhs; }

		friend bool operator==(const iterator &lhs, const iterator &rhs) { return lhs.addr == rhs.addr; }
		friend bool operator!=(const iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }
		friend bool operator<(const iterator &lhs, const iterator &rhs) { return lhs.addr < rhs.addr; }

		friend difference_type operator-(const iterator &lhs, const iterator &rhs) { return lhs.addr - rhs.addr; }

		uint8_t &operator[](uint16_t value) { return parent->operator[](addr + value); }
		const uint8_t &operator[](uint16_t value) const { return parent->operator[](addr + value); }
	private:
		pointer addr;
		__CPUMem__ *parent;
	};

	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, 0x10000); }

	uint8_t &operator[](uint16_t addr) { return AT(addr); }
	const uint8_t &operator[](uint16_t addr) const { return AT(addr); }
};

struct Operation;

class CPU {
public:
	friend class Cartridge;
	friend struct Operation;

	CPU() : P(0x34), A(0), X(0), Y(0), SP(0xfd) { }; // Power Up
	// 读取一个字节
	inline uint8_t Read8(uint16_t addr) const { return mem[addr]; }
	// 读取一个字
	inline uint16_t Read16(uint16_t addr) const { return (mem[addr + 1] << 8) | mem[addr]; }
	// 写一个字节
	inline void Write(uint16_t addr, uint8_t value) { mem[addr] = value; }
	inline void Reset() { PC = Read16(static_cast<uint16_t>(InterruptVector::Reset)); }
	uint8_t Execute();
#ifndef NDEBUG
	ProcessorStatus &getP() { return P; }
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

	/**************** 指令声明区Begin ****************/
	OpExeFuncDecl(OP_ASL);
	OpExeFuncDecl(OP_CLC);
	OpExeFuncDecl(OP_CLD);
	OpExeFuncDecl(OP_CLI);
	OpExeFuncDecl(OP_CLV);
	OpExeFuncDecl(OP_DEX);
	OpExeFuncDecl(OP_DEY);
	OpExeFuncDecl(OP_INX);
	OpExeFuncDecl(OP_INY);
	OpExeFuncDecl(OP_LSR);
	OpExeFuncDecl(OP_NOP);
	OpExeFuncDecl(OP_ROL);
	OpExeFuncDecl(OP_ROR);
	OpExeFuncDecl(OP_SEC);
	OpExeFuncDecl(OP_SED);
	OpExeFuncDecl(OP_SEI);
	OpExeFuncDecl(OP_TAX);
	OpExeFuncDecl(OP_TAY);
	OpExeFuncDecl(OP_TSX);
	OpExeFuncDecl(OP_TXA);
	OpExeFuncDecl(OP_TXS);
	OpExeFuncDecl(OP_TYA);
	OpExeFuncDecl(OP_PHA);
	OpExeFuncDecl(OP_PHP);
	OpExeFuncDecl(OP_PLA);
	OpExeFuncDecl(OP_PLP);
	OpExeFuncDecl(OP_RTI);
	OpExeFuncDecl(OP_RTS);
	OpExeFuncDecl(OP_BRK);
	OpExeFuncDecl(OP_BCC);
	OpExeFuncDecl(OP_BCS);
	OpExeFuncDecl(OP_BEQ);
	OpExeFuncDecl(OP_BMI);
	OpExeFuncDecl(OP_BNE);
	OpExeFuncDecl(OP_BPL);
	OpExeFuncDecl(OP_BVC);
	OpExeFuncDecl(OP_BVS);
	OpExeFuncDecl(OP_ADC);
	OpExeFuncDecl(OP_AND);
	OpExeFuncDecl(OP_CMP);
	OpExeFuncDecl(OP_CPX);
	OpExeFuncDecl(OP_CPY);
	OpExeFuncDecl(OP_EOR);
	OpExeFuncDecl(OP_LDA);
	OpExeFuncDecl(OP_LDX);
	OpExeFuncDecl(OP_LDY);
	OpExeFuncDecl(OP_ORA);
	OpExeFuncDecl(OP_SBC);
	OpExeFuncDecl(OP_BIT);
	OpExeFuncDecl(OP_STA);
	OpExeFuncDecl(OP_STX);
	OpExeFuncDecl(OP_STY);
	OpExeFuncDecl(OP_DEC);
	OpExeFuncDecl(OP_INC);
	OpExeFuncDecl(OP_JMP);
	OpExeFuncDecl(OP_JSR);
/****************  指令声明区End  ****************/





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
};

struct Operation { // 指令
	using ExeFunc = uint8_t (*)(const Operation& self, ProcessorStatus &P, uint8_t *operand1, uint8_t *operand2);
	uint8_t code;
	CPU::OpAddressingMode addressing_mode;
	uint8_t bytes, cycles, extra_cycles;
	ExeFunc exe; // 返回具体执行的cycles数目
};


#endif //FCEMU_CPU_H
