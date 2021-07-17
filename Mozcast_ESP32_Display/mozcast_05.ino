/*
 * Fetch & display the Mozcast weather report 
 * Just being weird, you can be too.
 * 
 * By johnmu / 2021-07-17 / Apache-2.0 License
 */

/* 
 *  Requires:
 *    ESP32 Lilypad TTGO with T-Display (can be adapted to others easily though)
 *      For example: https://s.click.aliexpress.com/e/_AaToZG 
 *      (currently $10-25; chips are a bit more expensive than they used to be)
 *    
 *  Libraries (all in Arduino too):
 *    TFT_eSPI - https://github.com/Bodmer/TFT_eSPI
 *    ESP_DoubleResetDetector - https://github.com/khoih-prog/ESP_DoubleResetDetector
 *    ESP_WiFiManager - https://github.com/khoih-prog/ESP_WiFiManager
 *
 *  Configuration:
 *    1. Edit TFT_eSPI User_Setup_Select.h:
 *       - add comment to //#include <User_Setup.h>
 *       - uncomment #include <User_Setups/Setup25_TTGO_T_Display.h> 
 *    2. Install ESP32 boards in Arduino ( https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/ )
 *    3. Select board in Arduino: ESP32 -> TTGO LoRa32-OLED V1 (others probably work too)
 *    4. Compile & upload
 *    
 */
 
#include <WiFi.h>
#include <HTTPClient.h> 
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(135, 240);    // TFT display, 135x240 resolution, using User_Setup_Select.h

unsigned long nextTime = 0;
unsigned long timerDelay = 600000L;   // Refresh every 10 minutes


#include <ESP_WiFiManager.h>          // https://github.com/khoih-prog/ESP_WiFiManager
#define USE_SPIFFS            true
#define ESP_DRD_USE_EEPROM    true
#define DRD_TIMEOUT             10
#define DRD_ADDRESS             0
#include <ESP_DoubleResetDetector.h>  // https://github.com/khoih-prog/ESP_DoubleResetDetector
DoubleResetDetector* drd;
bool      initialConfig = false;

void setup() {
  Serial.begin(115200); // Set up serial port to 115200 Baud for debugging (not needed)

  tft.init(); // initialize display, show status updates
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Connecting ...", 10, 10, 4);

  /* This sets up a wifi portal on the first setup. Connect to it, and add your wifi credentials there. 
   * Using reset twice within 10 seconds will return to this setup should you want to change the wifi setup.
   */
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  if (drd->detectDoubleReset()) { Serial.println(F("DRD")); initialConfig = true; }
  ESP_WiFiManager ESP_wifiManager("ConfigOnDoubleReset");
  if (ESP_wifiManager.WiFi_SSID() == "") { Serial.println(F("No AP credentials")); initialConfig = true; }
  if (initialConfig) {
    String AP_SSID = "ESP_AP_CONFIG";
    tft.fillScreen(TFT_BLACK); tft.setCursor(1,10); tft.setTextFont(4);
    tft.print("Set up with wifi access point: \n" + AP_SSID);
    Serial.println(F("Starting Config Portal"));
    if (!ESP_wifiManager.startConfigPortal(AP_SSID.c_str(), NULL)) { Serial.println(F("Not connected to WiFi")); }
    else { Serial.println(F("connected")); }
  } else { WiFi.mode(WIFI_STA); WiFi.begin(); } 
  unsigned long startedAt = millis();
  Serial.print(F("After waiting "));
  int connRes = WiFi.waitForConnectResult();
  float waited = (millis() - startedAt);
  Serial.print(waited / 1000); Serial.print(F(" secs , Connection result is ")); Serial.println(connRes);
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println(F("Failed to connect"));     
    tft.fillScreen(TFT_RED); tft.setCursor(1,10); tft.setTextFont(4);
    tft.print("WIFI failed.\nRebooting ...");
    uint32_t countdown = millis() + DRD_TIMEOUT*1000 + 2000; 
    while (millis() < countdown) { delay(10); }
    Serial.println("Rebooting.");
    ESP.restart();
  } else { Serial.print(F("Local IP: ")); Serial.println(WiFi.localIP()); }

  // wifi is now connected, get on with life.
  
  tft.fillScreen(TFT_BLACK); tft.drawString("Loading ...", 10, 10, 4);
  nextTime = millis() + 500; // get the first update in 0.5 seconds
}

/* Extract a value from a JSON string -- super-basic, error-prone
 */
String simple_json_value(String inputs, String varname, bool isString) {
  String find = "\"" + varname + "\"";
  String result;
  int index_found = inputs.indexOf(find);
  if (index_found != -1) {
    int name_end = index_found + find.length() + 1;
    int index_end_1 = inputs.indexOf("}", name_end);
    int index_end_2 = inputs.indexOf(",", name_end);
    if ((index_end_1 == -1) && (index_end_2 == -1)) {
      result = inputs.substring(name_end);
    } else {
      if ((index_end_2 < index_end_1) && (index_end_2 != -1)) {
        result = inputs.substring(name_end, index_end_2);
      } else {
        result = inputs.substring(name_end, index_end_1);
      }
    }
  }
  if (result) { if (isString) result.replace("\"", ""); } // lazy quote removal
  return result;
}

