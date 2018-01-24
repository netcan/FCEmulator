/****************************************************************************
 > File Name: Ppu.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-28 -- 15:48
 ****************************************************************************/

#include "Ppu.h"
#include "Cpu.h"

const SDL_Color PPU::palette[] = {
		{0x75, 0x75, 0x75, 0x00 }, {0x27, 0x1B, 0x8F, 0x00 },
		{0x00, 0x00, 0xAB, 0x00 }, {0x47, 0x00, 0x9F, 0x00 },
		{0x8F, 0x00, 0x77, 0x00 }, {0xAB, 0x00, 0x13, 0x00 },
		{0xA7, 0x00, 0x00, 0x00 }, {0x7F, 0x0B, 0x00, 0x00 },
		{0x43, 0x2F, 0x00, 0x00 }, {0x00, 0x47, 0x00, 0x00 },
		{0x00, 0x51, 0x00, 0x00 }, {0x00, 0x3F, 0x17, 0x00 },
		{0x1B, 0x3F, 0x5F, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0x00, 0x00, 0x00, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0xBC, 0xBC, 0xBC, 0x00 }, {0x00, 0x73, 0xEF, 0x00 },
		{0x23, 0x3B, 0xEF, 0x00 }, {0x83, 0x00, 0xF3, 0x00 },
		{0xBF, 0x00, 0xBF, 0x00 }, {0xE7, 0x00, 0x5B, 0x00 },
		{0xDB, 0x2B, 0x00, 0x00 }, {0xCB, 0x4F, 0x0F, 0x00 },
		{0x8B, 0x73, 0x00, 0x00 }, {0x00, 0x97, 0x00, 0x00 },
		{0x00, 0xAB, 0x00, 0x00 }, {0x00, 0x93, 0x3B, 0x00 },
		{0x00, 0x83, 0x8B, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0x00, 0x00, 0x00, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0xFF, 0xFF, 0xFF, 0x00 }, {0x3F, 0xBF, 0xFF, 0x00 },
		{0x5F, 0x97, 0xFF, 0x00 }, {0xA7, 0x8B, 0xFD, 0x00 },
		{0xF7, 0x7B, 0xFF, 0x00 }, {0xFF, 0x77, 0xB7, 0x00 },
		{0xFF, 0x77, 0x63, 0x00 }, {0xFF, 0x9B, 0x3B, 0x00 },
		{0xF3, 0xBF, 0x3F, 0x00 }, {0x83, 0xD3, 0x13, 0x00 },
		{0x4F, 0xDF, 0x4B, 0x00 }, {0x58, 0xF8, 0x98, 0x00 },
		{0x00, 0xEB, 0xDB, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0x00, 0x00, 0x00, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0xFF, 0xFF, 0xFF, 0x00 }, {0xAB, 0xE7, 0xFF, 0x00 },
		{0xC7, 0xD7, 0xFF, 0x00 }, {0xD7, 0xCB, 0xFF, 0x00 },
		{0xFF, 0xC7, 0xFF, 0x00 }, {0xFF, 0xC7, 0xDB, 0x00 },
		{0xFF, 0xBF, 0xB3, 0x00 }, {0xFF, 0xDB, 0xAB, 0x00 },
		{0xFF, 0xE7, 0xA3, 0x00 }, {0xE3, 0xFF, 0xA3, 0x00 },
		{0xAB, 0xF3, 0xBF, 0x00 }, {0xB3, 0xFF, 0xCF, 0x00 },
		{0x9F, 0xFF, 0xF3, 0x00 }, {0x00, 0x00, 0x00, 0x00 },
		{0x00, 0x00, 0x00, 0x00 }, {0x00, 0x00, 0x00, 0x00 }
};

PPU::PPU() {

	screen_width = 256;
	screen_height = 240;
	cycles = 0;

	// power up status
	odd_frame = false;
	PPUCTRL.ctrl = PPUMASK.mask = PPUSTATUS.status = 0;
	OAMADDR = 0;
	v.addr = t.addr = fineX = w = 0;
	PPUSCROLL = PPUADDR = 0;


	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		exit(-1);
	}

	window = SDL_CreateWindow("Famicom emulator by netcan",
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  screen_width * 3, screen_height * 3,
							  SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screen_width, screen_height);
	SDL_SetRenderTarget(renderer, texture);
}

PPU::~PPU() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void PPU::showPalette() {
	SDL_SetRenderTarget(renderer, texture);
	SDL_Rect color_tile {0, 0, 15, 15};

	// 绘制调色板
	for (int j = 0; j < 4; ++j) {
		for(int i = 0; i < 16; ++i) {
			color_tile.x = i * color_tile.w;
			color_tile.y = j * color_tile.h;
			const SDL_Color & color = palette[j * 16 + i];
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderFillRect(renderer, &color_tile);
		}
	}

	// 渲染调色板
	SDL_Rect board {0, 0, 16 * color_tile.w, 4 * color_tile.h};
	SDL_SetWindowSize(window, board.w * 3, board.h * 3);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, &board, NULL);

	SDL_RenderPresent(renderer);
	while(event.type != SDL_QUIT)
		SDL_PollEvent(&event);

}

