SkyNet-Dongle
=============

Welcome to the SkyNet-Dongle git repository!
The initial work was done in the context of a bachelor thesis at the
Friedrich-Alexander University Erlangen-Nürnberg.
Some updates were made during a master thesis at the
Friedrich-Alexander University Erlangen-Nürnberg.



Hardware
--------
The main components are
* CPU: LPC1769
* Radio: [LoRa-Feather](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module?view=all)


Firmware
--------
The firmware for the LPC1769 is written entirely in C99 and can **basically**
be compiled by a standard GNU GCC cross compiling toolchain.
Currently an Eclipse project is shipped that works with
[LPCXpresso](https://www.lpcware.com/lpcxpresso/download).
Additionally you need the [LPCOpen chip library](http://www.lpcware.com/lpcopen).

If both projects - lpc_chip_175x_6x and SkyNet-Dongle-Firmware - are present in
your current workspace you should be able to successfully compile the firmware.

The firmware uses the Si446x API library from Silicon Laboratories, Inc.
The library was slightly adjusted to compile for and work nicely on LPC1769.


Pinouts
-------
... are documented in `board/pinouts.txt`.


Flashing / Programming
----------------------
First flashing has to be done via ISP or JTAG.
Flash `bootloader/src/bootloader.bin` to the processor.
Afterwards you should be able to flash new firmware files
(.bin) via USB and mass storage emulation.

That works as following:
Send via USB CDC serial terminal a "goto bootloader" command.
You can use:

    ./usb-host/skynet-usb-host

(You have to compile the host application before that.)

The dongle restarts and goes into bootloader mode.
Now the dongle is detected by host as a mass storage device.
Copy firmware binary to the mass storage device and *eject the device* afterwards:

    sudo eject /dev/{devices name, usually sdb, sdc, sdd, or any...}

or:

    sudo umount {MOUNTPOINT}
    sudo udisk --detach /dev/{devices name, usually sdb, sdc, sdd, or any...}



Bootloader source: https://github.com/openxc/openlpc-USB_Bootloader



EAGLE files (schematics, layout)
--------------------------------

The eagle files contained in this repository
(schematics, layouts) use the following libraries:
* LPC1769: [lpc1xxx-v6.lbr](http://www.cadsoft.de/downloads/file/lpc1xxx-v6.lbr)
* HC-05: [Bluetooth HC-05.lbr](https://github.com/ErichStyger/mcuoneclipse/tree/master/Eagle/Library)
* MCP73871: [microchip-battery.lbr](https://github.com/scott-42/eagle-lib)
* Micro USB connector: [usb_con-update.lbr](http://www.cadsoft.de/downloads/file/usb_con-update.lbr)
* Switch: [switch-ck.lbr](https://github.com/robertstarr/lbr_user/blob/master/switch-ck.lbr)
* [Microbuilder Library](http://www.microbuilder.eu/Projects/EagleFootprintLibrary.aspx)
* RF4463PRO (from this repository)
* skynet_other (from this repository)


