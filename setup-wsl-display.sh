#!/bin/bash
# Setup script for running Tetris in WSL2

# Get WSL2 host IP
export WSL_HOST_IP=$(ip route | grep default | awk '{print $3}')
if [ -z "$WSL_HOST_IP" ]; then
  WSL_HOST_IP=$(grep nameserver /etc/resolv.conf | awk '{print $2}')
fi

# Set DISPLAY variable for X11 forwarding
export DISPLAY=${WSL_HOST_IP}:0

echo "Setting up WSL2 display forwarding..."
echo "DISPLAY set to: $DISPLAY"

# Check if an X server is likely running on Windows
if nc -z -w 1 ${WSL_HOST_IP} 6000 >/dev/null 2>&1; then
  echo "X server appears to be running on Windows host."
else
  echo "X server not detected on Windows host ($WSL_HOST_IP:6000)"
  echo "Please make sure you have VcXsrv, Xming, or another X server running on Windows."
  echo "Installation instructions:"
  echo "1. Download and install VcXsrv: https://sourceforge.net/projects/vcxsrv/"
  echo "2. Run XLaunch from the Start menu"
  echo "3. Select 'Multiple windows' and set Display number to 0"
  echo "4. Select 'Start no client'"
  echo "5. Check 'Disable access control'"
  echo "6. Click Finish"
fi

echo "For better SDL compatibility, we're setting the SDL environment variables:"
export SDL_VIDEODRIVER=x11
echo "SDL_VIDEODRIVER set to: $SDL_VIDEODRIVER"

# Set audio environment variables
export PULSE_SERVER=tcp:${WSL_HOST_IP}
export SDL_AUDIODRIVER=directsound
echo "Audio variables set for SDL"

echo ""
echo "You can add these to your ~/.bashrc to make them permanent:"
echo "export DISPLAY=${WSL_HOST_IP}:0"
echo "export SDL_VIDEODRIVER=x11"
echo "export PULSE_SERVER=tcp:${WSL_HOST_IP}"
echo "export SDL_AUDIODRIVER=directsound"
echo ""

echo "Now try running the game with: ./r"