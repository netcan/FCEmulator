/****************************************************************************
 > File Name: Ppu.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-28 -- 15:48
 ****************************************************************************/

#include "Ppu.h"
#include "Cpu.h"

const uint32_t PPU::palette[] = {
		0x757575, 0x271B8F, 0x0000AB, 0x47009F, 0x8F0077, 0xAB0013, 0xA70000, 0x7F0B00,
		0x432F00, 0x004700, 0x005100, 0x003F17, 0x1B3F5F, 0x000000, 0x000000, 0x000000,
		0xBCBCBC, 0x0073EF, 0x233BEF, 0x8300F3, 0xBF00BF, 0xE7005B, 0xDB2B00, 0xCB4F0F,
		0x8B7300, 0x009700, 0x00AB00, 0x00933B, 0x00838B, 0x000000, 0x000000, 0x000000,
		0xFFFFFF, 0x3FBFFF, 0x5F97FF, 0xA78BFD, 0xF77BFF, 0xFF77B7, 0xFF7763, 0xFF9B3B,
		0xF3BF3F, 0x83D313, 0x4FDF4B, 0x58F898, 0x00EBDB, 0x000000, 0x000000, 0x000000,
		0xFFFFFF, 0xABE7FF, 0xC7D7FF, 0xD7CBFF, 0xFFC7FF, 0xFFC7DB, 0xFFBFB3, 0xFFDBAB,
		0xFFE7A3, 0xE3FFA3, 0xABF3BF, 0xB3FFCF, 0x9FFFF3, 0x000000, 0x000000, 0x000000
};

PPU::PPU() {
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
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  screen_width * 3, screen_height * 3,
							  SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
	SDL_SetRenderTarget(renderer, texture);
}

PPU::~PPU() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void PPU::showPalette() {
	// TODO: Refactor show palette
//	SDL_Rect color_tile {0, 0, 15, 15};
//	SDL_SetRenderTarget(renderer, texture);
//	// 绘制调色板
//	for (int j = 0; j < 4; ++j) {
//		for(int i = 0; i < 16; ++i) {
//			color_tile.x = i * color_tile.w;
//			color_tile.y = j * color_tile.h;
//			uint32_t c = palette[j * 16 + i];
//			SDL_Color color {static_cast<uint8_t>((c >> 16) & 0xFF),
//			                 static_cast<uint8_t>((c >> 8) & 0xFF),
//			                 static_cast<uint8_t>(c & 0xFF),  0x00};
//			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
//			SDL_RenderFillRect(renderer, &color_tile);
//		}
//	}
//
//	// 渲染调色板
//	SDL_Rect board {0, 0, 16 * color_tile.w, 4 * color_tile.h};
//	SDL_SetWindowSize(window, board.w * 3, board.h * 3);
//	SDL_SetRenderTarget(renderer, NULL);
//	SDL_RenderCopy(renderer, texture, &board, NULL);
//
//	SDL_RenderPresent(renderer);

	while(event.type != SDL_QUIT)
		SDL_PollEvent(&event);

}

void PPU::showPatternTable() {
	// TODO: refactor show pattern table
//	SDL_SetRenderTarget(renderer, texture);
//	const uint32_t tile_colors[] = {
//			palette[0x00],
//			palette[0x01],
//			palette[0x02],
//			palette[0x03],
//	};
//
//
//	// left and right，绘制PatternTable
//	for(int j = 0; j < 16; ++j) {
//		for(int i = 0; i < 32; ++i) {
//			for(int y = 0; y < 8; ++y) {
//				uint16_t lower_tile_addr = y | ((i & 0xf) << 0x4) | j << 0x8 | GetBit(i, 4) << 0xC,
//				         upper_tile_addr = lower_tile_addr | 0x8;
//				uint8_t lower_tile = mem[lower_tile_addr],
//						upper_tile = mem[upper_tile_addr];
//
//				for (int x = 0; x < 8; ++x) {
//					uint32_t c = tile_colors[ (GetBit(upper_tile, 7 - x) << 0x1) | GetBit(lower_tile, 7-x) ];
//					SDL_Color color {static_cast<uint8_t>((c >> 0x8) & 0xFF),
//					                 static_cast<uint8_t>((c >> 0x4) & 0xFF),
//					                 static_cast<uint8_t>(c & 0xFF),  0x00};
//					SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
//					SDL_RenderDrawPoint(renderer, i * 8 + x, j * 8 + y);
//
//				}
//			}
//		}
//	}
//
//	// 渲染Pattern Table
//	SDL_Rect board {0, 0, 256, 128};
//	SDL_SetWindowSize(window, board.w * 3, board.h * 3);
//	SDL_SetRenderTarget(renderer, NULL);
//	SDL_RenderCopy(renderer, texture, &board, NULL);

	SDL_RenderPresent(renderer);
	while(event.type != SDL_QUIT)
		SDL_PollEvent(&event);
}

