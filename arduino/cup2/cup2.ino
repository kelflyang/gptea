#include <TJpg_Decoder.h>
#include <ArduinoJson.h> 
#include <FastLED.h>

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
#define EFFECT_URL "https://gptea-ten.vercel.app/api/image_effect"


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

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

int previousEffect = -1; // Store the previous effect


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (WiFi.status() == WL_CONNECTED) {
     int effect = fetchEffect();
    if (effect != previousEffect) {
      previousEffect = effect; // Update the previous effect only on change
      switch(effect) {
           case 3: //testing
            displayWobblyCircle();
            break;
          
//        case 5:
//          Serial.println("Fetching image...");
//          fetch_and_store_image();
//          Serial.println("Displaying image...");
//          TJpgDec.drawSdJpg(0, 0, "/new.jpg");
//          break;
//        case 3:
//          tft.fillScreen(TFT_WHITE);
//          break;
//        case 7:
//          displayBlinkingRedScreen();
//          break;
//        case 9:
//          displayWobblyCircle();
//          break;
//        case 11:
//          displayWobblyCircle();
//          break;
        default:
          Serial.println("Unknown effect.");
          break;
      }
    } else {
      // Continue running animations for cases other than 5
      switch(effect) {
        
         case 3: //testing
          displayWobblyCircle();
          break;
          
//        case 7:
//          displayBlinkingRedScreen();
//          break;
//        case 9:
//          displayWobblyCircle();
//          break;

        default:
          break;
      }
    }
  } else {
    Serial.println("Skipping operations due to Wi-Fi issues.");
  }

  delay(500); // Repeat every 1 seconds
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

int fetchEffect() {
  HTTPClient http;
  http.begin(EFFECT_URL);
  int httpCode = http.GET();

  int effectValue = 0; // Default to 0 if there's an issue

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      effectValue = doc["effect"];
      Serial.print("Effect value fetched: ");
      Serial.println(effectValue);
    } else {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.println("HTTP request for effect failed.");
  }

  http.end();
  return effectValue;
}



void displayBlinkingRedScreen() {
  
  tft.fillScreen(TFT_BLACK);
  delay(400);
  tft.fillScreen(TFT_RED);
  delay(100);
}



void displayFadingOrangeRing() {
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int radius = 100;
  int thickness = 20; // Adjust this value to change the thickness of the ring


  for (int i = 255; i >= 0; i -= 5) {
    tft.fillScreen(TFT_BLACK);
    uint16_t color = tft.color565(i, (i * 165 / 256), 0); // Smoothly change color from orange to black
    for (int t = 0; t < thickness; t++) {
      tft.drawCircle(centerX, centerY, radius + t, color);
    }
    delay(10);
  }

  
  for (int i = 0; i < 256; i += 5) {
    tft.fillScreen(TFT_BLACK);
    uint16_t color = tft.color565(i, (i * 165 / 256), 0); // Smoothly change color from black to orange
    for (int t = 0; t < thickness; t++) {
      tft.drawCircle(centerX, centerY, radius + t, color);
    }
    delay(10);
  }
  
}



void displayWobblyCircle() {
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int radius = 100;
  int numVertices = 36; // Number of vertices for the polygon
  float angleIncrement = 2 * PI / numVertices;
  float noiseScale = 0.2; // Scale for Perlin noise

  for (int frame = 0; frame < 360; frame += 10) {
    tft.fillScreen(TFT_BLACK);
    
    float firstX = centerX + (radius + (inoise8(0, frame * noiseScale * 255) / 255.0) * 20) * cos(0);
    float firstY = centerY + (radius + (inoise8(0, frame * noiseScale * 255) / 255.0) * 20) * sin(0);
    float prevX = firstX;
    float prevY = firstY;
    
    for (int i = 1; i <= numVertices; i++) {
      float angle = i * angleIncrement;
      float noise = inoise8(i * noiseScale * 255, frame * noiseScale * 255) / 255.0; // Perlin noise value between 0 and 1
      float x = centerX + (radius + noise * 20) * cos(angle);
      float y = centerY + (radius + noise * 20) * sin(angle);
      tft.drawLine(prevX, prevY, x, y, TFT_WHITE);
      prevX = x;
      prevY = y;
    }
    tft.drawLine(prevX, prevY, firstX, firstY, TFT_WHITE); // Close the loop
    delay(30);
  }
}
