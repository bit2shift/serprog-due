# Serial Flasher Protocol Specification v1 for Arduino Due
This implementation ~~uses~~ used software SPI (bit banging).  
It uses hardware SPI (`C̅S̅ → pin 52`) through the ICSP header since commit 2f98c47.

### Why bit banging?
I needed to flash my laptop's BIOS and the Arduino Due was the only one I had running at 3.3V.  
Yet I lacked any female jumper cables, since it has the SPI pins on the ICSP header.

The full v1 protocol specification is detailed in [`serprog-protocol.txt`](serprog-protocol.txt).

Use this with [flashrom](https://www.flashrom.org/Downloads), following [these instructions](https://www.flashrom.org/Serprog/Arduino_flasher#Running_flashrom).

<details>
  <summary>Pictures</summary>
  <img width="50%" src="overview.jpg">
  <img width="50%" src="clip.jpg">
</details>
