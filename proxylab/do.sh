#!/bin/bash
#
#
cd ./tiny
./tiny 18855 &
cd ..
./proxy 18854 &


