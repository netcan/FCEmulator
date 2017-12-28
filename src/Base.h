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

inline bool GetBit(uint16_t value, size_t pos) { return static_cast<bool>((value >> pos) & 0x01); }
inline uint8_t GetUpperBits(uint8_t value) { return (value >> 4) & 0x0f; }
inline uint8_t GetLowerBits(uint8_t value) { return value & 0x0f; }
inline uint8_t JointBits(uint8_t upper, uint8_t lower) { return (upper << 4) | (lower & 0x0f); }
#define LEN(container) (sizeof(container) / sizeof(container[0]))

template<class MEMType>
class MemIterator: public std::iterator // 内部用32位来表示地址，是因为16位不好判断end()
		<std::random_access_iterator_tag, uint8_t, ptrdiff_t, uint32_t, uint8_t &> {
public:
	MemIterator(): parent(NULL), addr(0) {}
	MemIterator(MEMType *parent, pointer addr): parent(parent), addr(addr) {}

	reference operator*() { return parent->operator[](addr); }
	uint8_t * get_raw_pointer() { return &parent->operator[](addr); }

	MemIterator& operator++() { ++addr; return *this; } // ++it
	MemIterator& operator--() { --addr; return *this; } // --it

	MemIterator& operator+=(const uint16_t value) { this->addr += value; return *this; }
	MemIterator& operator-=(const uint16_t value) { this->addr -= value; return *this; }
	friend MemIterator operator+(MemIterator lhs, const uint16_t& rhs) { lhs += rhs; return lhs; }
	friend MemIterator operator-(MemIterator lhs, const uint16_t& rhs) { lhs -= rhs; return lhs; }

	friend bool operator==(const MemIterator &lhs, const MemIterator &rhs) { return lhs.addr == rhs.addr; }
	friend bool operator!=(const MemIterator& lhs, const MemIterator& rhs) { return !(lhs == rhs); }
	friend bool operator<(const MemIterator &lhs, const MemIterator &rhs) { return lhs.addr < rhs.addr; }

	friend difference_type operator-(const MemIterator &lhs, const MemIterator &rhs) { return lhs.addr - rhs.addr; }

	uint8_t &operator[](uint16_t value) { return parent->operator[](addr + value); }
	const uint8_t &operator[](uint16_t value) const { return parent->operator[](addr + value); }
private:
	pointer addr;
	MEMType *parent;
};

#endif //FCEMU_BASE_H
