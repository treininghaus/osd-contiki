#!/bin/bash
make clean TARGET=osd-merkur
make TARGET=osd-merkur
avr-size -C --mcu=MCU=atmega128rfa1 er-example-client.osd-merkur
avr-objcopy -j .text -j .data -O ihex er-example-client.osd-merkur er-example-client.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex er-example-client.osd-merkur er-example-client.osd-merkur.eep
