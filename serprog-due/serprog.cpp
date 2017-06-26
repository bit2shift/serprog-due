#include "serprog.hpp"

#include <stddef.h>
#include <stdint.h>

static const char NAME[16] = "serprog-due";

template<typename I>
inline bool read(Stream& s, I& i, size_t n = sizeof(I))
{
  return (s.readBytes(reinterpret_cast<char*>(&i), n) == n);
}

template<typename I>
inline void write(Print& p, I i)
{
  p.write(reinterpret_cast<char*>(&i), sizeof(I));
}

void serprog::ack()
{
  out.write(0x06);
}

void serprog::nak()
{
  out.write(0x15);
}

void serprog::version()
{
  ack();
  write(out, uint16_t(1));
}

void serprog::map()
{
  ack();
  uint32_t bits = 0;
  for(auto& cmd : cmds)
  {
    int i = ((&cmd - cmds) & 31);
    bitWrite(bits, i, cmd);
    if(i == 31)
      write(out, bits);
  }
}

void serprog::name()
{
  ack();
  out.write(NAME, sizeof(NAME));
}

void serprog::size()
{
  ack();
  write(out, uint16_t(256));
}

void serprog::gbus()
{
  ack();
  out.write(8);
}

void serprog::sync()
{
  nak();
  ack();
}

void serprog::sbus()
{
  char bus;
  if(read(in, bus) && (bus == 8))
    ack();
  else
    nak();
}

void serprog::op()
{
  uint32_t slen = 0;
  if(!read(in, slen, 3))
    return nak();

  uint32_t rlen = 0;
  if(!read(in, rlen, 3))
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
  } guard(this);

  while(slen--)
  {
    char c;
    if(read(in, c))
      SPI.transfer(c);
    else
      return nak();
  }

  ack();

  while(rlen--)
    out.write(SPI.transfer(0));
}

void serprog::freq()
{
  uint32_t freq;
  if(read(in, freq) && freq)
  {
    freq = constrain(freq, 4e6, F_CPU);
    cfg = {freq, MSBFIRST, SPI_MODE0};

    ack();
    write(out, freq);
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
  if(in.available())
  {
    char op = in.read();
    if(cmds[op])
      (this->*cmds[op])();
    else
      nak();
  }
}
