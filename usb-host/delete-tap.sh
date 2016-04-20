#!/bin/bash

ARGC=$#

if [ $ARGC -lt 1 ]; then
    echo "Usage: $0 device" 1>&2
    exit
fi

NAME=$1
ip link delete $NAME
