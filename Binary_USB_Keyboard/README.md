
#  Binary USB keyboard

 *  By twitter.com/johnmu / Apache 2.0 license
 *  Arduino code.
 *  Uses https://github.com/FastLED/FastLED for WS2812B RGB lights
 *  
 *  Device: 3-key-ecosystems/2key2
 *  https://github.com/softplus/3keyecosystem
 *  
 *  Pins: (tweak for other devices)
 *  WS2812B DI -> PB2 / D16
 *  COL1 -> PF4 / D21
 *  ROW1 -> PF6 / D19
 *  COL2 -> PD7 / D6
 *  
 *  Reading keys:
 *  - set low on ROW1
 *  - read COL1, COL2 with internal pull-ups
 *  
 *  Decodes 7-bit binary, showing bits, and replacing with the character
 *  7/Tab = 6x tab key, 13 = return key, no other control keys
 *  Sample: https://www.youtube.com/watch?v=7qM0AqqsMCQ
