# Using PicKit4 for AVR MCU programming in Arduino

## Summary

* compile and install the latest AVRDUDE tool
* connect PicKit4 to the AVR-ISP connector
* set correct fuse byte by running `$ avrdude -c pickit4_isp -p m328p -U lfuse:w:0xFF:m`
* write HEX to the MCU by running `avrdude -c pickit4_isp -p m328p -U flash:w:filename.hex`

## Compile and Install the latest version of AVRDUDE

PicKit4 support is available on AVRDUDE v7.0 or grater.
At this time of writing, AVRDUDE in Ubuntu package repository is version 6.3
Thus clone the AVRDUDE's source repository and build it manually.

```
$ git clone https://github.com/avrdudes/avrdude
$ cd avrdude
$ sudo apt install cmake flex bison
$ ./build.sh
$ sudo cmake --build build_linux --target install
```

After being successful, `avrdude -v` command will give something similar to this.

```
$ avrdude -v

avrdude: Version 7.0-20220803 (53ece53)
         Copyright (c) Brian Dean, http://www.bdmicro.com/
         Copyright (c) Joerg Wunsch

         System wide configuration file is "/usr/local/bin/../etc/avrdude.conf"
         User configuration file is "/home/hotte/.avrduderc"
         User configuration file does not exist or is not a regular file, skipping


avrdude: no programmer has been specified on the command line or the config file
         Specify a programmer using the -c option and try again
```

## Connect PicKit4 Programmer to ATmega MCU

PicKit4 pin # | PicKit4 naming | AVRISP header naming | AVRISP pin #
--------------|----------------|----------------------|-------------
1             | TVPP           | -                    | -
2             | TVDD           | VCC                  | 2
3             | GND            | GND                  | 6
4             | PGD            | MISO                 | 1
5             | PGC            | SCK                  | 3
6             | TAUX           | RST                  | 5
7             | TTDI           | MOSI                 | 4
8             | TTMS           | -                    | -

## Testing on CLI

Check first if the PicKit4 is connected to the USB port correctly.

```
$ lsusb
...
Bus 003 Device 042: ID 03eb:2177 Atmel Corp. MPLAB PICkit 4 CMSIS-DAP
...
```

Assuming that ATmega328p (same one used in Arduino UNO rev3) is connected, the connection can be checked like this.

```
$ avrdude -p m328p -c pickit4_isp

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e950f (probably m328p)

avrdude done.  Thank you.
```

## Compile a simple Arduino program into HEX binary

(todo)


## Flash the ATmega MCU

Check the fuse bytes first.

```
$ avrdude -c pickit4_isp -p m328p -U lfuse:r:-:h
```

If the ATmega328p is very new device from factory, the default lfuse value will be 0x62.
ATmega328p is clocked by the Calibrated internal 8MHz RC Oscillator.The 8 MHz clock is divided by 8 due to the setting of the CKDIV8 bit in Fuse Low Byte. So effective clock rate available is 1 MHz.

In arduino IDE, choosing the board type as Arduino UNO, it assumes that using external 16MHz crystal oscillator as main clock source.
Thus, when initializing UART baud rate as 9600bps, it will work as 600bps.
To avoid this mess, need to correct the lfuse byte as 0xFF

```
$ avrdude -c pickit4_isp -p m328p -U lfuse:w:0xFF:m
```

Now complile any Arduino code in the Arduino IDE with the "Arduino UNO" board selected. And write it to the board.

```
$ avrdude -c pickit4_isp -p m328p -U flash:w:filename.hex
```

## Troubleshooting

If AVRDUDE cannot find a valid PicKit4 device in AVR programming mode, it will give the following error.

```
$ avrdude -p m328p -c pickit4_isp
avrdude: usbhid_open(): No device found
```

If PicKit4 is found, but the wiring between PicKit4 and AVR-ISP header is wrong, it will give the following error.

```
$ avrdude -p m328p -c pickit4_isp

avrdude: stk500v2_command(): command failed
avrdude: Target prepared for ISP, signed off.
avrdude: Now retrying without power-cycling the target.
avrdude: stk500v2_command(): command failed
avrdude: Failed to return from debugWIRE to ISP.
avrdude: initialization failed, rc=-1
         Double check connections and try again, or use -F to override
         this check.


avrdude done.  Thank you.
```
