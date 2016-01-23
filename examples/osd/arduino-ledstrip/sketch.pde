/*
 * Sample arduino sketch using contiki features.
 * We turn the LED off 
 * We allow read the moisture sensor
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

#include "RGBdriver.h"

#define CLK 2//pins definitions for the driver        
#define DIO 3
RGBdriver Driver(CLK,DIO);

extern "C" {
#include "rest-engine.h"
#include "net/netstack.h"

extern volatile uint8_t mcusleepcycle;  // default 16
extern resource_t res_door, res_battery;
uint8_t door_pin = 3;
uint8_t door_status = 0;

#define LED_PIN 4
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // init coap resourcen
    rest_init_engine ();
    rest_activate_resource (&res_door, "s/door");
    rest_activate_resource (&res_battery, "s/battery");
    
    NETSTACK_MAC.off(1);
}

void loop (void)
{
  static int a=1;
 
  mcusleepcycle=0;

  switch(a) {
	case 1: printf("a ist eins\n");
	  Driver.begin(); // begin
      Driver.SetColor(255, 0, 0); //Red. first node data
      Driver.end();
      a++;
	 break;
	case 2: printf("a ist zwei\n");
      Driver.begin(); // begin
      Driver.SetColor(0, 255, 0); //Green. first node data
      Driver.end();
      a++;
	 break;
	case 3: printf("a ist drei\n");
      Driver.begin(); // begin
      Driver.SetColor(0, 0, 255);//Blue. first node data
      Driver.end();
      a=1;
	 break;
	default: printf("a ist irgendwas\n"); break;
  }
  mcusleepcycle=16;
}
