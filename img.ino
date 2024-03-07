// Include the PNG decoder library
#include <PNGdec.h>
#include "panda.h" // Image is stored here in an 8-bit array
#include "okFlowey.h"

PNG png; // PNG decoder instance

#define MAX_IMAGE_WIDTH 240 // Adjust for your images

int16_t xpos = 0;
int16_t ypos = 0;

#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// Define sensor pins
const int moistureSensorPin = A0; // Moisture sensor connected to analog pin A0
const int lightSensorPin = A17;    // Light sensor (photoresistor) connected to analog pin A1

// Timing variables
unsigned long dayStartTime = millis();
unsigned long sunExposureStartTime = 0;
const unsigned long simulatedDayLength = 6000; // 1 minute to simulate 24 hours
const unsigned long requiredSunExposure = 1200; // 12 seconds to simulate 2 hours of sun

// State variables
bool plantWateredToday = false;
unsigned long totalSunExposureToday = 0;
int plantHappiness = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n Using the PNGdec library");
  // pinMode(moistureSensorPin, INPUT);
  // pinMode(lightSensorPin, INPUT);

  // Initialise the TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);

  Serial.println("\r\nInitialisation done.");
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{
 unsigned long currentMillis = millis();
 int moistureLevel = (float(analogRead(moistureSensorPin))/1023.0)*3.3;
//  int moi
 int lightLevel = (float(analogRead(lightSensorPin))/1023.0)*3.3;
 

  // Check if a simulated day has passed
  if (currentMillis - dayStartTime >= simulatedDayLength) {
    // Check if plant was watered and received enough sun
    if (!plantWateredToday || totalSunExposureToday < requiredSunExposure) {
      if(plantHappiness > -1) {
        plantHappiness--;
      }
      Serial.println("Plant happiness decreased! (plant happiness now at ");
      Serial.println(plantHappiness);
      Serial.println(")");
    }
    else {
      if(plantHappiness < 1) {
        plantHappiness++;
      }
      Serial.println("Plant happiness increased! (plant happiness now at ");
      Serial.println(plantHappiness);
      Serial.println(")");
    }
    // if (totalSunExposureToday < requiredSunExposure) {
    //  Serial.println("Plant did not receive enough sun today!");
    // }

    if(plantHappiness == -1) {
      sadflowy();
      Serial.println("Angry Flowey");
    } else {
      happyflowy();
      Serial.println("Happy Flowey");
    }
    
    // Reset for next day
    dayStartTime = currentMillis;
    plantWateredToday = false;
    totalSunExposureToday = 0;
  }

  // Check soil moisture
  Serial.println("Soil is dry, water the plant!");
  Serial.println(moistureLevel);
  if (moistureLevel >= 8) { // Threshold for dry soil
    // Serial.println("Soil is dry, water the plant!");
  } else {
    plantWateredToday = true;
  }

  // Check sun exposure
  if (lightLevel > 8) { // Threshold for sufficient light
    if (sunExposureStartTime == 0) { // Start timing sun exposure
      sunExposureStartTime = currentMillis;
    } else if (currentMillis - sunExposureStartTime >= requiredSunExposure) {
      totalSunExposureToday += currentMillis - sunExposureStartTime;
      sunExposureStartTime = 0; // Reset sun exposure timer
    }
  } else {
    sunExposureStartTime = 0; // Reset if light level drops below threshold
  } 
}

void happyflowy(){
  delay(100);
  int16_t rc = png.openFLASH((uint8_t *)panda, sizeof(panda), pngDraw);
  if (rc == PNG_SUCCESS) {
    // Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    // png.close(); // not needed for memory->memory decode
  }
}
void okflowy(){
  delay(100);
  int16_t rc = png.openFLASH((uint8_t *)okFlowey, sizeof(okFlowey), pngDraw);
  if (rc == PNG_SUCCESS) {
    // Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    // png.close(); // not needed for memory->memory decode
  }
}
void sadflowy(){
  delay(100);
  int16_t rc = png.openFLASH((uint8_t *)sadFlowey, sizeof(sadFlowey), pngDraw);
  if (rc == PNG_SUCCESS) {
    // Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    // png.close(); // not needed for memory->memory decode
  }
}



//=========================================v==========================================
//                                      pngDraw
//====================================================================================

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}
