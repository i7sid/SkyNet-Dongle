SkyNet-Dongle
=============

Welcome to the SkyNet-Dongle git repository!
The main work was done in the context of a bachelor thesis at the
Friedrich-Alexander University Erlangen-Nürnberg.


Hardware
--------
The main components are
* CPU: LPC1769
* Radio: RF4463PRO (Chipset: Si4463)
* Bluetooth: HC-05 (Chipset: CSR BC417)


Firmware
--------
The firmware for the LPC1769 is written entirely in C99 and can **basically**
be compiled by a standard GNU GCC toolchain.
Currently a Eclipse project is shipped that works with
[LPCXpresso](http://www.embeddedartists.com/products/lpcxpresso).
Additionally you need the [LPCOpen chip library](http://www.lpcware.com/lpcopen).

If both projects - lpc_chip_175x_6x and SkyNet-Dongle-Firmware - are present in
your current workspace you should be able to successfully compile the firmware.

The firmware uses the Si446x API library from Silicon Laboratories, Inc.
The library was slightly adjusted to compile for and work nicely on LPC1769.



Flashing / Programming
----------------------
TODO  /  TBA



EAGLE files (schematics, layout)
--------------------------------

The eagle files contained in this repository
(schematics, layouts) need the following libraries:
* [lpc1xxx-v6.lbr](http://www.cadsoft.de/downloads/file/lpc1xxx-v6.lbr)
* [Bluetooth HC-05.lbr](https://github.com/ErichStyger/mcuoneclipse/tree/master/Eagle/Library)
* [microchip-battery.lbr](https://github.com/scott-42/eagle-lib)
* [usb_con-update.lbr](http://www.cadsoft.de/downloads/file/usb_con-update.lbr)
* TPS63001 (from this repository)
* RF4463PRO (from this repository)
* skynet_other (from this repository)