void PPU::pixel(unsigned x, unsigned y) {
	uint8_t draw_palette = 0;

	if(y < 240 && x >= 0 && x < 256) {
		if (PPUMASK.b && !(!PPUMASK.m && x < 8)) {
			draw_palette = GetBit(bgShiftH, 15 - fineX) << 1 |
			               GetBit(bgShiftL, 15 - fineX);
			if (draw_palette)
				draw_palette |= (GetBit(atShiftH, 7 - fineX) << 1 |
				                 GetBit(atShiftL, 7 - fineX)) << 2;
		}


		video_buffer[screen_width * y + x] = palette[mem[0x3F00 + (rendering() ? draw_palette : 0)]];
	}

	bgShiftL <<= 1; bgShiftH <<= 1;
	atShiftL = (atShiftL << 1) | atL;
	atShiftH = (atShiftH << 1) | atH;
}

void PPU::reload_shift() {
	bgShiftL = static_cast<uint16_t>((bgShiftL & 0xFF00) | bgL);
	bgShiftH = static_cast<uint16_t>((bgShiftH & 0xFF00) | bgH);

	atL= static_cast<bool>(at & 1);
	atH = static_cast<bool>((at >> 0x01) & 1);
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

	if( (scanline >= 0 && scanline < 240) || scanline == 261) { // Visable frame or Pre-render line(dummy scanline)
		// 背景
		switch (dot) {
			case 2 ... 255:
			case 322 ... 337:
				pixel(dot - 2, scanline);
				switch (dot % 8) {
					// Nametable:
					case 1:  internal_load_addr  = get_nt_addr(); reload_shift(); break;
					case 2:  nt    = mem[internal_load_addr];  break;
					// Attribute:
					case 3:  internal_load_addr  = get_at_addr(); break;
					case 4:  at    = mem[internal_load_addr];
						if (v.coarseY & 2) at >>= 4;
						if (v.coarseX & 2) at >>= 2;
						break;
					// Background (low bits):
					case 5:  internal_load_addr  = get_bg_tile_addr(); break;
					case 6:  bgL   = mem[internal_load_addr];  break;
					// Background (high bits):
					case 7:  internal_load_addr += 8;         break;
					case 0:  bgH   = mem[internal_load_addr]; h_scroll(); break;
				} break;
			case         256:  pixel(dot - 2, scanline); bgH = mem[internal_load_addr]; v_scroll(); break;  // Vertical bump.
			case         257:  pixel(dot - 2, scanline); reload_shift(); h_update(); break;  // Update horizontal position.
			case 280 ... 304:  if (scanline == 261)            v_update(); break;  // Update vertical position.

			// No shift reloading:
			case             1:  internal_load_addr = get_nt_addr(); if(scanline == 261) PPUSTATUS.V = 0; break;
			case 321: case 339:  internal_load_addr = get_nt_addr(); break;
			// Nametable fetch instead of attribute:
			case           338:  nt = mem[internal_load_addr]; break;
			case           340:  nt = mem[internal_load_addr]; if(scanline == 261 && rendering() && odd_frame) ++cycles; break;
		}
	}
	else if(scanline == 240 && dot == 0) { // frame end
//		printf("one frame cpu cycles: %d\n", cpu->cycles);
		SDL_UpdateTexture(texture, NULL, video_buffer, sizeof(uint32_t) * screen_width);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		odd_frame = !odd_frame;
		++frames_count;
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