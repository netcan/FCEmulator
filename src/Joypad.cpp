/****************************************************************************
 > File Name: Joypad.cpp
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2018-05-04 -- 19:04
 ****************************************************************************/

#include "Joypad.h"
Joypad::Joypad() {
	memset(joypad_bits, 0, sizeof(joypad_bits));
	keypad = SDL_GetKeyboardState(NULL);
	strobe = false;
}

uint8_t Joypad::get_joypad_status(int n) {
	uint8_t key_status = 0;

	key_status |= keypad[SDL_SCANCODE_K] << 0; // A
	key_status |= keypad[SDL_SCANCODE_J] << 1; // B

	key_status |= keypad[SDL_SCANCODE_SPACE] << 2; // Select
	key_status |= keypad[SDL_SCANCODE_RETURN] << 3; // Start

	key_status |= keypad[SDL_SCANCODE_W] << 4; // Up
	key_status |= keypad[SDL_SCANCODE_S] << 5; // Down
	key_status |= keypad[SDL_SCANCODE_A] << 6; // Left
	key_status |= keypad[SDL_SCANCODE_D] << 7; // Right


	return key_status;
}

uint8_t Joypad::read_joypad_status(int n) {
	// 若S为高电平，则读取A键，由于硬件问题高第二位为1
	if(strobe)
		return 0x40 | (get_joypad_status(n) & 1);

	// 每次读取一位并右移一位
	uint8_t key_status = 0x40 | (joypad_bits[n] & 1);
	joypad_bits[n] = 0x80 | (joypad_bits[n] >> 1);
	return key_status;
}

void Joypad::write_joypad_status(bool v) {
	if(strobe && !v)
		for(int i = 0; i < 2; ++i)
			joypad_bits[i] = get_joypad_status(i);

	strobe = v;
}
