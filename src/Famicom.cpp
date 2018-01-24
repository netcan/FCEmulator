/****************************************************************************
 > File Name: Famicom.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-06 -- 18:24
 ****************************************************************************/

#include "Famicom.h"

void Famicom::Run() {
	uint8_t cpu_cycles; // based on cpu
	cart.PrintHeader();
	while(runing && ppu.event.type != SDL_QUIT) {
//		cpu.ShowStatus();
		cpu_cycles = cpu.Execute();
		ppu.Execute(cpu_cycles);
		SDL_PollEvent(&ppu.event);

	}

}
