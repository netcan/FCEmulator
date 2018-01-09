#include "SDL2/SDL.h"
#include <iostream>
#include <complex>

/* Moving Rectangle */
int main(int argc, char *argv[])
{
	using namespace std::complex_literals;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	int w = 1024, h = 768;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	window = SDL_CreateWindow("SDL Test",
	                          SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED,
	                          w, h,
	                          SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

	// 绘图, Mandelbrot Set
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00); // black
	SDL_RenderClear(renderer);
	double  MinRe = -2.0,
			MaxRe = 1.0,
			MinIm = -1.2,
			MaxIm = MinIm+(MaxRe-MinRe)*h/w;

	double  Re_factor = (MaxRe-MinRe)/(w-1),
			Im_factor = (MaxIm-MinIm)/(h-1);

	unsigned MaxIterations = 100;
	unsigned MaxColor = 0xff0000;
	for(int y = 0; y < h; ++y) {
		for(int x = 0; x < w; ++x) {
			std::complex<double> c(MinRe + x * Re_factor, MaxIm - y * Im_factor);
			std::complex<double> Z(c);

			int i = 0;
			for(i = 0; i < MaxIterations && std::abs(Z) <= 2; ++i)
				Z = Z * Z + c;

			// (MaxColor >> 0x10) & 0xff, (MaxColor >> 0x08) & 0xff, MaxColor & 0xff
			if(i < MaxIterations) { // outer of Mandelbrot Set
				double nsmooth = i + 1 - log2(std::log2(std::abs(Z)));
				unsigned color = nsmooth * (MaxColor / MaxIterations) / 50;
				SDL_SetRenderDrawColor(renderer, (color >> 0x10) & 0xff, (color >> 0x08) & 0xff, color & 0xff, 0x00);
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}
	}



	// 渲染
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	// 事件处理
	SDL_Event event;
	while (event.type != SDL_QUIT)
		SDL_PollEvent(&event);
	return 0;
}
