/****************************************************************************
 > File Name: Famicom.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-06 -- 18:24
 ****************************************************************************/
#pragma once
#include "Cpu.h"
#include "Ppu.h"
#include "Cartridge.h"

class Famicom {
public:
	Famicom(const std::string & RomFileName) {
		cpu.connectTo(ppu);
		ppu.connectTo(cpu);
		runing = cart.LoadRomFile(cpu, ppu, RomFileName);
		cpu.Reset();
	}
	void Run();

private:
	CPU cpu;
	PPU ppu;
	Cartridge cart;
	bool runing;

};

