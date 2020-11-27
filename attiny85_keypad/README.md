# simple 1-key USB keyboard using an attiny85 mcu

Based on Digistump's Digispark attiny85 design & firmware. Runs Arduino code. 

Kinda flakey USB connection, probably because the attiny85 doesn't have a real clock. Firmware uses V-USB. 

Arduino sketch is super-basic.

Board is designed for mechanical keyswitches (MX Cherry). PCB is round, 26mm in diameter (fits in a beer bottle-cap). Getting the PCB made with components comes to about $3/board (when ordering 5 at JLCPCB; you add the micro-USB port though). 

## Programming

1. Download & install things

  a. The micronucleus firmware (https://github.com/micronucleus/micronucleus)

`git clone https://github.com/micronucleus/micronucleus.git`

You can use the pre-compiled releases directly, no need to install or make anything for Micronucleus. You could also just download the t85_aggressive.hex file directly. 

  b. avrdude

`sudo apt-get update`

`sudo apt-get install avrdude`

2. Set up your ISP programmer

  a. Connect PCB to ISP connector on Arduino (or whatever you use to program)

I used a pogo-pin connector, you could try something similar or just hold the board to a bunch of pins. It's flakey without a good connection, but you only need to burn the bootloader once. For the Arduino ISP sketch you can either connect the ICSP header pins (without pin 5 - reset) to the PCB or use pins 10-13 + 5v + GND separately. 

  b. Test the ICSP connection (optional)

It's flakey. At least make sure you can use it. I used https://github.com/nickgammon/arduino_sketches/tree/master/Atmega_Board_Detector to check.

Output:

```
22:48:23.561 -> 
22:48:23.561 -> Atmega chip detector.
22:48:23.561 -> Written by Nick Gammon.
22:48:23.561 -> Version 1.20
22:48:23.561 -> Compiled on Nov 27 2020 at 22:47:07 with Arduino IDE 10813.
22:48:23.561 -> Attempting to enter ICSP programming mode ...
22:48:23.694 -> Entered programming mode OK.
22:48:23.694 -> Signature = 0x1E 0x93 0x0B 
22:48:23.694 -> Processor = ATtiny85
22:48:23.694 -> Flash memory size = 8192 bytes.
22:48:23.694 -> LFuse = 0x62 
22:48:23.694 -> HFuse = 0xDF 
22:48:23.694 -> EFuse = 0xFF 
22:48:23.694 -> Lock byte = 0xFF 
22:48:23.694 -> Clock calibration = 0xA8 
22:48:23.727 -> No bootloader support.
22:48:23.727 -> 
22:48:23.727 -> First 256 bytes of program memory:
22:48:23.727 -> 
22:48:23.727 -> 00: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.727 -> 10: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.727 -> 20: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.760 -> 30: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.760 -> 40: 0xFF 0xFE 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.760 -> 50: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.760 -> 60: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.760 -> 70: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.793 -> 80: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.793 -> 90: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.793 -> A0: 0xFF 0xFF 0xFF 0xFF 0xFE 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.793 -> B0: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.793 -> C0: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.826 -> D0: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.826 -> E0: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.826 -> F0: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 
22:48:23.826 -> 
22:48:23.826 -> Programming mode off.
```

Note it recognizes the attiny85 MCU.

  c. Install the ArduinoISP sketch on the Arduino

See https://www.arduino.cc/en/Tutorial/BuiltInExamples/ArduinoISP for more information. See https://www.gammon.com.au/bootloader for more detailed information. 


3. Burn the Micronucleus firmware / bootloader "aggressive" to attiny85 onto the PCB

a. Burn bootloader

`avrdude -v -v -P /dev/ttyACM0 -b 19200 -c stk500v1 -p attiny85 -Uflash:w:micronucleus/firmware/releases/t85_aggressive.hex:i`

(Weirdly, the ArduinoISP sketch needs to be set as stk500v1 in avrdude. Shrug.)

```
avrdude: Version 6.3-20171130
         Copyright (c) 2000-2005 Brian Dean, http://www.bdmicro.com/
         Copyright (c) 2007-2014 Joerg Wunsch

         System wide configuration file is "/etc/avrdude.conf"
         User configuration file is "/home/x/.avrduderc"
         User configuration file does not exist or is not a regular file, skipping

         Using Port                    : /dev/ttyACM0
         Using Programmer              : stk500v1
         Overriding Baud Rate          : 19200
         AVR Part                      : ATtiny85
         Chip Erase delay              : 4500 us
         PAGEL                         : P00
         BS2                           : P00
         RESET disposition             : possible i/o
         RETRY pulse                   : SCK
         serial program mode           : yes
         parallel program mode         : yes
         Timeout                       : 200
         StabDelay                     : 100
         CmdexeDelay                   : 25
         SyncLoops                     : 32
         ByteDelay                     : 0
         PollIndex                     : 3
         PollValue                     : 0x53
         Memory Detail                 :

                                  Block Poll               Page                       Polled
           Memory Type Mode Delay Size  Indx Paged  Size   Size #Pages MinW  MaxW   ReadBack
           ----------- ---- ----- ----- ---- ------ ------ ---- ------ ----- ----- ---------
           eeprom        65     6     4    0 no        512    4      0  4000  4500 0xff 0xff
           flash         65     6    32    0 yes      8192   64    128  4500  4500 0xff 0xff
           signature      0     0     0    0 no          3    0      0     0     0 0x00 0x00
           lock           0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           lfuse          0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           hfuse          0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           efuse          0     0     0    0 no          1    0      0  9000  9000 0x00 0x00
           calibration    0     0     0    0 no          1    0      0     0     0 0x00 0x00

         Programmer Type : STK500
         Description     : Atmel STK500 Version 1.x firmware
         Hardware Version: 2
         Firmware Version: 1.18
         Topcard         : Unknown
         Vtarget         : 0.0 V
         Varef           : 0.0 V
         Oscillator      : Off
         SCK period      : 0.1 us

avrdude: PAGEL and BS2 signals not defined in the configuration file for part ATtiny85, using dummy values
avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e930b (probably t85)
avrdude: safemode: lfuse reads as 62
avrdude: safemode: hfuse reads as DF
avrdude: safemode: efuse reads as FF
avrdude: NOTE: "flash" memory has been specified, an erase cycle will be performed
         To disable this feature, specify the -D option.
avrdude: erasing chip
avrdude: PAGEL and BS2 signals not defined in the configuration file for part ATtiny85, using dummy values
avrdude: reading input file "micronucleus/firmware/releases/t85_aggressive.hex"
avrdude: writing flash (8138 bytes):

Writing | ################################################## | 100% 0.00s

avrdude: 8138 bytes of flash written
avrdude: verifying flash memory against micronucleus/firmware/releases/t85_aggressive.hex:
avrdude: load data flash data from input file micronucleus/firmware/releases/t85_aggressive.hex:
avrdude: input file micronucleus/firmware/releases/t85_aggressive.hex contains 8138 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.00s

avrdude: verifying ...
avrdude: 8138 bytes of flash verified

avrdude: safemode: lfuse reads as 62
avrdude: safemode: hfuse reads as DF
avrdude: safemode: efuse reads as FF
avrdude: safemode: Fuses OK (E:FF, H:DF, L:62)

avrdude done.  Thank you.
```

b. Burn some fuses

This sets the fuses so that things work. 

`avrdude -v -v -P /dev/ttyACM0 -b 19200 -c stk500v1 -p attiny85 -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m`

or ...  change the value of the CKSEL bit (

`avrdude -v -v -P /dev/ttyACM0 -b 19200 -c stk500v1 -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m`

Fuse settings:

Picking fuse settings: http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85

L:E1 = PLL Clock; Start-up time PWRDWN/RESET: 1K CK/14 CK + 64 ms; [CKSEL=0001 SUT=10]

L:E2 = Int. RC Osc. 8 MHz; Start-up time PWRDWN/RESET: 6 CK/14 CK + 64 ms; [CKSEL=0010 SUT=10]; default value

H:DD = Brown-out detection level at VCC=2.7 V; [BODLEVEL=101] + Serial program downloading (SPI) enabled; [SPIEN=0]

E:FF = nothing

c. Verify fuses

(should read E:FE, H:DD, L:E1)

`avrdude -v -v -P /dev/ttyACM0 -b 19200 -c stk500v1 -p attiny85 -U lfuse:r:-:i`

```
...stuff...
avrdude: safemode: lfuse reads as E1
avrdude: safemode: hfuse reads as DD
avrdude: safemode: efuse reads as FE
avrdude: safemode: Fuses OK (E:FE, H:DD, L:E2)

avrdude done.  Thank you.
```

4. Connect PCB to USB directly

5. Install Arduino sketch

- Add additional board: http://digistump.com/package_digistump_index.json
- Install "Digistump AVR Boards"
- Select board "Digispark Default" and programmer "Micronucleus"

6. Retry any step that flakes (it often does)

7. Give up and take a break. Then come back and try it again. Good luck. 


## Sources:
- https://gist.github.com/amcolash/b930cb9d206b75ca5fd4ab974cb78a60
- https://learn.sparkfun.com/tutorials/how-to-install-an-attiny-bootloader-with-virtual-usb/install-micronucleus
- http://digistump.com/wiki/digispark/tutorials/connecting#software
- https://digistump.com/wiki/digispark/tutorials/programming
- https://www.gammon.com.au/bootloader
- https://github.com/nickgammon/arduino_sketches
- https://gist.github.com/Ircama/22707e938e9c8f169d9fe187797a2a2c
- https://www.instructables.com/How-to-Program-an-ATtiny-85-Digispark/
