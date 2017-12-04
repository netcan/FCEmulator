/****************************************************************************
 > File Name: cpu.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/

#include "cpu.h"

ProcessorStatus::ProcessorStatus(uint8_t value): Negative(*this), Overflow(*this),
                                    BrkExecuted(*this), Decimal(*this),
                                    IrqDisabled(*this), Zero(*this),
                                    Carry(*this) {
	P = value; // 初值
}
