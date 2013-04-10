/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
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
/**
 * \file
 *         border-router
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Nicolas Tsiftes <nvt@sics.se>
 *         Andreas Reder <andreas@reder.eu>
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"
#include "net/mac/framer-802154.h"

#include "net/netstack.h"
#include "dev/slip.h"
#include "cmd.h"
#include "border-router.h"
#include "border-router-cmds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

#include "erbium.h"
/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */



#define MAX_SENSORS 4

#define IPV6_ADDR_LEN	40
#define RPL_TABLE_SIZE 25000
char rpl_table[RPL_TABLE_SIZE];
int rpl_table_len;



uint16_t dag_id[] = {0x1111, 0x1100, 0, 0, 0, 0, 0, 0x0011};

extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_route_t uip_ds6_routing_table[];

extern long slip_sent;
extern long slip_received;

static uip_ipaddr_t prefix;
static uint8_t prefix_set;
static uint8_t mac_set;

static uint8_t sensor_count = 0;

/* allocate MAX_SENSORS char[32]'s */
static char sensors[MAX_SENSORS][32];

extern int contiki_argc;
extern char **contiki_argv;
extern const char *slip_config_ipaddr;

CMD_HANDLERS(border_router_cmd_handler);

PROCESS(border_router_process, "Border router process");
PROCESS(rest_server, "Erbium Coap Server");

AUTOSTART_PROCESSES(&border_router_process,&border_router_cmd_process, &rest_server);


static int create_ipv6_addr_str(const uip_ipaddr_t *addr, char *ptr){
  int len = 0;
  int i, f;
  uint16_t a;
        
  uip_debug_ipaddr_print(addr);
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0){
	  ptr[len++] = ':';
	  ptr[len++] = ':';
      }
    } else {
      if(f > 0) {
	f = -1;
      } else if(i > 0) {
	  ptr[len++] = ':';
      }
      len += snprintf(&ptr[len], IPV6_ADDR_LEN - len, "%x", a);
    }
  }  
  return len;
}

/*---------------------------------------------------------------------------*/
//create JSON RPL Table
static void 
generate_rpl_table(){
  char a[IPV6_ADDR_LEN];
  int addresses;
  static uip_ds6_route_t *r;
  int i;
  
  rpl_table_len = 0;
  // Start JSON 
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "{\n");
  
  // add title
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "\"title\": \"RPL Table\",\n");
  
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "\"Neighbors\": [ ");
  
  addresses = 0;
  for(i = 0; i < UIP_DS6_NBR_NB; i++) {
    if(uip_ds6_nbr_cache[i].isused) {
      addresses += 1;
      create_ipv6_addr_str(&uip_ds6_nbr_cache[i].ipaddr, a);
      rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "\"%s\", ", a);
    }
  }
  if(addresses > 0){
    //delete last coma
    rpl_table_len -= 2;
  }
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "],\n");
  
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "\"Routes\": [ ");
  
  addresses = 0;
  for(r = uip_ds6_route_list_head(); r != NULL; r = list_item_next(r)) {
    addresses += 1;
    create_ipv6_addr_str(&r->ipaddr, a);
    rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "\"%s via ", a);
    create_ipv6_addr_str(&r->nexthop, a);
    rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "%s\", ", a);
  }
  if(addresses > 0){
    //delete last coma
    rpl_table_len -= 2;
  }
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "]\n");
  
  // End JSON
  rpl_table_len += snprintf(&rpl_table[rpl_table_len], RPL_TABLE_SIZE - rpl_table_len, "}\n");
}

