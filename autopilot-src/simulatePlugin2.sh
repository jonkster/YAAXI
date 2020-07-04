#/usr/bin/bash

echo "turn all box leds on"
echo -n "LED0:1" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED1:1" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED2:1" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED3:1" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED4:1" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED5:1" | nc -4u -w0 192.168.0.180 8888
sleep 2
echo "turn all box leds off"
echo -n "LED0:0" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED1:0" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED2:0" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED3:0" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED4:0" | nc -4u -w0 192.168.0.180 8888
sleep 0.2
echo -n "LED5:0" | nc -4u -w0 192.168.0.180 8888
sleep 2
echo "listen for messages from box"
echo "(try setting switches, turning knobs on box)"
nc -4u -l 192.168.0.77 8889


