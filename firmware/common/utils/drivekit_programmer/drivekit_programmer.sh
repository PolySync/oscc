#!/bin/bash
INPUT_STRING=0
echo "Press return to begin progamming"
read


while [ "$INPUT_STRING" != "q" ]
do
  avrdude -c usbasp -p m32u4 -U flash:w:Leonardo-prod-firmware-2012-12-10.hex
  echo "Programming Complete! Press enter to program another device or q to quit."
  read INPUT_STRING
done

