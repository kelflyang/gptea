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



// Function declarations
void fetch_and_store_image();
void fetchEffectTask(void * parameter);
void displayPassiveScreen();
void displayBlinkingRedScreen();
void displayFadingOrangeRing();
void displayWobblyCircle();
void displayLoadingAnimation();
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
void drawName(String text);
void connectToWiFi();


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
void setup() {
  Serial.begin(115200);
  tft.init();

  // Connect to Wi-Fi
  connectToWiFi();

  // SD card init
  Serial.println("\n\n Testing TJpg_Decoder library");
  pinMode(D2, OUTPUT);
  // Initialise SD before TFT
  if (!SD.begin(D2)) {
    Serial.println(F("SD.begin failed!"));
    while (1) delay(0);
  }
  tft.fillScreen(0x051F); // blue
  Serial.println("\r\nInitialisation done.");
  delay(1000);

  // TFT init
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);

  // Create a task for fetching the effect value
  xTaskCreate(
    fetchEffectTask,       // Function that implements the task
    "FetchEffectTask",     // Name of the task
    4096,                  // Stack size (in words)
    NULL,                  // Task input parameter
    1,                     // Priority of the task
    NULL                   // Task handle
  );
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

int currentEffect = -1;   // Store the current effect
int previousEffect = -1;  // Store the previous effect


void loop() {
  // Run the current effect animation
  if (currentEffect != previousEffect) {
    previousEffect = currentEffect; // Update the previous effect only on change
    switch (currentEffect) {
      case 5: //show image
        TJpgDec.drawSdJpg(0, 0, "/new.jpg"); // Display the fetched image
        break;
        
      case 3: //pipeline2 passive
        displayPassiveScreen();
        break;
        
      case 7: //recording
        displayBlinkingRedScreen();
        break;
        
      case 9: //speaking
        displayWobblyCircle();
        break;

      case 11: //pipeline1 loading
        displayLoadingAnimation();
        break;
        
      default:
        Serial.println("Unknown effect.");
        break;
    }
  } else {
    // Continue running animations for cases other than 5 and 3, static
    switch (currentEffect) {
      case 3: //pipeline2 passive
        displayPassiveScreen();
        break;
        
      case 7: //recording
        displayBlinkingRedScreen();
        break;
        
      case 9: //speaking
        displayWobblyCircle();
        break;

      case 11: //pipeline1 loading
        displayLoadingAnimation();
        break;
        
      default:
        break;
    }
  }

  delay(10); // Small delay to avoid watchdog reset
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


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


void fetchEffectTask(void * parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(EFFECT_URL);
      int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          int effectValue = doc["effect"];
          if (effectValue != currentEffect) {
            currentEffect = effectValue;
            Serial.print("Effect value fetched: ");
            Serial.println(effectValue);
            if (effectValue == 5) {
              fetch_and_store_image();
            }
          }
        } else {
          Serial.print("Failed to parse JSON: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.println("HTTP request for effect failed.");
      }

      http.end();
    }
    delay(1000); // Fetch effect every 1 second
  }
}

void displayPassiveScreen() {
  int steps = 100; // Number of steps in the gradient
  int delayTime = 10; // Delay between updates to slow down the transition
  
  // Define start and end colors in RGB 16-bit format
  uint16_t startColor = 0xFFFF; // White
  uint16_t endColor = 0x76FF; // Light Sky Blue

  // Extract RGB components for both start and end colors
  int startR = (startColor >> 11) & 0x1F;
  int startG = (startColor >> 5) & 0x3F;
  int startB = startColor & 0x1F;

  int endR = (endColor >> 11) & 0x1F;
  int endG = (endColor >> 5) & 0x3F;
  int endB = endColor & 0x1F;

  // Transition from startColor to endColor
  for (int i = 0; i <= steps; i++) {
    // Calculate intermediate color
    int red = startR + (endR - startR) * i / steps;
    int green = startG + (endG - startG) * i / steps;
    int blue = startB + (endB - startB) * i / steps;

    // Convert RGB back to 16-bit color
    uint16_t color = (red << 11) | (green << 5) | blue;

    // Update display
    tft.fillScreen(color);
    delay(delayTime);
  }

  // Transition back from endColor to startColor
  for (int i = steps; i >= 0; i--) {
    // Calculate intermediate color
    int red = startR + (endR - startR) * i / steps;
    int green = startG + (endG - startG) * i / steps;
    int blue = startB + (endB - startB) * i / steps;

    // Convert RGB back to 16-bit color
    uint16_t color = (red << 11) | (green << 5) | blue;

    // Update display
    tft.fillScreen(color);
    delay(delayTime);
  }
}

void displayBlinkingRedScreen() {
  static bool state = false;
  state = !state;
  tft.fillScreen(state ? TFT_RED : TFT_BLACK);
  delay(500); // Adjust delay for blinking effect
}


void displayWobblyCircle() {
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int radius = 70;
  int numVertices = 36; // Number of vertices for the polygon
  float angleIncrement = 2 * PI / numVertices;
  float noiseScale = 0.2; // Scale for Perlin noise

  static int frame = 0;
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
  frame += 1;
  delay(10);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayLoadingAnimation() {
  static int frame = 0;
  int numFrames = 12;  // Number of frames for a full rotation
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int radius = min(centerX, centerY);  // Radius of the circle
  int innerLimit = 30;  // Distance from center to start the line
  int thickness = 10;   // Desired thickness of the line
  float angleIncrement = 360.0 / numFrames;

  // Calculate the angle for the current frame
  float angle = radians(frame * angleIncrement);
  float normalAngle = angle + PI / 2;  // Normal to the line angle

  // Clear the previous lines
  tft.fillScreen(TFT_BLACK);

  // Draw the lines offset from each other to create thickness
  for (int i = -thickness / 2; i <= thickness / 2; i += 2) {  // Adjust step for finer control
    // Adjust x and y start and end points slightly for each line
    int xOffset = i * cos(normalAngle);
    int yOffset = i * sin(normalAngle);

    int xStart = centerX + innerLimit * cos(angle) + xOffset;
    int yStart = centerY + innerLimit * sin(angle) + yOffset;
    int xEnd = centerX + radius * cos(angle) + xOffset;
    int yEnd = centerY + radius * sin(angle) + yOffset;

    tft.drawLine(xStart, yStart, xEnd, yEnd, TFT_WHITE);
  }

  // Update the frame for the next iteration
  frame = (frame + 1) % numFrames;
  delay(200);
}
