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
#include "Joypad.h"

class Famicom {
public:
	Famicom(const std::string & RomFileName);
	void Run();

private:
	CPU cpu;
	PPU ppu;
	Joypad pad;
	Cartridge cart;
	bool runing;

};

