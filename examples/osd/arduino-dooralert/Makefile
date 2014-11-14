# Set this to the name of your sketch (without extension .pde)
SKETCH=sketch

all: arduino-example \
     arduino-example.osd-merkur.hex arduino-example.osd-merkur.eep

# variable for this Makefile
# configure CoAP implementation (3|7|12|13) (er-coap-07 also supports CoAP draft 08)
WITH_COAP=13

# for some platforms
UIP_CONF_IPV6=1
# IPv6 make config disappeared completely
CFLAGS += -DUIP_CONF_IPV6=1

CONTIKI=../../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += resource_door.c ${SKETCH}.cpp

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

# REST framework, requires WITH_COAP
ifeq ($(WITH_COAP), 13)
${info INFO: compiling with CoAP-13}
CFLAGS += -DWITH_COAP=13
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-13
else ifeq ($(WITH_COAP), 12)
${info INFO: compiling with CoAP-12}
CFLAGS += -DWITH_COAP=12
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-12
else ifeq ($(WITH_COAP), 7)
${info INFO: compiling with CoAP-08}
CFLAGS += -DWITH_COAP=7
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-07
else ifeq ($(WITH_COAP), 3)
${info INFO: compiling with CoAP-03}
CFLAGS += -DWITH_COAP=3
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-03
else
${info INFO: compiling with HTTP}
CFLAGS += -DWITH_HTTP
CFLAGS += -DREST=http_rest_implementation
CFLAGS += -DUIP_CONF_TCP=1
APPS += er-http-engine
endif

APPS += erbium time json arduino json-resource

include $(CONTIKI)/Makefile.include
include $(CONTIKI)/apps/arduino/Makefile.include

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

.PHONY: flash

$(CONTIKI)/tools/tunslip6:      $(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:   $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0
