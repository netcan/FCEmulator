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

void PPU::step() {
	uint16_t    scanline = cycles / frame_width,
				dot = cycles % frame_width;
	if(scanline >= 0 && scanline < 240) { // Visable frame

	}
	if(scanline == 241 && dot == 1) { // set VBlank flag
		PPUSTATUS.V = 1;
		if(PPUCTRL.V) cpu->nmi = true;
	} else if(scanline == 261) { // Pre-render line
		if(dot == 1) PPUSTATUS.V = 0;
	}


	cycles = (cycles + 1) % (frame_width * frame_height);
}

void PPU::Execute(uint8_t cpu_cycles) {
	uint8_t remain_cycles = 3 * cpu_cycles;
	for(; remain_cycles; --remain_cycles) step();

}