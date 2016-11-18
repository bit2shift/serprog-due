#pragma once

#include <Arduino.h>

#include <cstddef>
#include <cstdint>

class TypeSerial
{
	Print& p;
	Stream& s;

public:
	template<typename T>
	explicit TypeSerial(T&& t) : p(t), s(t) {}

	operator std::uint8_t();
	operator std::uint16_t();
	operator std::uint32_t();
	operator std::uint64_t();

	void operator()(std::uint8_t num);
	void operator()(std::uint16_t num);
	void operator()(std::uint32_t num);
	void operator()(std::uint64_t num);

	void operator()(const void* ptr, std::size_t len);
};

inline std::uint8_t operator"" _b(std::uintmax_t num)
{
	return num;
}

inline std::uint16_t operator"" _w(std::uintmax_t num)
{
	return num;
}

inline std::uint32_t operator"" _d(std::uintmax_t num)
{
	return num;
}

inline std::uint64_t operator"" _q(std::uintmax_t num)
{
	return num;
}