/* Fetch weather report HTML page, parse it
 * We have to fetch it in chunks since the HTML page is quite large.
 * We combine the chunks once we know the data has started.  
 * Arduino does weird things when you try to simplify String() code, so
 * some of this is a bit awkward. Someone could port it to use clean
 * c++ char[] arrays, but it works. As long as Moz doesn't change things.
 * The data we need is conveniently in JSON-LD data on the page.
 * We just roughly search for it, and save the result in a global variable.
 * (I'm lazy, we have enough CPU & memory, so deal with it.)
 */
String scraped_weather; // global variable for JSON string since we can't return String()
String server_date;
bool check_weather() {
  if (WiFi.status() != WL_CONNECTED) return false;
  bool worked = false;
  scraped_weather = ""; // reset global
  server_date = "";

  HTTPClient http;

  const char * headerKeys[] = {"date" }; // we'll grab this HTTP response header
  const size_t numberOfHeaders = 1;

  String weather_url = "https://moz.com/mozcast";
  Serial.print("URL: "); Serial.println(weather_url);
  http.begin(weather_url.c_str()); 
  http.collectHeaders(headerKeys, numberOfHeaders); 
  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    // create buffer for read
    #define BUFSIZE 128
    #define MAX_ROUND_BUFFER 300
    #define SCRAPE_SIZE 1024
    #define MY_HTTP_TIMEOUT 2000

    int scrape_more = -1; // -1 = not yet, >0 = bytes to get, 0 = done
    String scraped;
    uint32_t http_timeout = millis() + MY_HTTP_TIMEOUT;
    uint8_t input_buffer[BUFSIZE+2];
    String stream_buffer;
    String round_buffer;
    String DATA_START = "weather: [";

    int len = http.getSize(); // get length of document (or -1)
    WiFiClient * stream = http.getStreamPtr();    
    server_date = http.header("date");

    // read all data from server
    while (http.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available(); // get available data size

      if (size) {
        int c = stream->readBytes(input_buffer, ((size > BUFSIZE) ? BUFSIZE : size));
        input_buffer[c] = 0; // terminate buffer for string conversion
        stream_buffer = (const char *)input_buffer;
        round_buffer += stream_buffer;
        if (scrape_more==-1) { // if we haven't found the start
          if (round_buffer.indexOf(DATA_START)>0) {
            scraped = round_buffer.substring(round_buffer.indexOf(DATA_START));
            scrape_more=1;
          }
        } else if (scrape_more==1) { // we have the start, get all data
          scraped += stream_buffer;
          if (scraped.length()>SCRAPE_SIZE) {
            scraped_weather = scraped; // save to global to return value
            scrape_more=0; // we're done
            worked = true;
            stream->stop(); // no need to process rest
          }
        }
        // truncate round buffer
        if (round_buffer.length()>MAX_ROUND_BUFFER) round_buffer = round_buffer.substring(round_buffer.length()-MAX_ROUND_BUFFER);
        http_timeout = millis() + MY_HTTP_TIMEOUT;
      } else if (millis() > http_timeout) stream->stop(); 
      delay(1); // give it a ms to get new data
    }
  }

  http.end(); // Free resources

  return worked;
}

// Display weather based on global variable JSON
void display_weather() {
  Serial.print("Checked: "); Serial.println(server_date);
  
  String data_date = simple_json_value(scraped_weather, "dateStr", true);
  Serial.print("date="); Serial.println(data_date);

  String data_temp_now = simple_json_value(scraped_weather, "temp", false);
  Serial.print("temp="); Serial.println(data_temp_now);
  
  String data_temp_was = simple_json_value(scraped_weather.substring(scraped_weather.indexOf("\"temp\"")+1), "temp", false);
  Serial.print("previous temp="); Serial.println(data_temp_was);
  
  String data_icon= simple_json_value(scraped_weather, "icon", true);
  Serial.print("icon="); Serial.println(data_icon);

  String data_wx_name = data_icon.substring(data_icon.lastIndexOf("/")+1, data_icon.indexOf("."));
  Serial.print("wx="); Serial.println(data_wx_name);

  int int_temp_now = data_temp_now.toInt();
  int int_temp_was = data_temp_was.toInt();
  int temp_diff = int_temp_now - int_temp_was;

  // update TFT
  int i, ix, iy;
  int ix_margin = 5;
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  i = tft.drawString(data_temp_now, ix_margin, 1, 6);
  tft.drawString("F", ix_margin+i+10, 10, 4);
  tft.drawString(data_date, 120, 10, 4);
  tft.drawString("It's " + data_wx_name + ".", ix_margin, 50, 4);

  tft.setTextColor(TFT_GREEN);
  ix = ix_margin; iy = 85;
  if (temp_diff>0) ix += tft.drawString("up from ", ix, iy, 4);
  else if (temp_diff<0) ix += tft.drawString("down from ", ix, iy, 4);
  else  ix += tft.drawString("previously ", ix, iy, 4);
  tft.drawString(data_temp_was + " F", ix, iy, 4);

  tft.setTextColor(0x7BEF); // dark grey
  tft.drawString(server_date, ix_margin, 118, 2);

}

void loop() {
  drd->loop(); // log for double-reset detector

  // only fetch weather every now and then, it updates daily anyway
  if (millis() > nextTime) {
    String *result;
    bool worked = check_weather();
    if (worked) display_weather();
    nextTime = millis() + (worked?timerDelay:5000);
  }
}

// https://www.youtube.com/watch?v=VScSEXRwUqQ
