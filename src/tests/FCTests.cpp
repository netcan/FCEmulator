/****************************************************************************
 > File Name: FCTests.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-04 -- 21:25
 ****************************************************************************/

#include "gtest/gtest.h"
#include "../Base.h"
#include "../Cartridge.h"
#include "../Cpu.h"
#include "../Famicom.h"

TEST(ProcessorStatusCheck, test_value) {
// 测试初值
	ProcessorStatus P;
	EXPECT_EQ(P, 0x34);
	for(int value = 0x00; value <= 0xff; ++value) {
		P = value;
		EXPECT_EQ(P, value);
	}
}

TEST(ProcessorStatusCheck, test_field) {
// 测试Flag位
	ProcessorStatus P = 0xff;
	EXPECT_TRUE(P.Carry && P.Zero && P.IrqDisabled
	            && P.Decimal && P.BrkExecuted &&
	            P.Overflow && P.Negative);

	P = 0;
	EXPECT_EQ(P, 0);
	EXPECT_FALSE(P.Carry || P.Zero || P.IrqDisabled
	             || P.Decimal || P.BrkExecuted ||
	             P.Overflow || P.Negative);


	P.Negative = 0;
	EXPECT_FALSE(P.Negative);
	EXPECT_EQ(P, 0);
	P.Negative = 1;
	EXPECT_TRUE(P.Negative);
	EXPECT_EQ(P, 0x80);

	P.Overflow = 0;
	EXPECT_FALSE(P.Overflow);
	EXPECT_EQ(P, 0x80);
	P.Overflow = 1;
	EXPECT_TRUE(P.Overflow);
	EXPECT_EQ(P, 0xc0);

	P.IrqDisabled = 0;
	EXPECT_FALSE(P.IrqDisabled);
	EXPECT_EQ(P, 0xc0);
	P.IrqDisabled = 1;
	EXPECT_TRUE(P.IrqDisabled);
	EXPECT_EQ(P, 0xc4);

	P = 0xb1;
	P.Negative = 0;
	P.Carry = 0;
	P.Zero = 1;
	EXPECT_EQ(P, 0x32);
}

TEST(BaseTest, test_bit_opt) {
	// 测试获取高低位
	uint8_t value = 0xff;
	for (size_t i = 0; i < 8; ++i)
		EXPECT_TRUE(GetBit(value, i));
	EXPECT_EQ(GetUpperBits(value), 0x0f);
	EXPECT_EQ(GetLowerBits(value), 0x0f);

	value = 0;
	for (size_t i = 0; i < 8; ++i)
		EXPECT_FALSE(GetBit(value, i));
	EXPECT_EQ(GetUpperBits(value), 0x00);
	EXPECT_EQ(GetLowerBits(value), 0x00);

	// 测试拼接高低位
	EXPECT_EQ(JointBits(0x3, 0xf), 0x3f);
}

TEST(CartridgeTest, test_loading_romfile) {
	CPU cpu;
	PPU ppu;
	Cartridge cart;
	EXPECT_TRUE(cart.LoadRomFile(cpu, ppu, "./nestest.nes"));
	cpu.Reset();
	// 测试是否正常读取
	EXPECT_EQ(cpu.Read8(0xc000), 0x4c);
	EXPECT_EQ(cpu.Read16(0xc001), 0xc5f5);
	EXPECT_EQ(cpu.Read8(0xCBAF), 0xa0);
	EXPECT_EQ(cpu.Read8(0xF87C), 0x60);

	EXPECT_EQ(ppu.Read8(0x0020), 0x80);
	EXPECT_EQ(ppu.Read8(0x0022), 0xff);
	EXPECT_EQ(ppu.Read8(0x07bf), 0x3c);
	EXPECT_TRUE(cart.PrintHeader());
}

