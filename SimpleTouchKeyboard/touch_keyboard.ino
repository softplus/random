//
// SimpleTouchKeyboard
//   Uses touch-pin (any wire, etc) to trigger a keypress as a USB keyboard
//   Requires USB HID keyboard capable Arduino device
//   https://github.com/softplus/random/tree/master/SimpleTouchKeyboard
//

// Hardware: Sparkfun Pro Micro (5v or 3.3v), or cheap clone ( https://www.aliexpress.com/wholesale?SearchText=%22pro+micro%22 )
// Pin A3 connected to paperclip & whatever metal thing you want to use to trigger
// 
// Uses libraries:
// Keyboard
//   generic Arduino USB keyboard library
//   https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
// ADCTouch
//   capacative touch sensor library
//   https://github.com/martin2250/ADCTouch
//   Available in Arduino libraries
//

// two LED pins on Pro Micro are 17 & 30; we'll use 17
#define LED_PIN 17

// where we're reading the touch, no particular reason why I chose this one
#define PIN_TOUCH A3

#include <Keyboard.h>
#include <ADCTouch.h>
int referenceValue;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_PIN, OUTPUT);   // Make LEDs blinkable
  Serial.begin(115200);       // Start serial USB port (for debugging)
  Keyboard.begin();           // Start acting like a USB keyboard

  // reference value for touch
  referenceValue = ADCTouch.read(PIN_TOUCH, 500);
}

uint32_t blinkUntil = 0;      // how long to blink
uint32_t blinkNext = 0;       // when to blink on/off next
int blinkState = 0;           // current blinking state (0=off)
uint32_t debounceWait = 0;    // to catch & ignore flakey presses

// the loop function runs over and over again, forever

void loop() {
  // read touch pin and wait for a sign
  int pinValue = ADCTouch.read(PIN_TOUCH) - referenceValue;   // get touch value
  if (pinValue > 100) {                      // yay, someone touched it!
    if (millis() > debounceWait) {           // sometimes the touch is flakey, just trigger once every 300ms
      debounceWait = millis() + 300;
      // Serial.println("Touch registered"); // sent to serial port for debugging, not really needed

      // do our thing -- send CTRL-1
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('1');
      delay(100);
      Keyboard.releaseAll(); 

      // tell the light to blink for 2 seconds to show that we're working
      blinkUntil = millis() + 2000;
      blinkNext = 0;
    }
  }

  // if someone wants me to blink, then blink
  if (blinkUntil) {
    if (blinkUntil>millis()) { 
      if (millis()>blinkNext) {
        blinkState = 1-blinkState;
        digitalWrite(LED_PIN, (blinkState==0)?LOW:HIGH );   // turn the LED on/off
        blinkNext = millis() + 100;    // blink in 200ms interval
      }      
    } else {                           // can we stop blinking now?
      digitalWrite(LED_PIN, HIGH );    // turn the LED off
      blinkUntil = 0;      
    }
  }
}
