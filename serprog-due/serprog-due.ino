/*
 * Serprog for Arduino Due using hardware SPI
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

#include "serprog.hpp"

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

//Change serial port and CS pin number accordingly
serprog sp(SerialUSB, 52);

void setup()
{
  SerialUSB.begin(0);
  sp.setup();
}

void loop()
{
  sp.loop();
}
