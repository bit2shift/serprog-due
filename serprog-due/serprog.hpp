#pragma once

#include <Arduino.h>
#include <SPI.h>

class serprog
{
	Stream& in;
	Print& out;
	SPISettings cfg;
	int cs;

	void (serprog::* cmds[32])()
	{
		&serprog::ack,     /* 0x00 */
		&serprog::version, /* 0x01 */
		&serprog::map,     /* 0x02 */
		&serprog::name,    /* 0x03 */
		&serprog::size,    /* 0x04 */
		&serprog::gbus,    /* 0x05 */
		nullptr,           /* 0x06 */
		nullptr,           /* 0x07 */
		nullptr,           /* 0x08 */
		nullptr,           /* 0x09 */
		nullptr,           /* 0x0A */
		nullptr,           /* 0x0B */
		nullptr,           /* 0x0C */
		nullptr,           /* 0x0D */
		nullptr,           /* 0x0E */
		nullptr,           /* 0x0F */
		&serprog::sync,    /* 0x10 */
		nullptr,           /* 0x11 */
		&serprog::sbus,    /* 0x12 */
		&serprog::op,      /* 0x13 */
		&serprog::freq,    /* 0x14 */
		nullptr            /* 0x15 */
		/* nullptr (default)  0x?? */
	};

	void ack();
	void nak();

	void version();
	void map();
	void name();
	void size();
	void gbus();
	void sync();
	void sbus();
	void op();
	void freq();

public:
	template<typename T>
	serprog(T& t, int pin) : in(t), out(t), cfg(), cs(pin) {}

	void setup();
	void loop();
};
