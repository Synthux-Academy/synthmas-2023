#!/bin/bash

# Simple Script to Connect to Daisy Seed
# Rebuild Project
make clean &
wait $!

DEBUG=1 make &
wait $!

# Make and Upload Program
make program &
wait $!

# Clear Screen and add loading screen
clear 
echo -ne 'Initializing #####                   (33%)\r'
sleep 1  
echo -ne 'Initializing #############           (66%)\r'
sleep 1
echo -ne 'Initializing ####################### (100%)\r'
echo -ne '\n'

# Wait a Second to Connect
sleep 2 &
wait $!

# Connect To Daisy
screen /dev/tty.usbmodem3781347A32301 115200
