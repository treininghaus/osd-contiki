# Set this to the name of your sketch (without extension .pde)
SKETCH=sketch

ifeq ($(TARGET), osd-merkur)
PLATFORM_FILES= avr-size arduino-example.osd-merkur.hex \
    arduino-example.osd-merkur.eep
endif

all: arduino-example $(PLATFORM_FILES)

CONTIKI=../../..

# Contiki IPv6 configuration
CONTIKI_WITH_IPV6 = 1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += ${SKETCH}.cpp RGBdriver.cpp

# automatically build RESTful resources
REST_RESOURCES_DIR = ./resources
REST_RESOURCES_DIR_COMMON = ../resources-common
REST_RESOURCES_FILES= $(notdir \
    $(shell find $(REST_RESOURCES_DIR) -name '*.c') \
    $(shell find $(REST_RESOURCES_DIR_COMMON) -name '*.c') \
    )

PROJECTDIRS += $(REST_RESOURCES_DIR) $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# variable for Makefile.include
ifneq ($(TARGET), minimal-net)
CFLAGS += -DUIP_CONF_IPV6_RPL=1
else
# minimal-net does not support RPL under Linux and is mostly used to test CoAP only
${info INFO: compiling without RPL}
CFLAGS += -DUIP_CONF_IPV6_RPL=0
CFLAGS += -DHARD_CODED_ADDRESS=\"fdfd::10\"
${info INFO: compiling with large buffers}
CFLAGS += -DUIP_CONF_BUFFER_SIZE=2048
CFLAGS += -DREST_MAX_CHUNK_SIZE=1024
CFLAGS += -DCOAP_MAX_HEADER_SIZE=640
endif

# linker optimizations
SMALL=1


# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
APPS += arduino
APPS += json json-resource

include $(CONTIKI)/Makefile.include
include $(CONTIKI)/apps/arduino/Makefile.include

avr-size: arduino-example.osd-merkur
	avr-size -C --mcu=MCU=atmega128rfa1 arduino-example.osd-merkur

arduino-example.osd-merkur.hex: arduino-example.osd-merkur
	avr-objcopy -j .text -j .data -O ihex arduino-example.osd-merkur \
	    arduino-example.osd-merkur.hex

arduino-example.osd-merkur.eep: arduino-example.osd-merkur
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 -O ihex                      \
	    arduino-example.osd-merkur arduino-example.osd-merkur.eep

flash: arduino-example.osd-merkur.hex arduino-example.osd-merkur.eep
	avrdude -pm128rfa1 -c arduino -P/dev/ttyUSB0 -b57600 -e -U \
	    flash:w:arduino-example.osd-merkur.hex:a -U \
	    eeprom:w:arduino-example.osd-merkur.eep:a

.PHONY: flash avr-size

$(CONTIKI)/tools/tunslip6:      $(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:   $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0
	