TEST(CPUTest, test_ram) {
	__CPUMem__ mem;
	// 测试Ram区
	for(uint16_t addr = 0; addr < 0x2000; ++addr) {
		mem[addr] = 0x23;
		uint16_t testaddr = addr;
		do {
			EXPECT_EQ(mem[testaddr], 0x23);
			testaddr = (testaddr + 0x800) % 0x2000;
		} while(testaddr != addr);
	}
	// 测试PPU Register区
	for(uint16_t addr = 0x2000; addr < 0x4000; ++addr) {
		mem[addr] = 0x34;
		uint16_t testaddr = addr;
		do {
			EXPECT_EQ(mem[testaddr], 0x34);
			testaddr = (testaddr + 0x8);
			if(testaddr >= 0x4000) testaddr -= 0x2000;
		} while(testaddr != addr);
	}
	// 测试剩余区域
	for(uint16_t addr = 0x4000; addr < 0xffff; ++addr) {
		mem[addr] = 0x15;
		EXPECT_EQ(mem[addr], 0x15);
	}

	mem[0xffff] = 0x50;
	EXPECT_EQ(mem[0xffff], 0x50);
}

TEST(CPUTest, test_ram_iterator) {
	__CPUMem__ mem;
	__CPUMem__::iterator it;
	// 迭代器全写测试
	for(it = mem.begin(); it != mem.end(); ++it) {
		*it = 0x23;
		EXPECT_EQ(*it, 0x23);
	}
	for(uint16_t addr = 0x0000; addr < 0xffff; ++addr)
		EXPECT_EQ(mem[addr], 0x23);
	EXPECT_EQ(mem[0xffff], 0x23);

	std::fill(mem.begin(), mem.end(), 0x66);
	std::for_each(mem.begin(), mem.end(), [](const auto &value) { EXPECT_EQ(value, 0x66); });


	// 迭代器使用测试
	it = mem.begin();
	it[0x0001] = 0x33;
	EXPECT_EQ(it[0x0801], 0x33);
	*(it + 0x0002) = 0xaa;
	EXPECT_EQ(it[0x0002], 0xaa);

	// 按块写测试
	uint8_t arr[0x4000];
	std::generate(arr, arr + sizeof(arr), []() { return rand() & 0xff; });
	std::copy(arr, arr + sizeof(arr), it + 0x8000);
	for(uint16_t addr = 0x8000; addr < 0xc000; ++addr)
		EXPECT_EQ(arr[addr - 0x8000], mem[addr]);

	// 指针测试
	uint8_t *p = (it + 0x0001).get_raw_pointer();
	*p = 0x34;
	EXPECT_EQ(it[0x0801], 0x34);
}



