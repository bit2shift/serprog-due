#pragma once

#include <Arduino.h>
#include <SPI.h>

class serprog
{
  Stream& in;
  Print& out;
  SPISettings cfg;
  
  void(serprog::*cmds[256])(){
    &serprog::ack,
    &serprog::version,
    &serprog::map,
    &serprog::name,
    &serprog::size,
    &serprog::gbus
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
  serprog(T&& t) : in(t), out(t), cfg()
  {
    cmds[0x10] = &serprog::sync;
    cmds[0x12] = &serprog::sbus;
    cmds[0x13] = &serprog::op;
    cmds[0x14] = &serprog::freq;
  }
  
  void loop()
  {
    if(in.available())
    {
      char op = in.read();
      if(cmds[op])
        (this->*cmds[op])();
      else
        nak();
    }
  }
};

