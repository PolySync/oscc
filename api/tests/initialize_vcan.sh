#!/bin/bash

# sets up two virtual can interfaces, vcan0 and vcan1

lsmod | grep -q "vcan"
VCAN_NOT_LOADED=$?

if [ $VCAN_NOT_LOADED -eq 1 ]; then
    echo "vcan kernel module is not available..."
    echo "loading it;"
    sudo modprobe -a vcan
fi

ifconfig vcan0 > /dev/null
VCAN_NOT_EXIST=$?

if [ $VCAN_NOT_EXIST -eq 1 ]; then
  echo "vcan0 does not exist, creating it."
  sudo ip link add dev vcan0 type vcan
  sudo ip link set vcan0 up
else
  echo "vcan0 already exists."
fi