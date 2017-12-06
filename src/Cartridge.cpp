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

bool Cartridge::LoadFile(const std::string &filename) {
	this->filename = filename;
	FILE *fp = fopen(filename.c_str(), "r");
	if(fp == NULL) {
		printf("open file ");
		perror(filename.c_str());
		return false;
	}
	fread((void*)&header, sizeof(header), 1, fp);
	fclose(fp);
	return true;
}

bool Cartridge::PrintHeader() const {
	if(header.INes[0] == 0x4e && header.INes[1] == 0x45
	&& header.INes[2] == 0x53 && header.INes[3] == 0x1a) {
		printf("File name: %s\n"
				       "PRG-ROM size: %dx16k\n"
				       "CHR-ROM size: %dx8k\n"
				       "Mapper number: %d\n"
				       "PRG-RAM size: %dx8k\n",
		       filename.c_str(), header.PRGRomBankCnt,
		       header.CHRRomBankCnt, JointBits(GetUpperBits(header.ROMControl[1]),
		                                       GetLowerBits(header.ROMControl[0])),
		       header.PRGRamBankCnt);
		return true;
	} else return false;
}

