ifeq ($(TARGET), osd-merkur)
PLATFORM_FILES= avr-size er-example-server.osd-merkur.hex \
    er-example-server.osd-merkur.eep
endif

all: er-example-server $(PLATFORM_FILES)
# use this target explicitly if requried: er-plugtest-server

CONTIKI=../../..

# Contiki IPv6 configuration
CONTIKI_WITH_IPV6 = 1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += resource_led_pwm.c

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
APPS += json json-resource

include $(CONTIKI)/Makefile.include

avr-size: er-example-server.osd-merkur
	avr-size -C --mcu=MCU=atmega128rfa1 er-example-server.osd-merkur

er-example-server.osd-merkur.hex: er-example-server.osd-merkur
	avr-objcopy -j .text -j .data -O ihex er-example-server.osd-merkur \
	    er-example-server.osd-merkur.hex

er-example-server.osd-merkur.eep: er-example-server.osd-merkur
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 -O ihex                      \
	    er-example-server.osd-merkur er-example-server.osd-merkur.eep

flash: er-example-server.osd-merkur.hex er-example-server.osd-merkur.eep
	avrdude -pm128rfa1 -c arduino -P/dev/ttyUSB0 -b57600 -e -U \
	    flash:w:er-example-server.osd-merkur.hex:a -U \
	    eeprom:w:er-example-server.osd-merkur.eep:a

.PHONY: flash avr-size

$(CONTIKI)/tools/tunslip6:      $(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router: $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:   $(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0
