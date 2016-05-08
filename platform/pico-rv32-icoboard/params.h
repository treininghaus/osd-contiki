#ifndef PARAMS_H_
#define PARAMS_H_

/* Include settings.h, then dummy out the write routines */
#include "settings.h"
#if PARAMETER_STORAGE==2
#define settings_add(...) 0
#define settings_add_uint8(...) 0
#define settings_add_uint16(...) 0
#endif

#define CHANNEL_802_15_4          25     // default frequency (11-26)

#ifdef SERVER_NAME
#define PARAMS_SERVERNAME SERVER_NAME
#else
#define PARAMS_SERVERNAME "PicoRV32"
#endif
#ifdef DOMAIN_NAME
#define PARAMS_DOMAINNAME DOMAIN_NAME
#else
#define PARAMS_DOMAINNAME "localhost"
#endif
#ifdef NODE_ID
#define PARAMS_NODEID NODE_ID
#else
#define PARAMS_NODEID 0
#endif
#ifdef CHANNEL_802_15_4
#define PARAMS_CHANNEL CHANNEL_802_15_4
#else
#define PARAMS_CHANNEL 26
#endif
#ifdef IEEE802154_PANID
#define PARAMS_PANID IEEE802154_PANID
#else
#define PARAMS_PANID 0xABCD
#endif
#ifdef IEEE802154_PANADDR
#define PARAMS_PANADDR IEEE802154_PANADDR
#else
#define PARAMS_PANADDR 0
#endif
#ifdef RF230_MAX_TX_POWER
#define PARAMS_TXPOWER RF230_MAX_TX_POWER
#else
#define PARAMS_TXPOWER 0
#endif
#ifdef EUI64_ADDRESS
#define PARAMS_EUI64ADDR EUI64_ADDRESS
#else
/* This form of of EUI64 mac allows full 6LoWPAN header compression from mac address */
#if UIP_CONF_LL_802154
#define PARAMS_EUI64ADDR {0x02, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01}
#else
#define PARAMS_EUI64ADDR {0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01}
#endif
/* This form of of EUI64 mac allows 16 bit 6LoWPAN header compression on multihops */
#endif

uint8_t params_get_eui64(uint8_t *eui64);
/* Hard coded program flash parameters */
#define params_get_servername(...) 
#define params_get_nodeid(...) PARAMS_NODEID
#define params_get_channel(...) PARAMS_CHANNEL
#define params_get_panid(...) PARAMS_PANID
#define params_get_panaddr(...) PARAMS_PANADDR
#define params_get_txpower(...) PARAMS_TXPOWER

#endif /* PARAMS_H_ */
