/**
 * Turn the ESP32 into a Bluetooth LE keyboard and 
 * sends ctrl+# clicks when buttons are pressed.
 * Handle appropriately on the computer, if you want to do something with that.
 */

#include <BleKeyboard.h>

// pick a name for your keyboard
BleKeyboard bleKeyboard("MeetBoard");

// number of buttons you have &
// the pins where you have buttons connected (on ESP32, must be <34)
#define NUM_BUTTONS 3
const int BUTTON_PINS[] = {32, 25, 33};

// global variables because why not
uint32_t button_press_wait_until = 0;
bool button_is_pressed = false;
bool keyboard_connected = false;
#define DEBOUNCE_TIME 500

void setup() {
  Serial.begin(115200);
  // Some info so that you know which version you're running
  Serial.println(__FILE__);
  Serial.print("Compiled "); Serial.print(__DATE__); Serial.print(" "); Serial.println(__TIME__);

  Serial.println("Starting BLE");
  bleKeyboard.begin();

  // set up button pins as digital inputs with pull-up resistors enabled 
  // on ESP32, only works for pins <34
  for (int i=0; i<NUM_BUTTONS; i++) pinMode(BUTTON_PINS[i], INPUT_PULLUP);

  Serial.println("Ready.");
}

void loop() {
  if(bleKeyboard.isConnected()) {
    
    // check to see if we're now connected to the computer
    if (!keyboard_connected) { 
      Serial.println("Keyboard connected."); 
      keyboard_connected=true; 
    }

    // check if any of the buttons are pressed. one at a time, please.
    int button_nr = -1;
    for (int i=0; i<NUM_BUTTONS; i++) {
      if (digitalRead(BUTTON_PINS[i]) == 0) button_nr = i;
    }    
    
    if (button_nr != -1) { 
      if ((!button_is_pressed) && (millis()>button_press_wait_until)) {
        button_is_pressed = true;
        button_press_wait_until = millis() + DEBOUNCE_TIME;
        
        Serial.print("Pressed: "); Serial.print(button_nr); Serial.println();

        // send ctrl + button-number to computer
        bleKeyboard.press(KEY_LEFT_CTRL); 
        bleKeyboard.write(0x31 + button_nr); // sends ^1, ^2, ^3, etc.
        delay(100);
        bleKeyboard.releaseAll();
        
      } // else skip (it's still pressed from before)
    } else {
      if (button_is_pressed) button_is_pressed = false;
    }
  } else {
    if (keyboard_connected) {
      // looks like we lost the computer. bummer. will try reconnecting. 
      Serial.println("Keyboard disconnected."); 
      keyboard_connected=false; 
    }
    Serial.println("Waiting for connection ...");
    delay(500);
  }
}

// EOF
