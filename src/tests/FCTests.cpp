/****************************************************************************
 > File Name: FCTests.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-04 -- 21:25
 ****************************************************************************/

#include "gtest/gtest.h"
#include "../Base.h"

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

TEST(CartridgeTest, test_loading) {
	Cartridge cart;
	EXPECT_TRUE(cart.LoadFile("./nestest.nes"));
	EXPECT_TRUE(cart.PrintHeader());


}