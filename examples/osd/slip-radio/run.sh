#!/bin/bash
make clean TARGET=osd-merkur
make TARGET=osd-merkur
avr-size slip-radio.osd-merkur
avr-objcopy -j .text -j .data -O ihex slip-radio.osd-merkur slip-radio.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex slip-radio.osd-merkur slip-radio.osd-merkur.eep
