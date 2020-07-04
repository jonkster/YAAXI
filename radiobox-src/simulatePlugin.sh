#/usr/bin/bash
echo "listening for an Avduino Box..."
BOXMSG=`nc -4u -l -w0 192.168.0.255 8889`

if [ "$BOXMSG" == "XP Plugin Fish" ]
then
	echo "found box!"
	echo "fish for box details"
	echo -n "Avduino Box Fish" | nc -4u -w0 192.168.0.181 8888
	nc -4u -l -w0 192.168.0.77 8889
	echo "know box details..."
	echo -n "DIGITS0:132800" | nc -4u -w0 192.168.0.181 8888
	echo -n "DIGITS1:124550" | nc -4u -w0 192.168.0.181 8888
	sleep 1
	echo -n "DIGITS0:124550" | nc -4u -w0 192.168.0.181 8888
	echo -n "DIGITS1:132800" | nc -4u -w0 192.168.0.181 8888
	sleep 1
	echo "listen for messages from box"
	echo "(try setting switches, turning knobs on box)"
	nc -4u -l 192.168.0.77 8889
else
	echo "could not find box :(" $BOXMSG
fi

