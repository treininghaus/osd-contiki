#!/bin/bash
sudo avrdude -pm128rfa1 -c arduino -P/dev/ttyUSB0 -b57600 -e -U flash:w:embedd-vm-server.osd-merkur.hex:a -U eeprom:w:embedd-vm-server.osd-merkur.eep:a
