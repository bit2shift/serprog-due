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
 *
 * Frequency: 8MHz
 * Bit-order: MSB
 */

class Shifter
{
  const byte CS = 4;
  const byte MISO = 5;
  const byte MOSI = 6;
  const byte SCLK = 7;
  
  const byte tclk = 1;
  
public:
  void init()
  {
    pinMode(CS, OUTPUT);
    pinMode(MISO, INPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(SCLK, OUTPUT);
    
    digitalWrite(CS, HIGH);
    digitalWrite(MOSI, LOW);
    digitalWrite(SCLK, LOW);
  }
  
  void begin()
  {
    digitalWrite(CS, LOW);
  }
  
  void end()
  {
    digitalWrite(CS, HIGH);
  }
  
  byte exchange(byte out = 0)
  {
    byte in = 0;
    for(byte b = 0x80; b; b >>= 1)
    {
      digitalWrite(MOSI, (out & b));
      delayMicroseconds(tclk);
      digitalWrite(SCLK, HIGH);
      if(digitalRead(MISO))in |= b;
      delayMicroseconds(tclk);
      digitalWrite(SCLK, LOW);
    }
    return in;
  }
};

//blocking wrapper
byte get_chr()
{
  while(!Serial.available())asm volatile("NOP");
  return Serial.read();
}

long get_num()
{
  byte ptr[4] = {get_chr(), get_chr(), get_chr(), 0};
  long num = *(long*)ptr;
  return num;
}

//tweak wrapper
void put_chr(const byte chr)
{
  Serial.write(chr);
}

void put_str(const byte* str, const byte len)
{
  Serial.write(str, len);
}

//programmer globals
Shifter spi;
void (*cmd[256])() = {0};

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
 */

const byte S_ACK = 0x06;
const byte S_NAK = 0x15;
const byte S_PGMNAME[16] = "serprog-due";

void cmd_NOP()
{
  put_chr(S_ACK);
}

void cmd_IFACE()
{
  put_chr(S_ACK);
  put_chr(0x01);
  put_chr(0x00);
}

void cmd_CMDMAP()
{
  put_chr(S_ACK);
  for(int i = 0; i < 256; i++)
  {
    static byte flag = 0;
    if(cmd[i])flag |= bit(i & 7);
    if((i & 7) == 7)
    {
      put_chr(flag);
      flag = 0;
    }
  }
}

void cmd_PGMNAME()
{
  put_chr(S_ACK);
  put_str(S_PGMNAME, sizeof(S_PGMNAME));
}

void cmd_SERBUF()
{
  put_chr(S_ACK);
  put_chr(0x40);
  put_chr(0x00);
}

void cmd_BUSTYPE()
{
  put_chr(S_ACK);
  put_chr(0x08);
}

void cmd_SYNCNOP()
{
  put_chr(S_NAK);
  put_chr(S_ACK);
}

void cmd_SELBUS()
{
  if(get_chr() == 0x08)put_chr(S_ACK);
  else put_chr(S_NAK);
}

void cmd_SPIOP()
{
  long slen = get_num();
  long rlen = get_num();
  
  spi.begin();
  while(slen--)spi.exchange(get_chr());
  put_chr(S_ACK);
  while(rlen--)put_chr(spi.exchange());
  spi.end();
}

/*
 * Arduino entrypoints
 */

void setup()
{
  Serial.begin(115200);
  spi.init();
  
  cmd[0x00] = cmd_NOP;
  cmd[0x01] = cmd_IFACE;
  cmd[0x02] = cmd_CMDMAP;
  cmd[0x03] = cmd_PGMNAME;
  cmd[0x04] = cmd_SERBUF;
  cmd[0x05] = cmd_BUSTYPE;
  cmd[0x10] = cmd_SYNCNOP;
  cmd[0x12] = cmd_SELBUS;
  cmd[0x13] = cmd_SPIOP;
}

void loop()
{
  byte op = get_chr();
  if(cmd[op])cmd[op]();
  else put_chr(S_NAK);
}