TEST(CPUTest, opTest) {
	CPU cpu;
	PPU ppu;
	Cartridge cart;
	uint8_t &X = cpu.getX(), &Y = cpu.getY(),
			&SP = cpu.getSP(), &A = cpu.getA();
	uint16_t &PC = cpu.getPC();
	ProcessorStatus &P = cpu.getP();

	EXPECT_TRUE(cart.LoadRomFile(cpu, ppu, "./test.nes"));
	EXPECT_TRUE(cart.PrintHeader());
	cpu.Reset();

	// 测试是否正常读取指令
	EXPECT_EQ(cpu.Read8(PC), 0xf8); // SED

	// 开始执行
	EXPECT_EQ(cpu.Execute(), 2);    // sed
	EXPECT_TRUE(P.Decimal);

	EXPECT_EQ(cpu.Execute(), 2);    // cld
	EXPECT_FALSE(P.Decimal);

	A = 0xff;
	EXPECT_EQ(cpu.Execute(), 2);    // asl
	EXPECT_EQ(A, 0xfe);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);
	EXPECT_TRUE(P.Carry);

	cpu.Write(0x0066, 0);
	EXPECT_EQ(cpu.Execute(), 5);    // asl $66
	EXPECT_EQ(cpu.Read8(0x66), 0x0);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Carry);

	X = 1;
	cpu.Write(0x0000, 0x80);
	EXPECT_EQ(cpu.Execute(), 6);    // asl $ff,X
	EXPECT_EQ(cpu.Read8(0x66), 0x0);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Carry);

	cpu.Write(0x6666, 0x66);
	EXPECT_EQ(cpu.Execute(), 6);    // asl $6666
	EXPECT_EQ(cpu.Read8(0x6666), 0xcc);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Carry);

	X = 0xff;
	cpu.Write(0x6765, 0x66);
	EXPECT_EQ(cpu.Execute(), 7);    // asl $6666,X
	EXPECT_EQ(cpu.Read8(0x6765), 0xcc);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Carry);

	P.Carry = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // clc
	EXPECT_FALSE(P.Carry);

	P.IrqDisabled = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // cli
	EXPECT_FALSE(P.IrqDisabled);

	P.Overflow = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // cli
	EXPECT_FALSE(P.Overflow);

	X = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // dex
	EXPECT_EQ(X, 0);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Zero);

	EXPECT_EQ(cpu.Execute(), 2);    // dex
	EXPECT_EQ(X, 0xff);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	X = 0xfe;
	EXPECT_EQ(cpu.Execute(), 2);    // inx
	EXPECT_EQ(X, 0xff);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	EXPECT_EQ(cpu.Execute(), 2);    // inx
	EXPECT_EQ(X, 0);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Zero);

	Y = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // dey
	EXPECT_EQ(Y, 0);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Zero);

	EXPECT_EQ(cpu.Execute(), 2);    // dey
	EXPECT_EQ(Y, 0xff);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	Y = 0xfe;
	EXPECT_EQ(cpu.Execute(), 2);    // iny
	EXPECT_EQ(Y, 0xff);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	EXPECT_EQ(cpu.Execute(), 2);    // iny
	EXPECT_EQ(Y, 0);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Zero);

	A = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // lsr
	EXPECT_EQ(A, 0);
	EXPECT_TRUE(P.Carry);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);

	cpu.Write(0x6666, 0xfe);
	EXPECT_EQ(cpu.Execute(), 6);    // lsr, $6666
	EXPECT_EQ(cpu.Read8(0x6666), 0x7f);
	EXPECT_FALSE(P.Carry);
	EXPECT_FALSE(P.Zero);
	EXPECT_FALSE(P.Negative);

	EXPECT_EQ(cpu.Execute(), 2);    // nop

	P.Carry = 1;
	A = 0x80;
	EXPECT_EQ(cpu.Execute(), 2);    // rol
	EXPECT_EQ(A, 1);
	EXPECT_TRUE(P.Carry);
	EXPECT_FALSE(P.Zero);
	EXPECT_FALSE(P.Negative);

	P.Carry = 1;
	cpu.Write(0x66, 0x7e);
	EXPECT_EQ(cpu.Execute(), 5);    // rol, $66
	EXPECT_EQ(cpu.Read8(0x66), 0xfd);
	EXPECT_FALSE(P.Carry);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	P.Carry = 0;
	cpu.Write(0x66, 0x0);
	EXPECT_EQ(cpu.Execute(), 5);    // rol, $66
	EXPECT_EQ(cpu.Read8(0x66), 0x00);
	EXPECT_FALSE(P.Carry);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);

	P.Carry = 0;
	A = 1;
	EXPECT_EQ(cpu.Execute(), 2);    // ror
	EXPECT_EQ(A, 0);
	EXPECT_TRUE(P.Carry);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);

	P.Carry = 1;
	X = 6;
	cpu.Write(0x66, 0x7a);
	EXPECT_EQ(cpu.Execute(), 6);    // ror $60, X
	EXPECT_EQ(cpu.Read8(0x66), 0xbd);
	EXPECT_FALSE(P.Carry);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	EXPECT_EQ(cpu.Execute(), 2);    // sec
	EXPECT_TRUE(P.Carry);

	P.IrqDisabled = false;
	EXPECT_EQ(cpu.Execute(), 2);    // sei
	EXPECT_TRUE(P.IrqDisabled);

	A = 0;
	EXPECT_EQ(cpu.Execute(), 2);    // tax
	EXPECT_EQ(X, A);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);

	A = 0xf0;
	EXPECT_EQ(cpu.Execute(), 2);    // tay
	EXPECT_EQ(Y, A);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	SP = 0x23;
	EXPECT_EQ(cpu.Execute(), 2);    // tsx
	EXPECT_EQ(X, SP);
	EXPECT_FALSE(P.Zero);
	EXPECT_FALSE(P.Negative);

	X = 0x66;
	EXPECT_EQ(cpu.Execute(), 2);    // txa
	EXPECT_EQ(A, X);
	EXPECT_FALSE(P.Zero);
	EXPECT_FALSE(P.Negative);

	X = 0xab;
	EXPECT_EQ(cpu.Execute(), 2);    // txs
	EXPECT_EQ(SP, X);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	Y = 0x7c;
	EXPECT_EQ(cpu.Execute(), 2);    // tya
	EXPECT_EQ(A, Y);
	EXPECT_FALSE(P.Zero);
	EXPECT_FALSE(P.Negative);

	A = 0xdd;
	SP = 0x00;
	EXPECT_EQ(cpu.Execute(), 3);    // pha
	EXPECT_EQ(cpu.Read8(0x100), A);
	EXPECT_EQ(SP, 0xff);

	P = 0xcc;
	EXPECT_EQ(cpu.Execute(), 3);    // php
	EXPECT_EQ(SP, 0xfe);

	EXPECT_EQ(cpu.Execute(), 4);    // pla
	EXPECT_EQ(A, 0xdc);
	EXPECT_EQ(SP, 0xff);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	EXPECT_EQ(cpu.Execute(), 4);    // plp
	EXPECT_EQ(SP, 0x00);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	uint16_t tmpPC = PC;

	EXPECT_EQ(cpu.Execute(), 7);    // brk
	EXPECT_TRUE(P.IrqDisabled);
	EXPECT_TRUE(P.BrkExecuted);

	EXPECT_EQ(cpu.Execute(), 6);    // rti
	EXPECT_EQ(tmpPC + 1, PC);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Carry = false;
	EXPECT_EQ(cpu.Execute(), 3);    // bcc *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Carry = true;
	EXPECT_EQ(cpu.Execute(), 2);    // bcc *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	EXPECT_EQ(cpu.Execute(), 3);    // bcs *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Carry = false;
	EXPECT_EQ(cpu.Execute(), 2);    // bcs *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Zero = true;
	EXPECT_EQ(cpu.Execute(), 3);    // beq *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Zero = false;
	EXPECT_EQ(cpu.Execute(), 2);    // beq *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Negative = true;
	EXPECT_EQ(cpu.Execute(), 3);    // bmi *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Negative = false;
	EXPECT_EQ(cpu.Execute(), 2);    // bmi *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Zero = false;
	EXPECT_EQ(cpu.Execute(), 3);    // bne *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Zero = true;
	EXPECT_EQ(cpu.Execute(), 2);    // bne *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Negative = false;
	EXPECT_EQ(cpu.Execute(), 3);    // bpl *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Negative = true;
	EXPECT_EQ(cpu.Execute(), 2);    // bpl *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Overflow = false;
	EXPECT_EQ(cpu.Execute(), 3);    // bvc *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Overflow = true;
	EXPECT_EQ(cpu.Execute(), 2);    // bvc *-1
	EXPECT_EQ(PC, tmpPC + 3);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	EXPECT_EQ(cpu.Execute(), 3);    // bvs *-1
	EXPECT_EQ(PC, tmpPC);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	P.Overflow = false;
	EXPECT_EQ(cpu.Execute(), 2);    // bvs *-1
	EXPECT_EQ(PC, tmpPC + 3);

	A = 1; // 0x01 + 0xff + 00 = 0x100
	P.Carry = 0;
	EXPECT_EQ(cpu.Execute(), 2);    // adc #$ff
	EXPECT_EQ(A, 0x00);
	EXPECT_TRUE(P.Carry);
	EXPECT_FALSE(P.Overflow);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Zero);

	A = 0x7f, Y = 1; // 0x7f + 0x01 + 0x01 = 0x81
	P.Carry = 1;
	cpu.Write(0x6700, 0x01);
	EXPECT_EQ(cpu.Execute(), 5);    // adc #66ff,Y
	EXPECT_EQ(A, 0x81);
	EXPECT_FALSE(P.Carry);
	EXPECT_TRUE(P.Overflow);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0xff, X = 1;  // 0xff + 0x80 + 0x00 = 0x17f
	P.Carry = 0;
	cpu.Write(0x67, 0x56);
	cpu.Write(0x68, 0x78);
	cpu.Write(0x7856, 0x80);
	EXPECT_EQ(cpu.Execute(), 6);    // adc ($66,X)
	EXPECT_EQ(A, 0x7f);
	EXPECT_TRUE(P.Carry);
	EXPECT_TRUE(P.Overflow);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0x01, Y = 1; // 0x01 + 0x01 + 0x00 = 0x02
	P.Carry = 0;
	cpu.Write(0x66, 0xff);
	cpu.Write(0x67, 0x66);
	cpu.Write(0x6700, 0x01);
	EXPECT_EQ(cpu.Execute(), 6);    // adc ($66),Y
	EXPECT_EQ(A, 0x02);
	EXPECT_FALSE(P.Carry);
	EXPECT_FALSE(P.Overflow);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0xa5, Y = 0x66;  // 0xa5 & 0x97 = 0x85
	cpu.Write(0x6666, 0x97);
	EXPECT_EQ(cpu.Execute(), 4);    // and $6600,Y
	EXPECT_EQ(A, 0x85);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0xff;
	EXPECT_EQ(cpu.Execute(), 2);    // cmp #$ff
	EXPECT_TRUE(P.Carry);
	EXPECT_FALSE(P.Negative);
	EXPECT_TRUE(P.Zero);

	X = 0x00;
	EXPECT_EQ(cpu.Execute(), 2);    // cpx #$ff
	EXPECT_FALSE(P.Carry);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Zero);

	Y = 0xff;
	EXPECT_EQ(cpu.Execute(), 2);    // cpy #$00
	EXPECT_TRUE(P.Carry);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0xff;  // 0xff ^ 0x00 = 0xff
	EXPECT_EQ(cpu.Execute(), 2);    // eor #$00
	EXPECT_EQ(A, 0xff);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	EXPECT_EQ(cpu.Execute(), 2);    // lda #$23
	EXPECT_EQ(A, 0x23);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Zero);

	Y = 6;
	cpu.Write(0x66, 0x66);
	EXPECT_EQ(cpu.Execute(), 4);    // ldx $60,Y
	EXPECT_EQ(X, 0x66);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Zero);

	EXPECT_EQ(cpu.Execute(), 2);    // ldy #$aa
	EXPECT_EQ(Y, 0xaa);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0x4f; // 0x4f | 0xab = 0xef
	EXPECT_EQ(cpu.Execute(), 2);    // ora #$0xab
	EXPECT_EQ(A, 0xef);
	EXPECT_TRUE(P.Negative);
	EXPECT_FALSE(P.Zero);

	P.Carry = 1;
	A = 0x01; // 0x01 - 0xff = 0x2
	EXPECT_EQ(cpu.Execute(), 2);    // sbc #$ff
	EXPECT_EQ(A, 0x02);
	EXPECT_FALSE(P.Carry);
	EXPECT_FALSE(P.Overflow);
	EXPECT_FALSE(P.Negative);
	EXPECT_FALSE(P.Zero);

	A = 0x3f;  // 0x3f & 0xc0 = 0x00
	cpu.Write(0x66, 0xc0);
	EXPECT_EQ(cpu.Execute(), 3);    // bit $66
	EXPECT_EQ(A, 0x3f);
	EXPECT_TRUE(P.Overflow);
	EXPECT_TRUE(P.Negative);
	EXPECT_TRUE(P.Zero);


	A = 0x6f;
	EXPECT_EQ(cpu.Execute(), 3);    // sta $23
	EXPECT_EQ(cpu.Read8(0x23), A);

	X = 0xee;
	EXPECT_EQ(cpu.Execute(), 3);    // stx $23
	EXPECT_EQ(cpu.Read8(0x23), X);

	Y = 0xaa;
	EXPECT_EQ(cpu.Execute(), 3);    // sty $23
	EXPECT_EQ(cpu.Read8(0x23), Y);

	cpu.Write(0x23, 0x00);
	EXPECT_EQ(cpu.Execute(), 5);    // dec $23
	EXPECT_EQ(cpu.Read8(0x23), 0xff);
	EXPECT_FALSE(P.Zero);
	EXPECT_TRUE(P.Negative);

	EXPECT_EQ(cpu.Execute(), 5);    // inc $23
	EXPECT_EQ(cpu.Read8(0x23), 0x00);
	EXPECT_TRUE(P.Zero);
	EXPECT_FALSE(P.Negative);

	tmpPC = PC;
	EXPECT_EQ(cpu.Execute(), 6);    // jsr jsr_test
	EXPECT_EQ(PC, tmpPC + 6);
	EXPECT_EQ(cpu.Execute(), 2);    // nop
	EXPECT_EQ(cpu.Execute(), 6);    // rts
	EXPECT_EQ(PC, tmpPC + 3);
	EXPECT_EQ(cpu.Execute(), 3);    // jmp after_test
	EXPECT_EQ(PC, tmpPC + 8);
	EXPECT_EQ(cpu.Execute(), 2);    // nop

	tmpPC = PC;
	cpu.Write(0x66ff, 0xab);
	cpu.Write(0x6600, 0xcd); // cdab
	EXPECT_EQ(cpu.Execute(), 5);    // jmp ($ff66)
	EXPECT_EQ(PC, 0xcdab);

	PC = tmpPC + 3;

}

