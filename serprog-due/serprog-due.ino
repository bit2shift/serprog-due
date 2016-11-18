/*
 * Serprog for Arduino Due using software SPI
 * Use a Pomona SOIC-8 clip to program a soldered serial flash
 *
 * SOP8 clip pinout
 * 1 -- /CS     (CS)
 * 2 -- DO      (MISO)
 * 3 -- /WP     (pull-up)
 * 4 -- GND     (GND)
 * 5 -- DI      (MOSI)
 * 6 -- CLK     (SCLK)
 * 7 -- /HOLD   (pull-up)
 * 8 -- VCC     (3.3V)
 */

#include <SPI.h>
#include <TypeSerial.h>
#include <bitset>

/*
 * Implemented commands
 * 0x00 (NOP)
 * 0x01 (Interface version 0x01)
 * 0x02 (Command bitmap)
 * 0x03 (Programmer name)
 * 0x04 (Serial buffer size)
 * 0x05 (Supported bus types)
 * ----
 * 0x10 (Sync NOP)
 * ----
 * 0x12 (Use bus type)
 * 0x13 (SPI OP)
 * 0x14 (Set SPI frequency)
 */

const byte S_ACK = 0x06;
const byte S_NAK = 0x15;
const byte S_PGMNAME[16] = "serprog-due";

TypeSerial serial(SerialUSB);
void (*CMD[256])() = {0};
SPISettings config;

void cmd_NOP()
{
  serial(S_ACK);
}

void cmd_IFACE()
{
  serial(S_ACK);
  serial(1_w);
}

void cmd_CMDMAP()
{
  serial(S_ACK);
  std::bitset<32> bits;
  for(int i = 0; i < 256; i++)
  {
    bits.set((i & 31), CMD[i]);
    if((i & 31) == 31)
      serial(bits.to_ullong());
  }
}

void cmd_PGMNAME()
{
  serial(S_ACK);
  serial(S_PGMNAME, sizeof(S_PGMNAME));
}

void cmd_SERBUF()
{
  serial(S_ACK);
  serial(64_w);
}

void cmd_BUSTYPE()
{
  serial(S_ACK);
  serial(8_b);
}

void cmd_SYNCNOP()
{
  serial(S_NAK);
  serial(S_ACK);
}

void cmd_SELBUS()
{
  if(byte(serial) == 8)serial(S_ACK);
  else serial(S_NAK);
}

void cmd_SPIOP()
{
  std::uint32_t slen = serial;
  std::uint32_t rlen = serial;

  SPI.beginTransaction(config);
  digitalWrite(52, LOW);

  while(slen--)SPI.transfer(serial);
  serial(S_ACK);
  while(rlen--)serial(SPI.transfer(0));

  digitalWrite(52, HIGH);
  SPI.endTransaction();
}

void cmd_SPI_FREQ()
{
  std::uint64_t freq = serial;
  freq = constrain(freq, 4e6, F_CPU);
  config = {unsigned(freq), MSBFIRST, SPI_MODE0};

  serial(S_ACK);
  serial(freq);
}

/*
 * Arduino entrypoints
 */

void setup()
{
  SerialUSB.begin(1e6);
  pinMode(52, OUTPUT);
  SPI.begin();

  CMD[0x00] = cmd_NOP;
  CMD[0x01] = cmd_IFACE;
  CMD[0x02] = cmd_CMDMAP;
  CMD[0x03] = cmd_PGMNAME;
  CMD[0x04] = cmd_SERBUF;
  CMD[0x05] = cmd_BUSTYPE;
  CMD[0x10] = cmd_SYNCNOP;
  CMD[0x12] = cmd_SELBUS;
  CMD[0x13] = cmd_SPIOP;
  CMD[0x14] = cmd_SPI_FREQ;
}

void loop()
{
  byte op = serial;
  if(CMD[op])CMD[op]();
  else serial(S_NAK);
}