void PPU::showPatternTable() {
	SDL_SetRenderTarget(renderer, texture);
	const SDL_Color tile_colors[] = {
			palette[0x00],
			palette[0x01],
			palette[0x02],
			palette[0x03],
	};


	// left and right，绘制PatternTable
	for(int j = 0; j < 16; ++j) {
		for(int i = 0; i < 32; ++i) {
			for(int y = 0; y < 8; ++y) {
				uint16_t lower_tile_addr = y | ((i & 0xf) << 0x4) | j << 0x8 | GetBit(i, 4) << 0xC,
				         upper_tile_addr = lower_tile_addr | 0x8;
				uint8_t lower_tile = mem[lower_tile_addr],
						upper_tile = mem[upper_tile_addr];

				for (int x = 0; x < 8; ++x) {
					const SDL_Color &color = tile_colors[ (GetBit(upper_tile, 7 - x) << 0x1) | GetBit(lower_tile, 7-x) ];
					SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
					SDL_RenderDrawPoint(renderer, i * 8 + x, j * 8 + y);

				}
			}
		}
	}

	// 渲染Pattern Table
	SDL_Rect board {0, 0, 256, 128};
	SDL_SetWindowSize(window, board.w * 3, board.h * 3);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, &board, NULL);

	SDL_RenderPresent(renderer);
	while(event.type != SDL_QUIT)
		SDL_PollEvent(&event);
}

void PPU::pixel(unsigned x, unsigned y) {
	uint8_t draw_palette;

	if(y < 240 && x >= 0 && x < 256) {
		if (PPUMASK.b && !(!PPUMASK.m && x < 8)) {
			draw_palette = GetBit(bgShiftH, 15 - fineX) << 1 |
			               GetBit(bgShiftL, 15 - fineX);
			if (draw_palette)
				draw_palette |= (GetBit(atShiftH, 7 - fineX) << 1 |
				                 GetBit(atShiftL, 7 - fineX)) << 2;
		}


		const SDL_Color &color = palette[mem[0x3F00 + (rendering() ? draw_palette : 0)]];
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawPoint(renderer, x, y);
	}

	bgShiftL <<= 1; bgShiftH <<= 1;
	atShiftL = (atShiftL << 1) | atL;
	atShiftH = (atShiftH << 1) | atH;
}

void PPU::reload_shift() {
	bgShiftL = (bgShiftL & 0xFF00) | bgL;
	bgShiftH = (bgShiftH & 0xFF00) | bgH;

	atL= (at & 1);
	atH = (at & 2);
}

void PPU::h_scroll() {
	if(! rendering()) return;
	if (v.coarseX == 31) {      // if coarse X == 31
		v.coarseX = 0;          // coarse X = 0
		v.NN ^= 0x1;             // switch horizontal nametable
	} else ++v.coarseX;      // increment coarse X
}

void PPU::v_scroll() {
	if(! rendering()) return;
	if(v.fineY < 7) ++v.fineY;
	else {
		v.fineY = 0;
		if(v.coarseY == 29) { v.coarseY = 0; v.NN ^= 0x2; }
		else if(v.coarseY == 31) v.coarseY = 0;
		else ++v.coarseY;
	}
}

void PPU::h_update() {
	if(! rendering()) return;
	v.addr = (v.addr & ~0x041F) | (t.addr & 0x041F);
}

void PPU::v_update() {
	if(! rendering()) return;
	v.addr = (v.addr & ~0x7BE0) | (t.addr & 0x7BE0);
}

void PPU::step() {
	uint16_t    scanline = cycles / frame_width,
				dot = cycles % frame_width;
	uint16_t addr;

	if( (scanline >= 0 && scanline < 240) || scanline == 261) { // Visable frame or Pre-render line(dummy scanline)

		// 背景
		switch (dot) {
			case 2 ... 255: case 322 ... 337:
				pixel(dot - 2, scanline);
				switch (dot % 8) {
					// Nametable:
					case 1:  addr  = get_nt_addr(); reload_shift(); break;
					case 2:  nt    = mem[addr];  break;
					// Attribute:
					case 3:  addr  = get_at_addr(); break;
					case 4:  at    = mem[addr]; if (v.coarseY & 2) at >>= 4;
												if (v.coarseX & 2) at >>= 2; break;
					// Background (low bits):
					case 5:  addr  = get_bg_tile_addr(); break;
					case 6:  bgL   = mem[addr];  break;
					// Background (high bits):
					case 7:  addr += 8;         break;
					case 0:  bgH   = mem[addr]; h_scroll(); break;
				} break;
			case         256:  pixel(dot - 2, scanline); bgH = mem[addr]; v_scroll(); break;  // Vertical bump.
			case         257:  pixel(dot - 2, scanline); reload_shift(); h_update(); break;  // Update horizontal position.
			case 280 ... 304:  if (scanline == 261)            v_update(); break;  // Update vertical position.

				// No shift reloading:
			case             1:  addr = get_nt_addr(); if(scanline == 261) PPUSTATUS.V = 0; break;
			case 321: case 339:  addr = get_nt_addr(); break;
				// Nametable fetch instead of attribute:
			case           338:  nt = mem[addr]; break;
			case           340:  nt = mem[addr]; if(scanline == 261 && rendering() && odd_frame) ++cycles; break;
		}
	}
	else if(scanline == 240 && dot == 0) { // frame end
		printf("one frame cpu cycles: %d\n", cpu->cycles);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_SetRenderTarget(renderer, texture);
		odd_frame = !odd_frame;
	} else if(scanline == 241 && dot == 1) { // set VBlank flag
		PPUSTATUS.V = 1;
		if(PPUCTRL.V) cpu->nmi = true;
	}

	if(++cycles >= (frame_width * frame_height)) {
		cycles %= (frame_width * frame_height);
		odd_frame = !odd_frame;
	}



}

void PPU::Execute(uint8_t cpu_cycles) {
	for(uint8_t remain_cycles = 3 * cpu_cycles; remain_cycles; --remain_cycles) step();
}