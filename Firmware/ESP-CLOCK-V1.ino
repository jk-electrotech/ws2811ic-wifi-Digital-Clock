//including librarys
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// --- Hardware Setup ---
#define PIN        4   // D2 on ESP8266
#define NUM_ICS    10  //Number Of Ic Use
#define BRIGHTNESS 120 // Brightness Control (0-255)

Adafruit_NeoPixel pixels(NUM_ICS, PIN, NEO_RGB + NEO_KHZ800);

// --- Time Setup ---
WiFiUDP ntpUDP;
// Offset 19800 = India Standard Time (+5:30)
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

// Segment (A, B, C, D, E, F, G)
const byte digitTable[10][7] = {
  {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
};

// Global for blinking colon led
unsigned long lastStatusMilli = 0;
bool statusState = false;

// Function to control only the Colon for Status Feedback
void setColonOnly(bool state) {
  pixels.clear(); 
  // IC 5 Pin 3 (Bottom) and IC 6 Pin 1 (Top)
  pixels.setPixelColor(4, pixels.Color(0, 0, state ? 255 : 0)); // Blue Pin IC 5
  pixels.setPixelColor(5, pixels.Color(state ? 255 : 0, 0, 0)); // Red Pin IC 6
  pixels.show();
}

// Function to draw the full clock display
void updateClock(int h, int m, bool colon) {
  bool s[30] = {0}; 
  int p = 0;

  // Mapping: Digit 4 -> Digit 3 -> Colon -> Digit 2 -> Digit 1
  for(int i=0; i<7; i++) s[p++] = digitTable[m % 10][i]; // D4 (Rightmost)
  for(int i=0; i<7; i++) s[p++] = digitTable[m / 10][i]; // D3
  s[p++] = colon; // Bottom dot (IC 5 Blue)
  s[p++] = colon; // Top dot (IC 6 Red)
  for(int i=0; i<7; i++) s[p++] = digitTable[h % 10][i]; // D2
  for(int i=0; i<7; i++) s[p++] = digitTable[h / 10][i]; // D1 (Leftmost)

  for(int i=0; i<10; i++) {
    pixels.setPixelColor(i, pixels.Color(s[i*3]?255:0, s[i*3+1]?255:0, s[i*3+2]?255:0));
  }
  pixels.show();
}

// Runs when WiFiManager enters AP Mode (Phone Setup)
void configModeCallback (WiFiManager *myWiFiManager) {
  setColonOnly(true); // Solid ON means "Ready for phone connection"
}

void setup() {
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.clear();
  pixels.show();

  WiFiManager wm;
  
  // Set behavior: Solid colon when in AP mode
  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalTimeout(180); // 3 minutes timeout

  // Visual: Rapid blink while searching for saved WiFi
  // I use a temporary non-blocking loop to show searching status
  WiFi.begin(); 
  int connectCounter = 0;
  while (WiFi.status() != WL_CONNECTED && connectCounter < 40) {
    statusState = !statusState;
    setColonOnly(statusState);
    delay(150);
    connectCounter++;
  }

  // If not connected by now, trigger the AP portal
  if (WiFi.status() != WL_CONNECTED) {
    if (!wm.startConfigPortal("ESP-CLOCK")) {
      delay(3000);
      ESP.restart();
    }
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();
  
  int h = timeClient.getHours();
  int m = timeClient.getMinutes();
  
  // Convert 24h to 12h
  if (h > 12) h -= 12;
  if (h == 0) h = 12;

  // Blink colon based on even/odd seconds
  bool colonBlink = (timeClient.getSeconds() % 2 == 0);
  
  updateClock(h, m, colonBlink);
  
  delay(500); // Check time twice a second for accuracy
}