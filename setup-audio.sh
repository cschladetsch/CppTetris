#!/bin/bash
# Export the DISPLAY variable to connect to Windows
export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0
# Set the PULSE_SERVER to point to the Windows host
export PULSE_SERVER=tcp:$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}')
echo "Audio configured to use Windows host at $PULSE_SERVER"
