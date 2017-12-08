/****************************************************************************
 > File Name: Base.h
 > Author: Netcan
 > Blog: http://www.netcan666.com/
 > Mail: netcan1996@gmail.com
 > Created Time: 2017-12-05 -- 21:51
 ****************************************************************************/

#ifndef FCEMU_BASE_H
#define FCEMU_BASE_H
#include <cstdint>
#include <string>
#include <cstdio>
#include <cassert>

inline bool GetBit(uint8_t value, size_t pos) { return static_cast<bool>((value >> pos) & 0x01); }
inline uint8_t GetUpperBits(uint8_t value) { return (value >> 4) & 0x0f; }
inline uint8_t GetLowerBits(uint8_t value) { return value & 0x0f; }
inline uint8_t JointBits(uint8_t upper, uint8_t lower) { return (upper << 4) | (lower & 0x0f); }
#define LEN(container) (sizeof(container) / sizeof(container[0]))

#endif //FCEMU_BASE_H
