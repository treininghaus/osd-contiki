A Quick Introduction to the IoT Austria Irrigation Firmware
for the OSD-Merkur Board Platform
based on Erbium (Er) REST Engine

===================================================

Compile the Example
-------------------

    ./run.sh

OSD-Merkur Board
----------------------

write the images to the OSD-Merkur Board:

    ./flash.sh

Used Hardware
----------------------

- OSD-Merkur Board on top of
- OSD-Merkur Grove Shield on top of
- OSD-Merkur Power Board (relay board)

for touch button pin see

    platform/osd-merkur/dev/button-sensor.c

	#define BUTTON_BIT INTF4

German Project Page
----------------------

[http://wiki.osdomotics.com/doku.php/de:projekte:pflanzenbewaesserung](http://wiki.osdomotics.com/doku.php/de:projekte:pflanzenbewaesserung)

Usage Preliminaries
----------------------

initialize clock and watering timer (otherwise the watering will last for zero seconds)  
to initialize clock and watering timer replace IPv6 and use example script

	<init-watering.sh>

CoAP well known interface
----------------------

</.well-known/core>;ct=40,  
</a/wat_dur>;title="Watering duration sec., POST time=XXX";rt="Control",  
</a/wat_tods>;title="Watering start time of day, POST tods=hh:mm";rt="Control",  
</a/relay>;title="RELAY: ?type=1|2|3|4, POST/PUT, POST/PUT mode=on|off";rt="Control",  
</p/model>;title="model";rt="simple.dev.mdl",  
</p/sw>;title="Software Version";rt="simple.dev.sv",  
</p/name>;title="name";rt="simple.dev.n",  
</s/button>;title="Irrigation Start/Stop Button";obs,  
</a/remote_button>;title="Remote button";rt="simple.act.rbutton";obs,  
</a/led>;title="LED: POST/PUT mode=on|off";rt="simple.act.led",  
</a/toggle>;title="Red LED";rt="Control",  
</s/battery>;title="Battery status";rt="Battery",  
</s/cputemp>;title="CPU Temperature";rt="simple.sen.tmp",  
</clock/timestamp>;title="Time";rt=UCUM:"s";ct="0 5",  
</clock/localtime>;title="Local time";rt=UCUM:"formatted time";ct="0 5",  
</clock/utc>;title="UTC";rt=UCUM:"formatted time";ct="0 5"

