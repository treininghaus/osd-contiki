#!/bin/sh

# MB in der Vorführanlage
suffix=343c
# MB in der 1-fach Ventilbox
# suffix=66ed
# MB ohne powerboard für Demo Programmierung
# suffix=665b

# MB im Powerboard von Harald 2016.06
# suffix=349f

echo $suffix
echo aaaa::221:2eff:ff00:$suffix

date +%s | coap-client -t text/plain -f-  -m put 'coap://[aaaa::221:2eff:ff00:'$suffix']:5683/clock/timestamp'
coap-client -m POST -e time=10 coap://[aaaa::221:2eff:ff00:$suffix]:5683/a/wat_dur
coap-client -m POST -e tods=17:55 coap://[aaaa::221:2eff:ff00:$suffix]:5683/a/wat_tods

