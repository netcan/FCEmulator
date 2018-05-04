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

class CPU;
class __PPUMem__ {
private:
	uint8_t VRAM[0x800];                // 2KB，存放2个NameTable
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
			ret = &Palette[addr & 0xf];

		return *ret;
	}

	inline uint8_t &AT(uint16_t addr) { // 复用const版本
		return const_cast<uint8_t &>(
				static_cast<const __PPUMem__&>(*this).AT(addr)
		);
	}

public:
	friend class Famicom;
	__PPUMem__() {
		memset(VRAM, 0xFF, sizeof(VRAM));
		setHorizontalMirroring();
	}
	uint8_t &operator[](uint16_t addr) { return AT(addr); }
	const uint8_t &operator[](uint16_t addr) const { return AT(addr); }

	using iterator = MemIterator<__PPUMem__>;
	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, 0x4000); }

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

	union {
		struct {
			unsigned NN: 2; // nametable的基地址
			unsigned I: 1;  // CPU读写PPUDATA后VRAM地址+1/32
			unsigned S: 1;  // 8x8精灵的pattern table地址
			unsigned B: 1;  // 背景的pattern table地址
			unsigned H: 1;  // 精灵的尺寸
			unsigned P: 1;  // PPU的主从模式选择
			unsigned V: 1;  // 是否产生NMI中断（当VBLANK开始的时候）
		};
		uint8_t ctrl;
	} PPUCTRL;

	union {
		struct {
			unsigned g: 1;  // 灰度
			unsigned m: 1;  // 屏幕最左边8个像素是否显示背景
			unsigned M: 1;  // 屏幕最左边8个像素是否显示精灵
			unsigned b: 1;  // 是否显示背景
			unsigned s: 1;  // 是否显示精灵
			unsigned R: 1;  // Emphasize red
			unsigned G: 1;  // Emphasize green
			unsigned B: 1;  // Emphasize blue
		};
		uint8_t mask;
	} PPUMASK;

	union {
		struct {
			unsigned empty: 5;  // 常量
			unsigned O: 1;      // Sprite overflow
			unsigned S: 1;      // Sprite 0 Hit
			unsigned V: 1;      // Vblank是否开始
		};
		uint8_t status;
	} PPUSTATUS;

	union {
		struct {
			unsigned coarseX: 5; // 确定nametable块的X
			unsigned coarseY: 5; // 确定nametable块的Y
			unsigned NN: 2;      // nametable
			unsigned fineY: 3;   // 8x8块中的Y行
		};
		struct {
			unsigned l: 8;
			unsigned h: 6;
			unsigned s: 1; // 15位
		};
		unsigned addr: 15;
	} v, t;         // current vram addr, temporary vram addr
	uint8_t fineX;  // 8x8块中的X列
	bool w;         // First or second write toggle
	bool odd_frame; // 奇偶帧
	uint32_t frames_count = 0; // 统计帧数

	struct {
		uint8_t id;
		uint8_t Y, tileIdx, Attr, X;
		uint8_t tileL, tileH;
	} secOAM[8], sprTile[8];                    // 每行最多8个sprites, 每行绘制的像素数据

	uint16_t bgShiftL, bgShiftH;
	uint8_t bgL, bgH, nt, at;   // nt存放的是pattern table的index
	uint8_t atShiftL, atShiftH;
	bool atL, atH;
	uint16_t internal_load_addr; // PPU读取像素数据用的地址

	uint8_t OAMADDR, OAMDATA, PPUSCROLL,
			PPUADDR, PPUDATA, OAMDMA;
	uint8_t PPUDATA_buffer;
	uint8_t OAM[0x100];                 // 256B SPR RAM
	uint32_t cycles; // PPU的时钟周期数，是cpu的三倍

	static const uint32_t palette[0x40];

	const static uint16_t frame_width = 341, frame_height = 262; // NTSC, 60fps
	const static uint16_t screen_width = 256, screen_height = 240;
	constexpr static double frame_duration = 1000.0 / 60; // 每帧的时间
	// 稳定帧率用的计时器
	double cur_time;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Event event;
	uint32_t video_buffer[frame_width * frame_height];

	CPU *cpu;

	void step();    // 执行一个ppu周期
	void clear_OAM();
	void pixel(unsigned x, unsigned y);
	void eval_sprites(unsigned y);
	void load_sprites(unsigned y);
public:
	friend class __CPUMem__;
	friend class CPU;
	friend class Cartridge;
	friend class Famicom;
	PPU();
	~PPU();
	inline void connectTo(CPU &cpu) { this->cpu = &cpu; }

	void showPalette();
	void showPatternTable();
	static double getMilliseconds() {
		static Uint64 start = SDL_GetPerformanceCounter();
		return (SDL_GetPerformanceCounter() - start) * 1000.0 / SDL_GetPerformanceFrequency();
	}

	void Execute(uint16_t cpu_cycles); // 执行cycle个周期，这里的cycle是cpu返回的周期，需要*3

	uint8_t getPPUCTRL() const { return PPUCTRL.ctrl; }
	uint8_t getPPUMASK() const { return PPUMASK.mask; }
	uint8_t getPPUSTATUS() const { return PPUSTATUS.status; }
	uint8_t getOAMADDR() const { return OAMADDR; }
	uint8_t getOAMDATA() const { return OAMDATA; }
	uint8_t getPPUSCROLL() const { return PPUSCROLL; }
	uint8_t getPPUADDR() const { return PPUADDR; }
	uint8_t getPPUDATA() const { return PPUDATA; }
	uint8_t getOAMDMA() const { return OAMDMA; }
	__PPUMem__& getPPUMEM()  { return mem; }

	void reload_shift();
	void h_scroll();
	void v_scroll();
	void h_update();
	void v_update();
	inline bool rendering() { return PPUMASK.s | PPUMASK.b; }
	inline uint16_t get_nt_addr() { return 0x2000 | (v.addr & 0x0FFF); }
	inline uint16_t get_at_addr() { return 0x23C0 | (v.NN << 10) | ((v.coarseY / 4) << 3) | (v.coarseX / 4); }
	inline uint16_t get_bg_tile_addr() { return (PPUCTRL.B * 0x1000) | (nt * 16) | v.fineY; }
};

