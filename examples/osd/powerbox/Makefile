SERIAL=/dev/ttyUSB0
ifeq ($(TARGET), osd-merkur)
PLATFORM_FILES= avr-size powerbox.osd-merkur.hex \
    powerbox.osd-merkur.eep
endif

all: powerbox $(PLATFORM_FILES)

CONTIKI=../../..

# Contiki IPv6 configuration
WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6_RPL=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# automatically build RESTful resources
REST_RESOURCES_DIR = ./resources
REST_RESOURCES_DIR_COMMON = ../resources-common
REST_RESOURCES_FILES= $(notdir \
    $(shell find $(REST_RESOURCES_DIR_COMMON) -name '*.c') \
    )

PROJECTDIRS += $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
APPS += json json-resource

# optional rules to get assembly
#CUSTOM_RULE_C_TO_OBJECTDIR_O = 1
#CUSTOM_RULE_S_TO_OBJECTDIR_O = 1

include $(CONTIKI)/Makefile.include

# minimal-net target is currently broken in Contiki
ifeq ($(TARGET), minimal-net)
CFLAGS += -DHARD_CODED_ADDRESS=\"fdfd::10\"
${info INFO: compiling with large buffers}
CFLAGS += -DUIP_CONF_BUFFER_SIZE=1300
CFLAGS += -DREST_MAX_CHUNK_SIZE=1024
CFLAGS += -DCOAP_MAX_HEADER_SIZE=176
CFLAGS += -DUIP_CONF_IPV6_RPL=0
endif

# optional rules to get assembly
#$(OBJECTDIR)/%.o: asmdir/%.S
#	$(CC) $(CFLAGS) -MMD -c $< -o $@
#	@$(FINALIZE_DEPENDENCY)
#
#asmdir/%.S: %.c
#	$(CC) $(CFLAGS) -MMD -S $< -o $@

# border router rules
$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 -p 60001 aaaa::1/64

connect-router-native:	$(CONTIKI)/examples/ipv6/native-border-router/border-router.native
	sudo $(CONTIKI)/exmples/ipv6/native-border-router/border-router.native -a 127.0.0.1 -p 60001 aaaa::1/64

connect-minimal:
	sudo ip address add fdfd::1/64 dev tap0 

avr-size: powerbox.osd-merkur
	avr-size -C --mcu=MCU=atmega128rfa1 powerbox.osd-merkur

powerbox.osd-merkur.hex: powerbox.osd-merkur
	avr-objcopy -j .text -j .data -O ihex powerbox.osd-merkur \
	    powerbox.osd-merkur.hex

powerbox.osd-merkur.eep: powerbox.osd-merkur
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 -O ihex                      \
	    powerbox.osd-merkur powerbox.osd-merkur.eep

flash: powerbox.osd-merkur.hex powerbox.osd-merkur.eep
	avrdude -pm128rfa1 -c arduino -P$(SERIAL) -b57600 -e -U \
	    flash:w:powerbox.osd-merkur.hex:a -U \
	    eeprom:w:powerbox.osd-merkur.eep:a

.PHONY: flash avr-size
