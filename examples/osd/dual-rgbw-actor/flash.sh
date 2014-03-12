#!/bin/bash
avrdude -pm128rfa1 -c arduino -P/dev/ttyUSB0 -b57600 -e -U flash:w:er-example-server.osd-merkur.hex:a -U eeprom:w:er-example-server.osd-merkur.eep:a
