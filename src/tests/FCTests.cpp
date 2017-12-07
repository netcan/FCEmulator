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
	Cartridge cart;
	EXPECT_TRUE(cart.LoadRomFile(cpu, "./nestest.nes"));
	cpu.Reset();
	// 测试是否正常读取
	EXPECT_EQ(cpu.Read8(0xc000), 0x4c);
	EXPECT_EQ(cpu.Read16(0xc001), 0xc5f5);
	EXPECT_EQ(cpu.Read8(0xCBAF), 0xa0);
	EXPECT_EQ(cpu.Read8(0xF87C), 0x60);
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

}