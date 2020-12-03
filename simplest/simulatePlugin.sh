#/usr/bin/bash
# this script establishes initial communication with the arduino device, it is
# generic.  You should write a box specific script called 'box.sh' that
# sends/receives device specific data to/from the arduino

echo "I am simulating XPlane and the Avduino plugin"
echo "listening for an Avduino Box..."
BOXMSG=`nc -4u -l -w1 192.168.0.255 8889`

if grep -q "XP Plugin Fish:" <<< "$BOXMSG"
then
	IFS=':' read -a arr <<< ${BOXMSG}
	IP_ARD=${arr[1]}
	echo "found arduino with IP "  "'${IP_ARD}'"
	./box.sh ${IP_ARD}
else
	echo "could not find box :(" $BOXMSG
fi