TEST(CPUTest, mappingTest) { // 测试IO寄存器的映射状态
	PPU ppu;
	CPU cpu(ppu);
	for(auto addr = 0x2000; addr < 0x2008; ++addr)
		cpu.Write(addr, 0x3c);
	cpu.Write(0x4014, 0x3c);
	EXPECT_EQ(ppu.getPPUCTRL(), 0x3c);
	EXPECT_EQ(ppu.getPPUMASK(), 0x3c);
	EXPECT_EQ(ppu.getPPUSTATUS(), 0x3c);
	EXPECT_EQ(ppu.getOAMADDR(), 0x3c);
	EXPECT_EQ(ppu.getOAMDATA(), 0x3c);
	EXPECT_EQ(ppu.getOAMDMA(), 0x3c);
	EXPECT_EQ(ppu.getPPUSCROLL(), 0x3c);
	EXPECT_EQ(ppu.getPPUADDR(), 0x3c);
	EXPECT_EQ(ppu.getPPUDATA(), 0x3c);

}

TEST(CPUTest, romTest) {
	PPU ppu;
	CPU cpu(ppu);

	Cartridge cart;
	uint8_t &X = cpu.getX(), &Y = cpu.getY(),
			&SP = cpu.getSP(), &A = cpu.getA();
	uint16_t &PC = cpu.getPC();
	ProcessorStatus &P = cpu.getP();

	int targetX, targetY, targetSP, targetA, targetP, targetOpcode, targetCYC, targetPC;

	EXPECT_TRUE(cart.LoadRomFile(cpu, ppu, "./nestest.nes"));
	EXPECT_TRUE(cart.PrintHeader());
	cpu.Reset();

	PC = 0xc000;
	P = 0x24;
	FILE* log = fopen("nestest.log", "r");
	char row[0xff], msg[0xff], opCodeName[5];
	while(fgets(row, sizeof(row), log)) {
		sscanf(row, "%X%X", &targetPC, &targetOpcode);
		sscanf(row + 11, "%s", opCodeName);
		sscanf(row + strlen(row) - 35, "A:%X X:%X Y:%X P:%X SP:%X CYC:%d",
		       &targetA, &targetX, &targetY, &targetP, &targetSP, &targetCYC
		);
		int CYC = cpu.getCycles() * 3 % 341;
		sprintf(msg, "%s(%02X): %X(%X) %02X(%02X) A:%02X(%02X) X:%02X(%02X) Y:%02X(%02X) P:%02X(%02X) SP:%02X(%02X) CYC:%3d(%3d)\n",
		        opCodeName, targetOpcode, PC, targetPC, cpu.Read8(PC), targetOpcode,
		        A, targetA, X, targetX, Y, targetY, (uint8_t)P, targetP,
		        SP, targetSP, CYC, targetCYC
		);
		ASSERT_EQ(PC, targetPC) << msg;
		ASSERT_EQ(P, targetP) << msg;
		ASSERT_EQ(A, targetA) << msg;
		ASSERT_EQ(X, targetX) << msg;
		ASSERT_EQ(Y, targetY) << msg;
		ASSERT_EQ(SP, targetSP) << msg;
		ASSERT_EQ(PC, targetPC) << msg;
		ASSERT_EQ(CYC, targetCYC) << msg;
		cpu.Execute();
	}

}


