/*
 * Copyright (c) 2016, Dr. Ralf Schlatterbeck Open Source Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
#define printf_P printf
#define PSTR(x) (x)

#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define ANNOUNCE_BOOT 1
#if ANNOUNCE_BOOT
#define PRINTA(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTA(...)
#endif

#define DEBUG 0
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
#endif

#include <stdio.h>
#include <string.h>

#include "params.h"
//#include "radio/rf230bb/rf230bb.h"
#include "net/mac/frame802154.h"
#include "net/mac/framer-802154.h"
#include "net/ipv6/sicslowpan.h"

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

//#include "dev/rs232.h"
//#include "dev/serial-line.h"
//#include "dev/slip.h"

#include "net/rime/rime.h"

/* Track interrupt flow through mac, rdc and radio driver */
//#define DEBUGFLOWSIZE 32
#if DEBUGFLOWSIZE
uint8_t debugflowsize,debugflow[DEBUGFLOWSIZE];
#define DEBUGFLOW(c) if (debugflowsize<(DEBUGFLOWSIZE-1)) debugflow[debugflowsize++]=c
#else
#define DEBUGFLOW(c)
#endif

/* Get periodic prints from idle loop, from clock seconds or rtimer interrupts */
/* Use of rtimer will conflict with other rtimer interrupts such as contikimac radio cycling */
/* STAMPS will print ENERGEST outputs if that is enabled. */
#define PERIODICPRINTS 1
#if PERIODICPRINTS
//#define PINGS 64
#define ROUTES 600
#define STAMPS 60
#define STACKMONITOR 1024
uint32_t clocktime;
#define TESTRTIMER 0
#if TESTRTIMER
uint8_t rtimerflag=1;
struct rtimer rt;
void rtimercycle(void) {rtimerflag=1;}
#endif
#endif

uint16_t ledtimer;

/*-------------------------Low level initialization------------------------*/
/*------Done in a subroutine to keep main routine stack usage small--------*/
void initialize(void)
{
#if 0 && STACKMONITOR
  /* Simple stack pointer highwater monitor. Checks for magic numbers in the main
   * loop. In conjuction with PERIODICPRINTS, never-used stack will be printed
   * every STACKMONITOR seconds.
   */
{
extern uint16_t __bss_end;
uint16_t *p=&__bss_end;
    do {
      *p = 0x4242;
      p+=5;
    } while (p<SP-10); //don't overwrite our own stack
}
#endif
  clock_init();

  PRINTA("\n*******Booting %s*******\n",CONTIKI_VERSION_STRING);

/* rtimers needed for radio cycling */
  rtimer_init();

 /* Initialize process subsystem */
  process_init();

  /* etimers must be started before ctimer_init */
  process_start(&etimer_process, NULL);
  ctimer_init();

  /* Start radio and radio receive process */
  NETSTACK_RADIO.init();

/* Get a random seed for the 802.15.4 packet sequence number.
 * Some layers will ignore duplicates found in a history (e.g. Contikimac)
 * causing the initial packets to be ignored after a short-cycle restart.
 */
  //random_init(rng_get_uint8());

  /* Set addresses BEFORE starting tcpip process */

  linkaddr_t addr;

  if (params_get_eui64(addr.u8)) {
      PRINTA("Random EUI64 address generated\n");
  }
 
#if NETSTACK_CONF_WITH_IPV6 
  memcpy(&uip_lladdr.addr, &addr.u8, sizeof(linkaddr_t));
#elif WITH_NODE_ID
  node_id=get_panaddr_from_eeprom();
  addr.u8[1]=node_id&0xff;
  addr.u8[0]=(node_id&0xff00)>>8;
  PRINTA("Node ID from eeprom: %X\n",node_id);
#endif  
  linkaddr_set_node_addr(&addr); 

  // FIXME undefined
  //rf230_set_pan_addr(params_get_panid(),params_get_panaddr(),(uint8_t *)&addr.u8);
  //rf230_set_channel(params_get_channel());
  //rf230_set_txpower(params_get_txpower());

#if NETSTACK_CONF_WITH_IPV6
  PRINTA("EUI-64 MAC: %x-%x-%x-%x-%x-%x-%x-%x\n",addr.u8[0],addr.u8[1],addr.u8[2],addr.u8[3],addr.u8[4],addr.u8[5],addr.u8[6],addr.u8[7]);
#else
  PRINTA("MAC address ");
  uint8_t i;
  for (i=sizeof(linkaddr_t); i>0; i--){
    PRINTA("%x:",addr.u8[i-1]);
  }
  PRINTA("\n");
#endif

  /* Initialize stack protocols */
  queuebuf_init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();

#if ANNOUNCE_BOOT
  //FIXME: undefined
  //PRINTA("%s %s, channel %u , check rate %u Hz tx power %u\n",NETSTACK_MAC.name, NETSTACK_RDC.name, rf230_get_channel(),
  //  CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:NETSTACK_RDC.channel_check_interval()),
  //  rf230_get_txpower());	  
#if UIP_CONF_IPV6_RPL
  PRINTA("RPL Enabled\n");
#endif
#if UIP_CONF_ROUTER
  PRINTA("Routing Enabled\n");
#endif

#endif /* ANNOUNCE_BOOT */

#if NETSTACK_CONF_WITH_IPV6 || NETSTACK_CONF_WITH_IPV4
  process_start(&tcpip_process, NULL);
#endif

#ifdef RAVEN_LCD_INTERFACE
  process_start(&raven_lcd_process, NULL);
#endif

  /* Autostart other processes */
  autostart_start(autostart_processes);

/* Add addresses for testing */
#if 0
{  
  uip_ip6addr_t ipaddr;
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
//  uip_ds6_prefix_add(&ipaddr,64,0);
}
#endif

/*--------------------------Announce the configuration---------------------*/
#if ANNOUNCE_BOOT
   PRINTA("Online\n");
#endif /* ANNOUNCE_BOOT */

}

