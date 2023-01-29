#!/bin/bash
# This Script is for Ubuntu/Debian people
# Install make and gcc
apt install gcc -y
apt install make -y
# Libraries for libsdl2
apt install libsdl2-dev -y
apt install libsdl2-image-dev -y
apt install libsdl2-mixer-dev -y
apt install libsdl2-ttf-dev -y
# Following are WSL specific requirements
apt install libpng-dev -y
apt install libjpeg-dev -y
