#!/bin/bash
# Get WSL IP address
export WSL_HOST_IP=$(grep nameserver /etc/resolv.conf | awk '{print $2}')
export DISPLAY=$WSL_HOST_IP:0
export PULSE_SERVER=tcp:$WSL_HOST_IP

# Optional: kill any existing PulseAudio processes
pkill pulseaudio 2>/dev/null

# Test if we can play a sound
echo "Testing audio..."
aplay -l
echo "If you see audio devices listed above, that's good!"

echo "Setup complete. Your audio should work now."
