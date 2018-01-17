/****************************************************************************
 > File Name: Ppu.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-28 -- 15:48
 ****************************************************************************/
#pragma once
#include "Base.h"
#include <SDL2/SDL.h>


class __PPUMem__ {
private:
	uint8_t VRAM[0x800];                // 2KB，存放2个NameTable
	uint8_t OAM[0x100];                 // 256B SPR RAM
	uint8_t PatternTable[2][0x1000];    // 4KB * 2
	uint8_t *NameTable[4][0x400];       // 1KB * 4
	uint8_t Palette[0x20];              // 32B
	inline const uint8_t &AT(uint16_t addr) const {
		const uint8_t *ret =
				&(addr < 0x2000 ? PatternTable[addr >> 0x0c][addr & 0xfff]:
				  addr < 0x3000 ? *NameTable[(addr >> 0x0a) & 0x03][addr & 0x3ff]:
				  addr < 0x3f00 ? AT(addr & 0x2fff) :
				  addr < 0x4000 ? Palette[addr & 0x1f]:
				  AT(addr & 0x3fff));

		if(addr >= 0x3f00 && addr < 0x4000 && (addr & 0x1f) % 4 == 0)
			ret = &Palette[0x00];
		return *ret;
	}

	inline uint8_t &AT(uint16_t addr) { // 复用const版本
		return const_cast<uint8_t &>(
				static_cast<const __PPUMem__&>(*this).AT(addr)
		);
	}

public:
	__PPUMem__() { setHorizontalMirroring(); }
	uint8_t &operator[](uint16_t addr) { return AT(addr); }
	const uint8_t &operator[](uint16_t addr) const { return AT(addr); }

	using iterator = MemIterator<__PPUMem__>;
	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, 0x10000); }

	void setHorizontalMirroring() {
		for (int addr = 0; addr < 0x400; ++addr) {
			NameTable[0][addr] = NameTable[1][addr] = &VRAM[addr];
			NameTable[2][addr] = NameTable[3][addr] = &VRAM[addr + 0x400];
		}
	}

	void setVerticalMirroring() {
		for (int addr = 0; addr < 0x400; ++addr) {
			NameTable[0][addr] = NameTable[2][addr] = &VRAM[addr];
			NameTable[1][addr] = NameTable[3][addr] = &VRAM[addr + 0x400];
		}
	}

};

class PPU {
private:
	__PPUMem__ mem;
	uint8_t PPUCTRL, PPUMASK, PPUSTATUS,
			OAMADDR, OAMDATA, PPUSCROLL,
			PPUADDR, PPUDATA, OAMDMA;
	uint32_t cycles; // PPU的时钟周期数，是cpu的三倍

	static const SDL_Color palette[0x40];

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Event event;

	size_t screen_width, screen_height;
	const size_t frame_width = 341, frame_height = 262; // NTSC, 60fps
public:
	friend class __CPUMem__;
	friend class Cartridge;
	// 读取一个字节
	inline uint8_t Read8(uint16_t addr) const { return mem[addr]; }
	PPU(): cycles(0) {};
	~PPU();
	void showPalette();
	void showPatternTable();

	void Execute(uint8_t cycle); // 执行cycle个周期，这里的cycle是cpu返回的周期，需要*3

	uint8_t getPPUCTRL() const { return PPUCTRL; }
	uint8_t getPPUMASK() const { return PPUMASK; }
	uint8_t getPPUSTATUS() const { return PPUSTATUS; }
	uint8_t getOAMADDR() const { return OAMADDR; }
	uint8_t getOAMDATA() const { return OAMDATA; }
	uint8_t getPPUSCROLL() const { return PPUSCROLL; }
	uint8_t getPPUADDR() const { return PPUADDR; }
	uint8_t getPPUDATA() const { return PPUDATA; }
	uint8_t getOAMDMA() const { return OAMDMA; }

};

