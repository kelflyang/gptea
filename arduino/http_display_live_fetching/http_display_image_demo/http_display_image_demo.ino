#include <TJpg_Decoder.h>

// Include SD
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
#include <WiFi.h>                  // Wi-Fi connectivity
#include <HTTPClient.h>

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

#define SD_CS   2
#define WIFI_SSID "tangatory"       // Change to your Wi-Fi SSID
#define WIFI_PASSWORD "kevinisthegoat" // Change to your Wi-Fi password
#define IMAGE_URL "https://gptea-ten.vercel.app/api/load_image"


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void drawName(String text){
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(3);
  int16_t x_center = (tft.width() - tft.textWidth(text)) / 2;
  int16_t y_center = (tft.height() / 2) - 4;
  tft.drawString(text, x_center, y_center);
}


void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Try for a certain number of times
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected.");
    tft.fillScreen(0x03E0); //green
    drawName("Wifi ✓");
    delay(1000);
  } else {
    Serial.println("\nWi-Fi connection failed.");
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



void setup()
{
  Serial.begin(115200);
  tft.init();

  // Connect to Wi-Fi
  connectToWiFi();

  //SD card init
  Serial.println("\n\n Testing TJpg_Decoder library");
  pinMode(D2, OUTPUT);
  // Initialise SD before TFT
  if (!SD.begin(D2)) {
    Serial.println(F("SD.begin failed!"));
    while (1) delay(0);
  }
  tft.fillScreen(0x051F); //blue
  Serial.println("\r\nInitialisation done.");
  delay(1000);

  //tft init
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) { // Check Wi-Fi status
    connectToWiFi(); // Attempt to reconnect
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Fetching...");
    fetch_and_store_image();
    Serial.println("Display image now");
    TJpgDec.drawSdJpg(0, 0, "/new.jpg");
  } else {
    Serial.println("Skipping image fetch due to Wi-Fi issues.");
  }

  delay(5000); // Repeat every 5 seconds
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void fetch_and_store_image() {
  HTTPClient http;
  http.begin(IMAGE_URL); // Set the URL to fetch
  int httpCode = http.GET(); // Make the GET request

  if (httpCode == HTTP_CODE_OK) {
    // If the request is successful, get the image data
    File file = SD.open("/new.jpg", FILE_WRITE); // Open the SD card file for writing
    if (file) {
      http.writeToStream(&file); // Write the HTTP response to the SD card
      file.close(); // Close the file
      Serial.println("Image fetched and saved to SD card.");
    } else {
      Serial.println("Failed to open file on SD card.");
    }
  } else {
    Serial.println("HTTP request failed.");
  }

  http.end(); // Close the HTTP connection
}
