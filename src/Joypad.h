/****************************************************************************
 > File Name: Joypad.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2018-05-04 -- 19:04
 ****************************************************************************/


#pragma once
#include "Base.h"
#include <SDL2/SDL.h>


class Joypad {
private:
	uint8_t const *keypad;
	// R, L, D, U, St, Sel, B, A
	uint8_t joypad_bits[2]; // 手柄1、手柄2移位寄存器
	bool strobe; // 控制是否读入键位

public:
	Joypad();
	// n为0或者1，表明操作第几个手柄
	uint8_t read_joypad_status(int n = 0);
	void write_joypad_status(bool v);

	// 读取键盘上的按键状态
	uint8_t get_joypad_status(int n = 0);
};


