#!/bin/bash
# For the new bootloader (using a jump-table) you want to use
# BOOTLOADER_GET_MAC=0x0001ff80 (which is the current default)
#make clean TARGET=osd-merkur
make -j TARGET=osd-merkur BOOTLOADER_GET_MAC=0x0001f3a0
#avr-size -C --mcu=MCU=atmega128rfa1 er-example-server.osd-merkur
avr-objcopy -j .text -j .data -O ihex er-example-server.osd-merkur er-example-server.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex er-example-server.osd-merkur er-example-server.osd-merkur.eep
