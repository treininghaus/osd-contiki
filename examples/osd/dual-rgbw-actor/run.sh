#!/bin/bash
#make clean TARGET=osd-merkur
make -j TARGET=osd-merkur
#avr-size -C --mcu=MCU=atmega128rfa1 er-example-server.osd-merkur
avr-objcopy -j .text -j .data -O ihex er-example-server.osd-merkur er-example-server.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex er-example-server.osd-merkur er-example-server.osd-merkur.eep
