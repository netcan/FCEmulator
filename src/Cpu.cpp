/****************************************************************************
 > File Name: cpu.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-03 -- 20:31
 ****************************************************************************/

#include "Cpu.h"

ProcessorStatus::ProcessorStatus(uint8_t value): Negative(this->P), Overflow(this->P),
                                    BrkExecuted(this->P), Decimal(this->P),
                                    IrqDisabled(this->P), Zero(this->P),
                                    Carry(this->P) {
	P = value; // 初值
}

