#!/bin/bash
make clean TARGET=osd-er-lp24
make TARGET=osd-er-lp24
avr-size -C --mcu=MCU=atmega128rfa1 er-example-server.osd-er-lp24
avr-objcopy -j .text -j .data -O ihex er-example-server.osd-er-lp24 er-example-server.osd-er-lp24.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex er-example-server.osd-er-lp24 er-example-server.osd-er-lp24.eep

