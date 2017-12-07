/****************************************************************************
 > File Name: Cartridge.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-05 -- 20:33
 ****************************************************************************/

#ifndef FCEMU_CARTRIDGE_H
#define FCEMU_CARTRIDGE_H
#include "Base.h"
#include "Cpu.h"

class Cartridge {
public:
	Cartridge();
	bool LoadRomFile(CPU &cpu, const std::string & filename);
	bool PrintHeader() const;

private:
	struct {
		uint8_t INes[4];        // 应为0x4e 0x45 0x53 0x1a
		uint8_t PRGRomBankCnt;  // PRG ROM的大小（块数），单位16K一块
		uint8_t CHRRomBankCnt;  // CHR ROM的大小（块数），单位8K一块，0表示使用CHR RAM
		uint8_t ROMControl[2];  // ROM控制位
		uint8_t PRGRamBankCnt;  // PRG RAM的大小（块数）,单位8K一块，若为0则代表1块，8k
		uint8_t Reserved[7];    // 保留位，全零
	} header;
	std::string filename;

};


#endif //FCEMU_CARTRIDGE_H
