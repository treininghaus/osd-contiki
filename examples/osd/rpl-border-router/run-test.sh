#!/bin/bash
make clean TARGET=atmega128rfa1
make TARGET=atmega128rfa1
avr-size -C --mcu=MCU=atmega128rfa1 rest-server-example.osd-er-lp24
avr-objcopy -j .text -j .data -O ihex border-router.osd-er-lp24 border-router.osd-er-lp24.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex border-router.osd-er-lp24 border-router.osd-er-lp24.eep
#sudo avrdude -pm128rfa1 -c arduino -P/dev/ttyUSB0 -b57600 -e -U flash:w:border-router.osd-er-lp24.hex:a -U eeprom:w:border-router.osd-er-lp24.eep:a