#!/bin/bash

ARGC=$#

if [ $ARGC -lt 4 ]; then
    echo "Usage: $0 user group device ip" 1>&2
    exit
fi

USER=$1
GROUP=$2
NAME=$3
IP=$4
tunctl -u $USER -g $GROUP -t $NAME

ip link set $NAME up
ip addr add $IP/24 dev $NAME
#ip link set dev $NAME arp off

sysctl -w net.ipv4.conf.${NAME}.rp_filter=2
