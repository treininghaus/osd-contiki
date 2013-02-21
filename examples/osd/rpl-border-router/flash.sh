#!/bin/bash
sudo avrdude -pm128rfa1 -c arduino -P/dev/ttyUSB0 -b57600 -e -U flash:w:border-router.osd-merkur.hex:a -U eeprom:w:border-router.osd-merkur.eep:a
