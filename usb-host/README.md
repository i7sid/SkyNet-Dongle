SkyNet-Dongle host application
==============================

For use with a tap device (as a transparent ethernet bridge) you need to be on
a linux machine with enabled TUNTAP support.
The program `tunctl` is needed to perform tap device setup.


Prerequisites
-------------
You need an up-to-date GNU C++ toolchain with support for C++11.

On Debian or its derivates you need the following packages:

    uml-utilities (for tunctl, so for setup-tap.sh script)
    build-essentials
    libmysql++-dev


Building
--------

    make
    # in Makefile you can uncomment the NO_TAP line,
    # then no dependencies for linux networking headers are left


Usage with tap device
---------------------

    ./setup-tap.sh user group devname ip
    # for example:
    ./setup-tap.sh root root tap0 10.254.0.1


Usage as base station logger
----------------------------
    
