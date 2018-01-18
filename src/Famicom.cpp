/****************************************************************************
 > File Name: Famicom.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-06 -- 18:24
 ****************************************************************************/

#include "Famicom.h"

void Famicom::Run() {
	uint8_t cycles; // based on cpu
	while(runing) {
		cpu.ShowStatus();
		cycles = cpu.Execute();
		ppu.Execute(cycles);

	}

}
