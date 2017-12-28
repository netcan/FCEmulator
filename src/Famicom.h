/****************************************************************************
 > File Name: Famicom.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-06 -- 18:24
 ****************************************************************************/

#ifndef FCEMU_FAMICOM_H
#define FCEMU_FAMICOM_H
#include "Base.h"
#include "Cpu.h"
#include "Ppu.h"
#include "Cartridge.h"

class Famicom {
public:
	Famicom(const std::string & RomFileName) {
		cart.LoadRomFile(cpu, RomFileName);
		cpu.Reset();
	}

private:
	PPU ppu;
	CPU cpu;
	Cartridge cart;

};


#endif //FCEMU_FAMICOM_H
