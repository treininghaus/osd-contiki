ifeq ($(TARGET), osd-merkur)
PLATFORM_FILES= avr-size wallclock.osd-merkur.hex \
    wallclock.osd-merkur.eep
endif
ifeq ($(TARGET), pico-rv32-icoboard)
PLATFORM_FILES= appimage.hex
endif

ifeq ($(ICOTOOLS),)
    ICOTOOLS=/usr/local/src/unpacked/icotools
endif

all: wallclock.$(TARGET) $(PLATFORM_FILES)
# use target "er-plugtest-server" explicitly when requried 

CONTIKI=../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECTDIRS += $(REST_RESOURCES_DIR) $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine
APPS += json json-resource time

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include

appimage.hex: wallclock.pico-rv32-icoboard
	riscv32-unknown-elf-objcopy -O verilog wallclock.pico-rv32-icoboard \
	    appimage.hex

icosoc.mk: icosoc.cfg
	$(ICOTOOLS)/icosoc/icosoc.py -c

include icosoc.mk
CLEAN += appimage.hex wallclock.$(TARGET)
-include *.d
