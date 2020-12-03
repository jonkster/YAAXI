#/usr/bin/bash
# this is box specific test/simulation
COM1=12000
COM2=12500
NAV1=10900
NAV2=11000
if [ -z $1 ]
then
	echo "No IP address, exiting"
else
	echo "assuming arduino with IP "  "${1}"
	echo "ask for arduino details:"
	echo -n "Avduino Box Fish" | nc -4u -w0 ${1} 8888
	echo "response: "
	nc -4u -l -w1  8889
	echo
	echo "sending ${1} a control message..."
	echo "set COM1 Use freq to 128.10"
	echo -n "C1U:12810" | nc -4u -w0 $1 8888
	sleep 0.2
	echo "set COM2 Stdby freq to 124.00"
	echo -n "C2S:12400" | nc -4u -w0 $1 8888
	sleep 0.5
	echo "(check COM1 Use freq and COM2 Stdby freqs look OK...)"
	sleep 3
	echo "listening for messages from arduino box ${1}"
	echo "(try setting switches, turning knobs on box and check to see there are responses below...)"
	while [ true ]	
	do
		RESPONSE=`nc -4u -w0 -l  8889` 
		echo ${RESPONSE}
		case ${RESPONSE} in
			"MHZ_KNOB_COM1:1")
				COM1=$((${COM1} + 100))
				echo -n "C1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_COM1:-1")
				COM1=$((${COM1} - 100))
				echo -n "C1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_COM1:1")
				COM1=$((${COM1} + 5))
				echo -n "C1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_COM1:-1")
				COM1=$((${COM1} - 5))
				echo -n "C1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_COM1:2")
				COM1=$((${COM1} + 100))
				echo -n "C2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_COM2:-1")
				COM1=$((${COM1} - 100))
				echo -n "C2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_COM2:1")
				COM1=$((${COM1} + 5))
				echo -n "C2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_COM2:-1")
				COM1=$((${COM1} - 5))
				echo -n "C2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_NAV1:1")
				COM1=$((${COM1} + 100))
				echo -n "N1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_NAV1:-1")
				COM1=$((${COM1} - 100))
				echo -n "N1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_NAV1:1")
				COM1=$((${COM1} + 5))
				echo -n "N1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_NAV1:-1")
				COM1=$((${COM1} - 5))
				echo -n "N1S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_NAV1:2")
				COM1=$((${COM1} + 100))
				echo -n "N2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"MHZ_KNOB_NAV1")
				COM1=$((${COM1} - 100))
				echo -n "N2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_NAV1")
				COM1=$((${COM1} + 5))
				echo -n "N2S:${COM1}" | nc -4u -w0 $1 8888
				;;
			"KHZ_KNOB_NAV1")
				COM1=$((${COM1} - 5))
				echo -n "N2S:${COM1}" | nc -4u -w0 $1 8888
				;;
		esac
	done
fi

