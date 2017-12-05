/****************************************************************************
 > File Name: FCTests.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-04 -- 21:25
 ****************************************************************************/

#include "gtest/gtest.h"
#include "../cpu.h"
#include <iostream>
#include <vector>

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

