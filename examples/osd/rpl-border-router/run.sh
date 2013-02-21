#!/bin/bash
make clean TARGET=osd-merkur
make TARGET=osd-merkur
avr-size -C --mcu=MCU=atmega128rfa1 border-router.osd-merkur
avr-objcopy -j .text -j .data -O ihex border-router.osd-merkur border-router.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex border-router.osd-merkur border-router.osd-merkur.eep