#if ROUTES && NETSTACK_CONF_WITH_IPV6
static void
ipaddr_add(const uip_ipaddr_t *addr)
{
  uint16_t a;
  int8_t i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) PRINTF("::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        PRINTF(":");
      }
      PRINTF("%x",a);
    }
  }
}
#endif

/*-------------------------------------------------------------------------*/
/*------------------------- Main Scheduler loop----------------------------*/
/*-------------------------------------------------------------------------*/
int
main(void)
{
  puts ("Hello world!\n");
  printf ("Hello world!\n");
#if NETSTACK_CONF_WITH_IPV6
  uip_ds6_nbr_t *nbr;
#endif /* NETSTACK_CONF_WITH_IPV6 */
  initialize();

  while(1) {
    process_run();

#if PERIODICPRINTS
#if TESTRTIMER
/* Timeout can be increased up to 8 seconds maximum.
 * A one second cycle is convenient for triggering the various debug printouts.
 * The triggers are staggered to avoid printing everything at once.
 */
    if (rtimerflag) {
      rtimer_set(&rt, RTIMER_NOW()+ RTIMER_ARCH_SECOND*1UL, 1,(void *) rtimercycle, NULL);
      rtimerflag=0;
#else
  if (clocktime!=clock_seconds()) {
     clocktime=clock_seconds();
#endif

#if STAMPS
if ((clocktime%STAMPS)==0) {
  PRINTF("%lus\n",clocktime);
}
#endif

#if ROUTES && NETSTACK_CONF_WITH_IPV6
if ((clocktime%ROUTES)==2) {
      
extern uip_ds6_netif_t uip_ds6_if;

  uint8_t i,j;
  PRINTF("\nAddresses [%u max]\n",UIP_DS6_ADDR_NB);
  for (i=0;i<UIP_DS6_ADDR_NB;i++) {
    if (uip_ds6_if.addr_list[i].isused) {
      ipaddr_add(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
  PRINTF("\nNeighbors [%u max]\n",NBR_TABLE_MAX_NEIGHBORS);

  for(nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      nbr = nbr_table_next(ds6_neighbors, nbr)) {
    ipaddr_add(&nbr->ipaddr);
    PRINTF("\n");
    j=0;
  }
  if (j) PRINTF("  <none>");
  PRINTF("\nRoutes [%u max]\n",UIP_DS6_ROUTE_NB);
  {
    uip_ds6_route_t *r;
    PRINTF("\nRoutes [%u max]\n",UIP_DS6_ROUTE_NB);
    j = 1;
    for(r = uip_ds6_route_head();
        r != NULL;
        r = uip_ds6_route_next(r)) {
      ipaddr_add(&r->ipaddr);
      PRINTF("/%u (via ", r->length);
      ipaddr_add(uip_ds6_route_nexthop(r));
      PRINTF(") %lus\n", r->state.lifetime);
      j = 0;
    }
  }
  if (j) PRINTF("  <none>");
  PRINTF("\n---------\n");
}
#endif

#if 0 && STACKMONITOR
if ((clocktime%STACKMONITOR)==3) {
  extern uint16_t __bss_end;
  uint16_t *p=&__bss_end;
  do {
    if (*p != 0x4242) {
      PRINTF("Never-used stack > %d bytes\n",((char*)p) - ((char*)&__bss_end));
      break;
    }
    p+=5;
  } while (p<RAMEND-10);
}
#endif

    }
#endif /* PERIODICPRINTS */

#if 0
#if RF230BB&&0
extern uint8_t rf230processflag;
    if (rf230processflag) {
      PRINTF("rf230p%d",rf230processflag);
      rf230processflag=0;
    }
#endif

#if RF230BB&&0
extern uint8_t rf230_interrupt_flag;
    if (rf230_interrupt_flag) {
 //   if (rf230_interrupt_flag!=11) {
        PRINTF("**RI%u",rf230_interrupt_flag);
 //   }
      rf230_interrupt_flag=0;
    }
#endif
#endif /* 0 */
  }
  return 0;
}

/*---------------------------------------------------------------------------*/

void log_message(char *m1, char *m2)
{
  PRINTF("%s%s\n", m1, m2);
}
