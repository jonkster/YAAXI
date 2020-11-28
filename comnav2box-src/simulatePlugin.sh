#/usr/bin/bash
echo "I am simulating XPlane and the Avduino plugin"
echo "listening for an Avduino Box..."
BOXMSG=`nc -4u -l -w0 192.168.0.255 8889`

if [ "$BOXMSG" == "XP Plugin Fish" ]
then
	echo "found arduino!"
	echo "ask for arduino details:"
	echo -n "Avduino Box Fish" | nc -4u -w0 192.168.0.190 8888
	echo "now listen for arduino details response..."
	nc -4u -l -w0  8889
	echo "I now know box details..."
	echo "sending a control message..."
	echo -n "C1USE:128.00" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "C1STBY:124.00" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "N1USE:108.00" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "N1STBY:108.05" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "C2USE:128.10" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "C2STBY:124.10" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "N2USE:108.10" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	echo -n "N2STBY:108.15" | nc -4u -w0 192.168.0.190 8888
	sleep 0.2
	sleep 2
	echo "listening for messages from arduino box"
	echo "(try setting switches, turning knobs on box)"
	nc -4u -l  8889
else
	echo "could not find box :(" $BOXMSG
fi

