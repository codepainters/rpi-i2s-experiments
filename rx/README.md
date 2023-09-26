# RX code

This directory contains a simple C code that configures I2S receiver,
then polls RX FIFO for new data and prints it as 32-bit hex numbers
to `stdout`. No DMA, no interrupts.

It is supposed to be compiled on Raspberry with `make`.

It uses raw register access. To use it you have to unload `bcm2835-i2s` 
module.

Configuration of the I2S:

* slave mode (`BCLK` and `LRCK` as inputs)
* frame length - 64 bits

  * divided into 2 slots of 32 bits (starting at bit 1 and 33, respectively)




See:

* [BCM2711 ARM Peripherals](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)
