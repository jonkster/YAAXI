#/usr/bin/bash
# this is any box specific test/simulation
if [ -z $1 ]
then
	echo "No IP address, exiting"
else
	echo "Assuming arduino with IP "  "${1}"
	echo "Ask for arduino details:"
	echo -n "Avduino Box Fish" | nc -4u -w0 ${1} 8888
	BOXID=`nc -4u -l -w1 8889`
	echo "Looking for a BOXID response, got: ${BOXID}"

	if [[ "${BOXID}" == "BOXID"* ]]
	then
		echo "Got valid BOXID."
		echo "Sending ${1} control messages - LED should flash every second, several times..."
		for in in {1..3}
		do
			echo -n "LED0:1" | nc -4u -w0 $1 8888
			echo -n "LED1:1" | nc -4u -w0 $1 8888
			sleep 0.5 
			echo -n "LED0:0" | nc -4u -w0 $1 8888
			echo -n "LED1:0" | nc -4u -w0 $1 8888
			sleep 0.5
			echo "(check LED turned on and off...)"
		done
		sleep 3
		echo -n "LED0:1" | nc -4u -w0 $1 8888
		echo -n "LED1:0" | nc -4u -w0 $1 8888
		echo "listening for messages from arduino box ${1}"
		echo "(try toggling switch and check to see there are responses below. Ctrl-C to quit)"
		nc -4u -l  8889
	else
		if [[ "${BOXID}" == "XP Plugin Fish"* ]] 
		then
			echo "that is plugin fish request - the box is not properly responding to messages.  It isn't recognising the plugin :("
		else
			echo "that is not a proper BOXID response :("
		fi
	fi

fi

