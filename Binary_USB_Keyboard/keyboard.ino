/* 
 *  Binary USB keyboard
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
 */

#define PIN_COL1 21
#define PIN_COL2 6
#define PIN_ROW1 19
#define PIN_LEDS 16
#define LED_COUNT 2

#include <FastLED.h>
CRGB leds[LED_COUNT];

#include <Keyboard.h>

void blink_start(int status, int duration);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, PIN_LEDS, GRB>(leds, LED_COUNT);
  pinMode(PIN_ROW1, OUTPUT);
  digitalWrite(PIN_ROW1, LOW);
  pinMode(PIN_COL1, INPUT_PULLUP);
  pinMode(PIN_COL2, INPUT_PULLUP);
  Keyboard.begin();
  blink_start(2, 2000); // notify that we're working
}

// blinks with interval timers to show status

uint32_t blink_until = 0;
uint32_t blink_timer = 0;
int blink_status = 0;
bool blink_on = false;
#define BLINK_INTERVAL 50

void blink_loop() {
  if (blink_status==0) return;
  int i;
  if (millis()>blink_timer) {
    blink_on = !blink_on;
    if (blink_on) {
      switch (blink_status) {
        case 1: // grey
          for (i=0; i<LED_COUNT; i++) leds[i]= CRGB(64,64,64);
          break;
        case 2: // green
          for (i=0; i<LED_COUNT; i++) leds[i]= CRGB(0,255,0);
          break;
      }
    } else { // off
      for (i=0; i<LED_COUNT; i++) leds[i]= CRGB(0,0,0);
    }
    FastLED.show();
    blink_timer = millis() + BLINK_INTERVAL;
  }
  if (millis()>blink_until) {
    for (i=0; i<LED_COUNT; i++) leds[i]= CRGB(0,0,0);
    FastLED.show();
    blink_status=0;
    blink_on=false;
  }
}

// start blinking in the given type, for this many ms
void blink_start(int status, int duration) {
  blink_until = millis() + duration;
  blink_status = status;
}

// returns the currently pressed key
int read_key() {
  // returns 0: none, 1=first, 2=second, 3=both
  //return (1-digitalRead(PIN_COL1)) + 2*(1-digitalRead(PIN_COL2));  // normal
  return (1-digitalRead(PIN_COL2)) + 2*(1-digitalRead(PIN_COL1));  // upside-down
}

// read the key once, debounce as needed
#define DEBOUNCE_TIMEOUT 25
uint32_t debounce_timeout = 0;
int last_key = 0;
int check_keys() { 
  if (millis()<debounce_timeout) return 0;
  int this_key = read_key();
  if (this_key==last_key) return 0; // nothing changed
  last_key = this_key;
  debounce_timeout = millis() + DEBOUNCE_TIMEOUT; // debounce time in ms
  return this_key; 
}

// tracking the currently entered 7-bit character
byte result = 0;
byte bitnum = 0;

// loops infintely
void loop() {
  blink_loop();
  int new_key = check_keys();
  if ((new_key==1) || (new_key==2)) { // do we have a key?
    Serial.print(new_key-1); // show bit
    Keyboard.write(char(new_key+48-1));
    blink_start(1, 10);
    result = result * 2; // shift
    result += new_key-1; 
    bitnum += 1;
    if (bitnum==7) { // all 7 bits done? show result, remove bits and enter it
      int i;
      Serial.print(" -> "); Serial.print(result); 
      Serial.print(" / "); Serial.print(char(result)); 
      Serial.println();
      for (i=0;i<7;i++) { Keyboard.press(KEY_BACKSPACE);delay(10);Keyboard.releaseAll(); }
      if (result>=32) {
        Keyboard.write(char(result));
      } else { // check control characters
        switch (result) {
          case 7: // 6 for replies, 8 for posts
            for (i=0; i<6; i++) { Keyboard.press(KEY_TAB);delay(20);Keyboard.releaseAll(); }
            break;
          case 13: Keyboard.press(KEY_RETURN);delay(10);Keyboard.releaseAll(); break;
        }
      }
      // start next character
      bitnum = 0; result = 0;
      blink_start(2, 500); // notify that we have a "byte"
    }
  }
}
