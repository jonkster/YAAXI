#/usr/bin/bash
echo "I am simulating XPlane and the Avduino plugin"
echo "listening for an Avduino Box..."
BOXMSG=`nc -4u -l -w1 192.168.0.255 8889`

if grep -q "XP Plugin Fish:" <<< "$BOXMSG"
then
	IFS=':' read -a arr <<< ${BOXMSG}
	IP_ARD=${arr[1]}
	echo "found arduino with IP "  "'${IP_ARD}'"
	echo "ask for arduino details:"
	echo -n "Avduino Box Fish" | nc -4u -w0 ${IP_ARD} 8888
	line=`nc -4u -l -w1  8889`
	echo "details: " $line
	echo "sending ${IP_ARD} a control message..."
	echo "set COM1 Use freq to 128.10"
	echo -n "C1USE:128100" | nc -4u -w0 $IP_ARD 8888
	sleep 0.2
	echo "set COM2 Stdby freq to 124.00"
	echo -n "C2STB:124000" | nc -4u -w0 $IP_ARD 8888
	sleep 0.2
	sleep 2
	echo "listening for messages from arduino box ${IP_ARD}"
	echo "(try setting switches, turning knobs on box)"
	nc -4u -l  8889
else
	echo "could not find box :(" $BOXMSG
fi