TEST(PPUTest, test_ram) {
	__PPUMem__ mem;
	// 测试NameTable
	for (int addr = 0; addr < 0x2000; ++addr) {
		mem[addr] = addr & 0xff;
		EXPECT_EQ(mem[addr], addr & 0xff);
	}
	// 测试NameTable
	// 测试Horizontal Mirroring
	mem.setHorizontalMirroring();
	for (int i = 0; i < 0x400; ++i) {
		mem[0x2000 + i] = i & 0xff;
		EXPECT_EQ(mem[0x2000 + i], mem[0x2400 + i]);
		mem[0x2800 + i] = (i+1) & 0xff;
		EXPECT_EQ(mem[0x2800 + i], mem[0x2C00 + i]);
	}
	// 测试Vertical Mirroring
	mem.setVerticalMirroring();
	for (int i = 0; i < 0x400; ++i) {
		mem[0x2000 + i] = i & 0xff;
		EXPECT_EQ(mem[0x2000 + i], mem[0x2800 + i]);
		mem[0x2800 + i] = (i+1) & 0xff;
		EXPECT_EQ(mem[0x2400 + i], mem[0x2C00 + i]);
	}

	for (int i = 0; i < 0xf00; ++i)
		EXPECT_EQ(mem[0x2000 + i], mem[0x3000 + i]);

	// 测试Palette
	for (int addr = 0x3f00; addr < 0x4000; ++addr) {
		mem[addr] = addr & 0xff;
		EXPECT_EQ(mem[((addr + 0x20) & 0xff) | 0x3f00], addr & 0xff);
	}
	mem[0x3f00] = 0xdf;
	EXPECT_EQ(mem[0x3f04], 0xdf);
	EXPECT_EQ(mem[0x3f08], 0xdf);
	EXPECT_EQ(mem[0x3f0C], 0xdf);
	EXPECT_EQ(mem[0x3f10], 0xdf);
	EXPECT_EQ(mem[0x3f14], 0xdf);
	EXPECT_EQ(mem[0x3f18], 0xdf);
	EXPECT_EQ(mem[0x3f1C], 0xdf);

	// 剩余区域
	for (int addr = 0x4000; addr < 0x10000; ++addr)
		EXPECT_EQ(mem[addr], mem[addr & 0x3fff]);

}

