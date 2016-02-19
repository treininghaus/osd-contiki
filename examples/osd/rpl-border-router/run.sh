#!/bin/bash
# For the ages-old bootloader (before 2014) you want to use
# BOOTLOADER_GET_MAC=0x0001f3a0 as parameter to make below.
make clean TARGET=osd-merkur
make TARGET=osd-merkur
avr-size -C --mcu=MCU=atmega128rfa1 border-router.osd-merkur
avr-objcopy -j .text -j .data -O ihex border-router.osd-merkur border-router.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex border-router.osd-merkur border-router.osd-merkur.eep
