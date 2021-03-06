/****************************************************************************
 > File Name: Famicom.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-06 -- 18:24
 ****************************************************************************/

#include "Famicom.h"
#include <chrono>

Famicom::Famicom(const std::string &RomFileName) {
	cpu.connectTo(ppu);
	cpu.connectTo(pad);
	ppu.connectTo(cpu);
	runing = cart.LoadRomFile(cpu, ppu, RomFileName);
	cpu.Reset();
}

void Famicom::Run() {
	using namespace std::chrono;
	high_resolution_clock::time_point t = high_resolution_clock::now();
	uint16_t cpu_cycles; // based on cpu
	cart.PrintHeader();
	while(runing && ppu.event.type != SDL_QUIT) {
		// cpu.ShowStatus();
		cpu_cycles = cpu.Execute();
		ppu.Execute(cpu_cycles);

	}
	int rt = duration_cast<seconds>(high_resolution_clock::now() - t).count();
	printf("%.2lf fps(%d frame in %d s)\n", ppu.frames_count * 1.0 / rt, ppu.frames_count, rt);

}
