#!/bin/bash
# For the new bootloader (using a jump-table) you want to use
# BOOTLOADER_GET_MAC=0x0001ff80 (which is the current default)
make clean TARGET=osd-merkur
# for Toms Board 34ab
make TARGET=osd-merkur BOOTLOADER_GET_MAC=0x0001f3a0
# make TARGET=osd-merkur

# für das neu Board Mac 00:21:2e:ff:ff:00:66:5b
# scheint es der alte Bootloader zu sein, da ich sonst nicht flashen kann
# make TARGET=osd-merkur BOOTLOADER_GET_MAC=0x0001ff80
avr-size -C --mcu=MCU=atmega128rfa1 er-example-server.osd-merkur
avr-objcopy -j .text -j .data -O ihex er-example-server.osd-merkur er-example-server.osd-merkur.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex er-example-server.osd-merkur er-example-server.osd-merkur.eep
