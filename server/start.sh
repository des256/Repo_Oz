#!/bin/sh

# TODO: make sure this is run as root

cd /usr/oz/arch/robokind-r50/bin
export LD_LIBRARY_PATH=../lib
./ozserver