TEST(PPUTest, test_ram_iterator) {
	__PPUMem__ mem;
	__PPUMem__::iterator it;
	// 迭代器全写测试
	for(it = mem.begin(); it != mem.end(); ++it) {
		*it = 0x23;
		EXPECT_EQ(*it, 0x23);
	}
	for(uint16_t addr = 0x0000; addr < 0xffff; ++addr)
		EXPECT_EQ(mem[addr], 0x23);
	EXPECT_EQ(mem[0xffff], 0x23);

	std::fill(mem.begin(), mem.end(), 0x66);
	std::for_each(mem.begin(), mem.end(), [](const auto &value) { EXPECT_EQ(value, 0x66); });


	// 迭代器使用测试
	it = mem.begin();
	it[0x0001] = 0x33;
	EXPECT_EQ(it[0x0001], 0x33);
	*(it + 0x0002) = 0xaa;
	EXPECT_EQ(it[0x0002], 0xaa);

	// 指针测试
	uint8_t *p = (it + 0x0001).get_raw_pointer();
	*p = 0x34;
	EXPECT_EQ(it[0x0001], 0x34);
}

TEST(PPUTest, test_palette) {
	PPU ppu;
	ppu.showPalette();
}

TEST(PPUTest, test_pattern_table) {
	PPU ppu;
	CPU cpu(ppu);
	Cartridge cart;
//	EXPECT_TRUE(cart.LoadRomFile(cpu, ppu, "./Super Mario Bros (E).nes"));
	EXPECT_TRUE(cart.LoadRomFile(cpu, ppu, "./nestest.nes"));
	cart.PrintHeader();
	ppu.showPatternTable();
}

