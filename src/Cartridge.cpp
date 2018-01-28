/****************************************************************************
 > File Name: Cartridge.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-05 -- 20:33
 ****************************************************************************/

#include "Cartridge.h"

Cartridge::Cartridge() {
	static_assert(sizeof(header) == 16, "size of header should be 16 bytes");
}

bool Cartridge::LoadRomFile(CPU &cpu, PPU &ppu, const std::string &filename) {
	this->filename = filename;
	FILE *fp = fopen(filename.c_str(), "rb");
	if(fp == NULL) {
		printf("open file ");
		perror(filename.c_str());
		return false;
	}
	fread((void*)&header, sizeof(header), 1, fp); // 读取头

	__CPUMem__::iterator cpuRamIt = cpu.mem.begin();
	__PPUMem__::iterator ppuRamIt = ppu.mem.begin();
	// PPU mirroring
	if(! GetBit(header.ROMControl[0], 3)) { // Horizontal or Vectical Mirroring
		GetBit(header.ROMControl[0], 0) ?
		ppu.mem.setVerticalMirroring() :
		ppu.mem.setHorizontalMirroring();
	} else { // Four-screen mirroring
		// TODO: Four-screen mirroring

	}

	// Mapper
	switch (JointBits(GetUpperBits(header.ROMControl[1]),
	                  GetUpperBits(header.ROMControl[0]))) {
		case 0: { // Mapper 0
			if(GetBit(header.ROMControl[0], 2)) { // Trainer
				uint8_t trainer[0x200];
				fread((void*)trainer, sizeof(trainer), 1, fp);
				std::copy(trainer, trainer + sizeof(trainer), cpuRamIt + 7000);
			}
			uint8_t PRGRomData[0x4000];
			fread((void*)PRGRomData, sizeof(PRGRomData), 1, fp); // Lower PRG ROM Bank to cpu
			std::copy(PRGRomData, PRGRomData + sizeof(PRGRomData), cpuRamIt + 0x8000);
			if(header.PRGRomBankCnt > 1)
				fread((void*)PRGRomData, sizeof(PRGRomData), 1, fp); // Upper PRG ROM Bank to cpu
			std::copy(PRGRomData, PRGRomData + sizeof(PRGRomData), cpuRamIt + 0xc000);

			uint8_t CHRRomData[0x2000];
			fread((void*)CHRRomData, sizeof(CHRRomData), 1, fp); // CHR Rom to ppu
			std::copy(CHRRomData, CHRRomData + sizeof(CHRRomData), ppuRamIt + 0x0000);

			break;
		}
	}

	fclose(fp);
	return true;
}

bool Cartridge::PrintHeader() const {
	if(header.INes[0] == 0x4e && header.INes[1] == 0x45
	&& header.INes[2] == 0x53 && header.INes[3] == 0x1a) {
		puts("\n---------------------------------------");
		printf("File name: %s\n"
				       "PRG-ROM size: %dx16k\n"
				       "CHR-ROM size: %dx8k\n"
				       "Mapper number: %d\n"
				       "PRG-RAM size: %dx8k\n",
		       filename.c_str(), header.PRGRomBankCnt,
		       header.CHRRomBankCnt, JointBits(GetUpperBits(header.ROMControl[1]),
		                                       GetUpperBits(header.ROMControl[0])),
		       header.PRGRamBankCnt);
		return true;
	} else return false;
}

