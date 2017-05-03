#pragma once

#include <Arduino.h>
#include <SPI.h>

class serprog
{
  Stream& in;
  Print& out;
  SPISettings cfg;

  void(serprog::* cmds[256])()
  {
    /* 0x00 */ &serprog::ack,
    /* 0x01 */ &serprog::version,
    /* 0x02 */ &serprog::map,
    /* 0x03 */ &serprog::name,
    /* 0x04 */ &serprog::size,
    /* 0x05 */ &serprog::gbus,
    /* 0x06 */ nullptr,
    /* 0x07 */ nullptr,
    /* 0x08 */ nullptr,
    /* 0x09 */ nullptr,
    /* 0x0A */ nullptr,
    /* 0x0B */ nullptr,
    /* 0x0C */ nullptr,
    /* 0x0D */ nullptr,
    /* 0x0E */ nullptr,
    /* 0x0F */ nullptr,
    /* 0x10 */ &serprog::sync,
    /* 0x11 */ nullptr,
    /* 0x12 */ &serprog::sbus,
    /* 0x13 */ &serprog::op,
    /* 0x14 */ &serprog::freq,
    /* 0x15 */ nullptr
    /* 0x??    nullptr (by default) */
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
  serprog(T&& t) : in(t), out(t), cfg() {}

  void loop();
};
