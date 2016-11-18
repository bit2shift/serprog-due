#include "TypeSerial.h"

TypeSerial::operator std::uint8_t()
{
	while(!s.available())
		asm volatile("NOP");
	return s.read();
}

TypeSerial::operator std::uint16_t()
{
	std::uint8_t a = (*this);
	std::uint8_t b = (*this);
	return (a | b << 8);
}

TypeSerial::operator std::uint32_t()
{
	std::uint16_t a = (*this);
	std::uint8_t b = (*this);
	return (a | b << 16);
}

TypeSerial::operator std::uint64_t()
{
	std::uint16_t a = (*this);
	std::uint16_t b = (*this);
	return (a | b << 16);
}

////////////////////////////////////////////////////////////////

void TypeSerial::operator()(const void* ptr, std::size_t len)
{
	p.write(static_cast<const byte*>(ptr), len);
}

////////////////////////////////////////////////////////////////

void TypeSerial::operator()(std::uint8_t num)
{
	(*this)(&num, sizeof(num));
}

void TypeSerial::operator()(std::uint16_t num)
{
	(*this)(&num, sizeof(num));
}

void TypeSerial::operator()(std::uint32_t num)
{
	(*this)(&num, 3);
}

void TypeSerial::operator()(std::uint64_t num)
{
	(*this)(&num, 4);
}
