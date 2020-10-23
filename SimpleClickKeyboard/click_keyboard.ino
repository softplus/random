
//
// SimpleClickKeyboard
//   Uses clicky button to trigger a keypress as a USB keyboard
//   Requires USB HID keyboard capable Arduino device
//   https://github.com/softplus/random/tree/master/SimpleClickKeyboard
//

// Hardware: Sparkfun Pro Micro (5v or 3.3v), or cheap clone ( https://www.aliexpress.com/wholesale?SearchText=%22pro+micro%22 )
// Pin A0 connected to one button connection
// Pin GND connected to other button connection
// 
// Uses libraries:
// Keyboard
//   generic Arduino USB keyboard library
//   https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
//

// connected stuff
#define BTN_PIN A0

// Keyboard driver for USB
#include <Keyboard.h>

#define DEBOUNCE_MS 300

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(BTN_PIN, INPUT_PULLUP);
  Serial.begin(115200); // for debug output

  Keyboard.begin();
  Serial.println("Started.");
}

uint32_t debounceWait = 0;

// the loop function runs over and over again forever
void loop() {
  int pinValue = digitalRead(BTN_PIN);
  if (pinValue == LOW) { // click!
    if (millis() > debounceWait) {
      debounceWait = millis() + DEBOUNCE_MS;
      Serial.println("Click"); // just in case anyone's listening, also blinks onboard LED
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('1');
      delay(100);
      Keyboard.releaseAll();
    }
  }
}
