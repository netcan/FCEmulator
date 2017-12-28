/****************************************************************************
 > File Name: Ppu.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-28 -- 15:48
 ****************************************************************************/

#ifndef FCEMU_PPU_H
#define FCEMU_PPU_H
#include "Base.h"

class __PPUMem__ {
private:
	uint8_t VRAM[0x800];                // 2KB，存放2个NameTable
	uint8_t SPRRAM[0x100];              // 256B SPR RAM，也就是OAM
	uint8_t PatternTable[2][0x1000];    // 4KB * 2
	uint8_t *NameTable[4][0x400];       // 1KB * 4
	uint8_t Palette[0x20];              // 32B
	inline const uint8_t &AT(uint16_t addr) const {
		return addr < 0x2000 ? PatternTable[addr >> 0x0c][addr & 0xfff]:
		       addr < 0x3000 ? *NameTable[(addr >> 0x0a) & 0x03][addr & 0x3ff]:
		       addr < 0x3f00 ? AT(addr & 0x2fff) :
		       addr < 0x4000 ? Palette[addr & 0x1f]:
		       AT(addr & 0x3fff);
	}

	inline uint8_t &AT(uint16_t addr) { // 复用const版本
		return const_cast<uint8_t &>(
				static_cast<const __PPUMem__&>(*this).AT(addr)
		);
	}

public:
	__PPUMem__() { memset(NameTable, 0x0, sizeof(NameTable)); }
	uint8_t &operator[](uint16_t addr) { return AT(addr); }
	const uint8_t &operator[](uint16_t addr) const { return AT(addr); }

	using iterator = MemIterator<__PPUMem__>;
	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, 0x10000); }
};

class PPU {
private:
	__PPUMem__ mem;

};


#endif //FCEMU_PPU_H
