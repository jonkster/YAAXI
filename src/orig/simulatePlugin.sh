#/bin/sh
echo -n  "Discover:1" | nc -4u -b  -w0  -W1 192.168.0.255 8888
echo -n  "Discover:2" | nc -4u -b  -w0  -W1 192.168.0.255 8888
echo -n  "Discover:3" | nc -4u -b  -w0  -W1 192.168.0.255 8888
#echo -n "Hello" | nc -4u  -W1 192.168.0.177 8888
exit 0

echo -n "R:0:12725" | nc -4u  -W1 192.168.0.177 8888
echo -n "R:1:12100" | nc -4u  -W1 192.168.0.177 8888
echo -n "R:2:10955" | nc -4u  -W1 192.168.0.177 8888
echo -n "R:3:10875" | nc -4u  -W1 192.168.0.177 8888
exit 0

