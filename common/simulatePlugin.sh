#/usr/bin/bash
# this script establishes initial communication with the arduino device, it is
# generic.  You should write a box specific script called 'box.sh' that
# sends/receives device specific data to/from the arduino

XP_PLUGIN_LISTEN_PORT=8889
echo "I am simulating XPlane and the Avduino plugin"
echo "listening for an Avduino Box on port $XP_PLUGIN_LISTEN_PORT..."
BOXMSG=`nc -4u -l -w1 $XP_PLUGIN_LISTEN_PORT`

if grep -q "XP Plugin Fish:" <<< "$BOXMSG"
then
	echo "I received '$BOXMSG'"
	IFS=':' read -a arr <<< ${BOXMSG}
	IP_ARD=${arr[1]}
	echo "found arduino with IP '${IP_ARD}'"
	./box.sh ${IP_ARD}
else
	echo "could not find box :(    I received '$BOXMSG'"
fi

