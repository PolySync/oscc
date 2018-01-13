#!/bin/bash

echo "Resetting Device"

#Find serial port (Linux)
ARDUINO_UPLOAD_PORT="$(find /dev/ttyACM* | head -n 1)"

#Find serial port (Mac)
#ARDUINO_UPLOAD_PORT="$(find /dev/cu.usbmodem* | head -n 1)"

# reset the Arduino
stty -F "${ARDUINO_UPLOAD_PORT}" 1200

# wait for it...
while :; do
  sleep 0.5
  [ -c "${ARDUINO_UPLOAD_PORT}" ] && break
done
