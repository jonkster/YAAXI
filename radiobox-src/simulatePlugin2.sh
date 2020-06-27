#/usr/bin/bash

echo "set freq"
echo -n "DIGITS0:132800" | nc -4u -w0 192.168.0.181 8888
echo -n "DIGITS1:124550" | nc -4u -w0 192.168.0.181 8888
sleep 2
echo "swap freqs"
echo -n "DIGITS0:124550" | nc -4u -w0 192.168.0.181 8888
echo -n "DIGITS1:132800" | nc -4u -w0 192.168.0.181 8888
sleep 2
echo "listen for messages from box"
echo "(try setting switches, turning knobs on box)"
nc -4u -l 192.168.0.77 8889


