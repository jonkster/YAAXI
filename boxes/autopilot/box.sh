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
		ALTITUDE=230
		ALTSEL=2000
		VS=500
		echo -n "ALT_SEL:${ALTSEL}" | nc -4u -w0 $1 8888
		echo -n "AP_VS:${VS}" | nc -4u -w0 $1 8888
		echo -n "ALTITUDE:${ALTITUDE}" | nc -4u -w0 $1 8888
		echo -n "AP_MODE:0" | nc -4u -w0 $1 8888
		echo -n "FD_MODE:0" | nc -4u -w0 $1 8888
		echo -n "VS_MODE:1" | nc -4u -w0 $1 8888
		echo "Got valid BOXID."
		echo "Sending ${1} control messages - AP annunciator should flash several times"
		for in in {1..3}
		do
			ALTITUDE=$(( $ALTITUDE + 500 ));
			ALTSEL=$(( $ALTSEL + 1000 ));
			VS=$(( $VS + 100 ));

			echo -n "AP_MODE:1" | nc -4u -w0 $1 8888
			sleep 0.5
			echo -n "AP_MODE:0" | nc -4u -w0 $1 8888
			sleep 0.5
		done
		sleep 1
		echo -n "AP_MODE:1" | nc -4u -w0 $1 8888
		echo -n "FD_MODE:1" | nc -4u -w0 $1 8888
		echo "listening for messages from arduino box ${1}"
		echo "(try toggling switches etc and check to see there are responses below. Ctrl-C to quit)"
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
