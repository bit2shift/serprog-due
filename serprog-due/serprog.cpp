#include "serprog.hpp"

#include <stddef.h>
#include <stdint.h>

static const char NAME[16] = "serprog-due";

template<typename I>
inline bool read(Stream& s, I& i, size_t n = sizeof(I))
{
	return (s.readBytes(reinterpret_cast<uint8_t*>(&i), n) == n);
}

template<typename I>
inline void write(Print& p, I i)
{
	p.write(reinterpret_cast<uint8_t*>(&i), sizeof(I));
}

template<typename T, size_t N>
constexpr size_t length(const T (&)[N]) noexcept
{
	return N;
}

void serprog::ack()
{
	ios.write(0x06);
}

void serprog::nak()
{
	ios.write(0x15);
}

void serprog::version()
{
	ack();
	write(ios, uint16_t(1));
}

void serprog::map()
{
	ack();
	uint8_t i = 0;
	uint8_t bits = 0;
	do
	{
		bitWrite(bits, (i & 7), ((i < length(cmds)) && cmds[i]));
		if(!(~i & 7))
			ios.write(bits);
	}
	while(++i);
}

void serprog::name()
{
	ack();
	ios.write(NAME, sizeof(NAME));
}

void serprog::size()
{
	ack();
	write(ios, uint16_t(256));
}

void serprog::gbus()
{
	ack();
	ios.write(8);
}

void serprog::sync()
{
	nak();
	ack();
}

void serprog::sbus()
{
	uint8_t bus;
	if(read(ios, bus) && (bus == 8))
		ack();
	else
		nak();
}

void serprog::op()
{
	uint32_t slen = 0;
	if(!read(ios, slen, 3))
		return nak();

	uint32_t rlen = 0;
	if(!read(ios, rlen, 3))
		return nak();

	class guard_t
	{
		serprog& sp;

	public:
		guard_t(serprog* psp) : sp(*psp)
		{
			SPI.beginTransaction(sp.cfg);
			digitalWrite(sp.cs, LOW);
		}

		~guard_t()
		{
			digitalWrite(sp.cs, HIGH);
			SPI.endTransaction();
		}
	} guard{this};

	while(slen--)
	{
		uint8_t c;
		if(read(ios, c))
			SPI.transfer(c);
		else
			return nak();
	}

	ack();

	while(rlen--)
		ios.write(SPI.transfer(0));
}

void serprog::freq()
{
	uint32_t freq;
	if(read(ios, freq) && freq)
	{
		freq = constrain(freq, 4e6, F_CPU);
		cfg = {freq, MSBFIRST, SPI_MODE0};

		ack();
		write(ios, freq);
	}
	else
		nak();
}

void serprog::setup()
{
	pinMode(cs, OUTPUT);
	SPI.begin();
}

void serprog::loop()
{
	if(ios.available())
	{
		uint8_t op = ios.read();
		if((op < length(cmds)) && cmds[op])
			(this->*cmds[op])();
		else
			nak();
	}
}
