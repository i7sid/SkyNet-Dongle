#!/bin/bash

ARGC=$#

if [ $ARGC -lt 3 ]; then
    echo "Usage: $0 user group device" 1>&2
    exit
fi

USER=$1
GROUP=$2
NAME=$3
tunctl -u $USER -g $GROUP -t $NAME

ip link set $NAME up
ip addr add 10.254.0.1/24 dev $NAME
ip link set dev $NAME arp off

sysctl -w net.ipv4.conf.${NAME}.rp_filter=2
