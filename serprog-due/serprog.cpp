#include "serprog.hpp"

#include <bitset>

static const char NAME[16] = "serprog-due";

template<typename I>
inline bool read(Stream& s, I& i, unsigned n = sizeof(I))
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
  write(out, short(1));
}

void serprog::map()
{
  ack();
  std::bitset<32> bits;
  for(auto& cmd : cmds)
  {
    int i = ((&cmd - cmds) % bits.size());
    bits.set(i, cmd);
    if(i == (bits.size() - 1))
      write(out, bits.to_ulong());
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
  write(out, short(256));
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
  unsigned slen = 0;
  if(!read(in, slen, 3))
    return nak();

  unsigned rlen = 0;
  if(!read(in, rlen, 3))
    return nak();

  SPI.beginTransaction(cfg);
  digitalWrite(52, LOW);

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

  digitalWrite(52, HIGH);
  SPI.endTransaction();
}

void serprog::freq()
{
  unsigned freq = 0;
  if(read(in, freq) && freq)
  {
    freq = constrain(freq, 4e6, F_CPU);
    cfg = {unsigned(freq), MSBFIRST, SPI_MODE0};

    ack();
    write(out, freq);
  }
  else
    nak();
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
