#!/bin/bash
# For the ages-old bootloader (before 2014) you want to use
# BOOTLOADER_GET_MAC=0x0001f3a0 as parameter to make below.
make clean TARGET=osd-merkur
make TARGET=osd-merkur
avr-size slip-radio.osd-merkur
avr-objcopy -j .text -j .data -O ihex slip-radio.osd-merkur slip-radio.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex slip-radio.osd-merkur slip-radio.osd-merkur.eep
