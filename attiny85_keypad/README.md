# simple 1-key USB keyboard using an attiny85 mcu

Based on Digistump's Digispark attiny85 design & firmware. Runs Arduino code. 

Kinda flakey USB connection, probably because the attiny85 doesn't have a real clock. Firmware uses V-USB. 

Arduino sketch is super-basic.

Board is designed for mechanical keyswitches (MX Cherry). PCB is round, 26mm in diameter (fits in a beer bottle-cap). Getting the PCB made with components comes to about $3/board (when ordering 5 at JLCPCB; you add the micro-USB port though). 

## Programming

1. Download & install things

a. The micronucleus firmware (https://github.com/micronucleus/micronucleus)

git clone https://github.com/micronucleus/micronucleus.git

b. avrdude

sudo apt-get update
sudo apt-get install avrdude

2. Connect PCB to ISP connector on Arduino (or whatever you use to program)

3. Install the ArduinoISP sketch on the Arduino

4. Burn the Micronucleus firmware / bootloader "aggressive" to attiny85 onto the PCB

avrdude -v -P /dev/ttyACM0 -b 19200 -c stk500v1 -p attiny85 -Uflash:w:micronucleus/firmware/releases/t85_aggressive.hex:i -v

(Weirdly, the ArduinoISP sketch needs to be set as stk500v1 in avrdude. Shrug.)

5. Connect PCB to USB directly

6. Install Arduino sketch

7. Retry any step that flakes (it often does)

8. Give up and take a break. Then come back and try it again. Good luck. 

