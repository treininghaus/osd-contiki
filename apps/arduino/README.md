Arduino Compatibility
=====================

This application contains hardware-independent implementations of
arduino compatibilty libraries and include files to be used with
contiki.

The whole arduino compatibility library is work in progress. Note that
features having to do with timers like `millis` and `delay` are
currently untested. In Arduino they use timer 0 of the AVR
microcontroller. It should be investigated to use the hardware timer
already in use by contiki (on the OSD-merkur platform this is currently
timer 5).