/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTA("Server IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINTA(" %p: =>", &uip_ds6_if.addr_list[i]);
      uip_debug_ipaddr_print(&(uip_ds6_if.addr_list[i]).ipaddr);
      PRINTA("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
request_mac(void)
{
  write_to_slip((uint8_t *)"?M", 2);
}
/*---------------------------------------------------------------------------*/
void
border_router_set_mac(const uint8_t *data)
{
  memcpy(uip_lladdr.addr, data, sizeof(uip_lladdr.addr));
  rimeaddr_set_node_addr((rimeaddr_t *)uip_lladdr.addr);

  /* is this ok - should instead remove all addresses and
     add them back again - a bit messy... ?*/
  uip_ds6_init();
  rpl_init();

  mac_set = 1;
}
/*---------------------------------------------------------------------------*/
void
border_router_print_stat()
{
  printf("bytes received over SLIP: %ld\n", slip_received);
  printf("bytes sent over SLIP: %ld\n", slip_sent);
}

/*---------------------------------------------------------------------------*/
/* Format: <name=value>;<name=value>;...;<name=value>*/
/* this function just cut at ; and store in the sensor array */
void
border_router_set_sensors(const char *data, int len)
{
  int i;
  int last_pos = 0;
  int sc = 0;
  for(i = 0;i < len; i++) {
    if(data[i] == ';') {
      sensors[sc][i - last_pos] = 0;
      memcpy(sensors[sc++], &data[last_pos], i - last_pos);
      last_pos = i + 1; /* skip the ';' */
    }
    if(sc == MAX_SENSORS) {
      sensor_count = sc;
      return;
    }
  }
  sensors[sc][len - last_pos] = 0;
  memcpy(sensors[sc++], &data[last_pos], len - last_pos);
  sensor_count = sc;
}
/*---------------------------------------------------------------------------*/
static void
set_prefix_64(const uip_ipaddr_t *prefix_64)
{
  uip_ipaddr_t ipaddr;
  memcpy(&prefix, prefix_64, 16);
  memcpy(&ipaddr, prefix_64, 16);

  prefix_set = 1;
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(border_router_process, ev, data)
{
  static struct etimer et;
  rpl_dag_t *dag;

  PROCESS_BEGIN();
  prefix_set = 0;

  PROCESS_PAUSE();

  PRINTF("RPL-Border router started\n");

  slip_config_handle_arguments(contiki_argc, contiki_argv);

  /* tun init is also responsible for setting up the SLIP connection */
  tun_init();

  while(!mac_set) {
    etimer_set(&et, CLOCK_SECOND);
    request_mac();
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }

  if(slip_config_ipaddr != NULL) {
    uip_ipaddr_t prefix;

    if(uiplib_ipaddrconv((const char *)slip_config_ipaddr, &prefix)) {
      PRINTF("Setting prefix ");
      PRINT6ADDR(&prefix);
      PRINTF("\n");
      set_prefix_64(&prefix);
    } else {
      PRINTF("Parse error: %s\n", slip_config_ipaddr);
      exit(0);
    }
  }

  dag = rpl_set_root(RPL_DEFAULT_INSTANCE,(uip_ip6addr_t *)dag_id);
  if(dag != NULL) {
    rpl_set_prefix(dag, &prefix, 64);
    PRINTF("created a new RPL dag\n");
  }

#if DEBUG
  print_local_addresses();
#endif

  /* The border router runs with a 100% duty cycle in order to ensure high
     packet reception rates. */
  NETSTACK_MAC.off(1);

  while(1) {
  //  etimer_set(&et, CLOCK_SECOND * 2);
 //   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    PROCESS_YIELD();
    /* do anything here??? */
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*
* Resources are defined by the RESOURCE macro.
* Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
*/
RESOURCE(rpl, METHOD_GET, "rpl", "title=\"rpl routing table\";rt=\"application/json\"");

/*
* A handler function named [resource name]_handler must be implemented for each RESOURCE.
* A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
* preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
* If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
*/

void
rpl_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int32_t strpos = 0;
  int i;

  if(*offset == 0){
      generate_rpl_table();
  }
  
  /* Check the offset for boundaries of the resource data. */
  if (*offset>=rpl_table_len)
  {
    REST.set_response_status(response, REST.status.BAD_OPTION);
    /* A block error message should not exceed the minimum block size (16). */

    const char *error_msg = "BlockOutOfScope";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  /* Generate data until reaching CHUNKS_TOTAL. */
  while (strpos<preferred_size)
  {
	  for( i = 0; i < (preferred_size-strpos+1); i++){
      buffer[strpos] = rpl_table[strpos + *offset];
      strpos += 1;
    }
  }

  /* snprintf() does not adjust return value if truncated by size. */
  if (strpos > preferred_size)
  {
    strpos = preferred_size;
  }

  /* Truncate if above CHUNKS_TOTAL bytes. */
  if (*offset+(int32_t)strpos > rpl_table_len)
  {
    strpos = rpl_table_len - *offset;
  }

  REST.set_response_payload(response, buffer, strpos);

  /* IMPORTANT for chunk-wise resources: Signal chunk awareness to REST engine. */
  *offset += strpos;

  /* Signal end of resource representation. */
  if (*offset>=rpl_table_len)
  {
    *offset = -1;
  }
}


RESOURCE(info, METHOD_GET, "info", "title=\"Info\";rt=\"application/json\"");

/*
* A handler function named [resource name]_handler must be implemented for each RESOURCE.
* A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
* preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
* If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
*/
void
info_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /* |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
     index += sprintf(message + index,"{\n \"version\" : \"V0.2\",\n");
     index += sprintf(message + index," \"name\" : \"native coap border router\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}


RESOURCE(network, METHOD_GET | METHOD_PUT, "network", "title=\"osd configs\"; rt=\"application/json\"");
void
network_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  uint8_t buf[10];
  int index = 0;
  int length = 0; /* |<-------->| */
  const char* strg;
  uint16_t panid;
  
  if((length = REST.get_post_variable(request, "panid", &strg))){
	// parse panid
    panid = (uint16_t)atoi(strg);
    // set framer panid
    framer_802154_set_panid(panid);
    //set radio panid
    buf[0] = '?';
    buf[1] = 'P';
    buf[2] = panid >> 8;
    buf[3] = panid;
    write_to_slip(buf, 4);

  }else if((length = REST.get_post_variable(request, "channel", &strg))){
    REST.set_response_status(response, REST.status.BAD_REQUEST);
    return;
  }
  
  index += sprintf(message + index, "{\n");
  index += sprintf(message + index, "\"panid\": \"%u\",\n", framer_802154_get_panid());
  //index += sprintf(message + index, "\"channel\": \"%u\",\n", //params_get_channel());
  index += sprintf(message + index, "}\n");

  length = strlen(message);
  memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}


/*---------------------------------------------------------------------------*/

PROCESS_THREAD(rest_server, ev, data)
{

  PROCESS_BEGIN();



  PRINTF("Starting Erbium Coap Server\n");

#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
  rest_activate_resource(&resource_rpl);
  rest_activate_resource(&resource_network);
  rest_activate_resource(&resource_info);

  while(1) {
    PROCESS_YIELD();
  }


  PROCESS_END();
}



