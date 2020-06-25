#/usr/bin/bash
echo "listening for an Avduino Box..."
BOXMSG=`nc -4u -l -w0 192.168.0.255 8889`

if [ "$BOXMSG" == "XP Plugin Fish" ]
then
	echo "found box!"
	echo "fish for box details"
	echo -n "Avduino Box Fish" | nc -4u -w0 192.168.0.178 8888
	nc -4u -l -w0 192.168.0.179 8889
	echo "know box details..."
	echo "turn all box leds on"
	echo -n "LED0:1" | nc -4u -w0 192.168.0.178 8888
	sleep 0.2
	echo -n "LED1:1" | nc -4u -w0 192.168.0.178 8888
	sleep 0.2
	echo -n "LED2:1" | nc -4u -w0 192.168.0.178 8888
	sleep 0.2
	echo -n "LED3:1" | nc -4u -w0 192.168.0.178 8888
	sleep 0.2
	echo -n "LED4:1" | nc -4u -w0 192.168.0.178 8888
	sleep 0.2
	echo -n "LED5:1" | nc -4u -w0 192.168.0.178 8888
	sleep 2
	echo "turn all box leds off"
	echo -n "LED0:0" | nc -4u -w0 192.168.0.178 8888
	echo -n "LED1:0" | nc -4u -w0 192.168.0.178 8888
	echo -n "LED2:0" | nc -4u -w0 192.168.0.178 8888
	echo -n "LED3:0" | nc -4u -w0 192.168.0.178 8888
	echo -n "LED4:0" | nc -4u -w0 192.168.0.178 8888
	echo -n "LED5:0" | nc -4u -w0 192.168.0.178 8888
	sleep 2
	echo "listen for messages from box"
	echo "(try setting switches, turning knobs on box)"
	nc -4u -l 192.168.0.179 8889
else
	echo "could not find box :(" $BOXMSG
fi

