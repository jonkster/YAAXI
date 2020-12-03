#/usr/bin/bash
# this is any box specific test/simulation
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
	echo -n "C1U:128100" | nc -4u -w0 $1 8888
	sleep 0.2
	echo "set COM2 Stdby freq to 124.00"
	echo -n "C2S:124000" | nc -4u -w0 $1 8888
	sleep 0.5
	echo "(check COM1 Use freq and COM2 Stdby freqs look OK...)"
	sleep 3
	echo "listening for messages from arduino box ${1}"
	echo "(try setting switches, turning knobs on box and check to see there are responses below...)"
	nc -4u -l  8889
fi

