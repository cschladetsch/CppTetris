#!/bin/bash
# For WSL2, we need to handle audio in a special way
# The IP in resolv.conf isn't always reliable for audio

# First try to get the host IP more reliably by looking at the default route
if command -v ip &>/dev/null; then
  export WSL_HOST_IP=$(ip route | grep default | awk '{print $3}')
else
  # Fallback to resolv.conf method
  export WSL_HOST_IP=$(grep nameserver /etc/resolv.conf | awk '{print $2}')
fi

echo "Detected Windows host IP: $WSL_HOST_IP"

# Offer option to manually specify host IP
read -p "Use this IP? If not, enter your Windows host IP manually (or press Enter to accept): " MANUAL_IP
if [ ! -z "$MANUAL_IP" ]; then
  WSL_HOST_IP=$MANUAL_IP
  echo "Using manually provided IP: $WSL_HOST_IP"
fi

export DISPLAY=$WSL_HOST_IP:0
export PULSE_SERVER=tcp:$WSL_HOST_IP

# Ensure required packages are installed
if ! command -v pulseaudio &> /dev/null; then
    echo "PulseAudio is not installed. Attempting to install..."
    sudo apt update && sudo apt install -y pulseaudio pulseaudio-utils
fi

# Kill any existing PulseAudio processes that might be running with wrong config
pkill pulseaudio 2>/dev/null

# Wait to ensure process is fully terminated
sleep 1

# Ensure PulseAudio config directory exists
mkdir -p ~/.config/pulse

# Create a custom client.conf that uses the network
cat > ~/.config/pulse/client.conf << EOF
default-server = tcp:$WSL_HOST_IP
# Prevent automatic startup of the PulseAudio daemon
autospawn = no
EOF

# Test audio configuration
echo "Testing audio configuration..."
aplay -l
echo "If you see audio devices listed above, that's good!"

# Check if Windows has PulseAudio server running
echo "Testing connection to PulseAudio server on Windows host..."
timeout 3 pactl info || { 
    echo "Couldn't connect to PulseAudio server on Windows host ($WSL_HOST_IP)."
    echo "TROUBLESHOOTING STEPS:"
    echo "1. Ensure PulseAudio server is running on Windows"
    echo "   - Install PulseAudio for Windows: https://www.freedesktop.org/wiki/Software/PulseAudio/Ports/Windows/"
    echo "   - Or use WSL PulseAudio Server: https://github.com/microsoft/WSL/issues/5816#issuecomment-786301004"
    echo "2. Check Windows Defender Firewall - it may be blocking connections"
    echo "3. Try finding the correct host IP by running this in PowerShell on Windows:"
    echo "   ipconfig | findstr IPv4"
    echo "4. For a quick test, you can try to run SDL/OpenAL applications with:"
    echo "   export SDL_AUDIODRIVER=directsound"
    echo "   export ALSA_CONFIG_PATH=/usr/share/alsa/alsa.conf"
}

# For SDL-based apps like Tetris, we can try to set a direct backend as well
echo "Setting up SDL audio driver fallback..."
export SDL_AUDIODRIVER=directsound
export ALSA_CONFIG_PATH=/usr/share/alsa/alsa.conf

echo "Audio setup complete. Environment variables have been set for this terminal session."
echo "To make these settings permanent, add these lines to your ~/.bashrc:"
echo "  export DISPLAY=$WSL_HOST_IP:0"
echo "  export PULSE_SERVER=tcp:$WSL_HOST_IP"
