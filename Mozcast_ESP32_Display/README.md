# Displays Mozcast weather on a ESP32 board

## Requirements

 *  ESP32 Lilypad TTGO with T-Display (can be adapted to others easily though)
    For example: https://s.click.aliexpress.com/e/_AaToZG 
    (currently $10-25; chips are a bit more expensive than they used to be)
    
 *  Libraries (all in Arduino too):
 *  * TFT_eSPI - https://github.com/Bodmer/TFT_eSPI
 *  * ESP_DoubleResetDetector - https://github.com/khoih-prog/ESP_DoubleResetDetector
 *  * ESP_WiFiManager - https://github.com/khoih-prog/ESP_WiFiManager

 
## Configuration

1. Edit TFT_eSPI User_Setup_Select.h:
 - add comment to //#include <User_Setup.h>
 - uncomment #include <User_Setups/Setup25_TTGO_T_Display.h> 

2. Install ESP32 boards in Arduino ( https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/ )

3. Select board in Arduino: ESP32 -> TTGO LoRa32-OLED V1 (others probably work too)

4. Compile & upload

Arduino C++ code. 

Photo of running device:

![](photo-ttgo.png)
