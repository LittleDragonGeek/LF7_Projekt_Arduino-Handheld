// Import von h-Dateien (Klassen)
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Definiere Pins an ESP32/Arduino
#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_CS    19
#define TFT_RST   21
#define TFT_DC    22

#define BUTTON_RIGHT  14  // A
#define BUTTON_DOWN   15  // B
#define BUTTON_UP     17  // X
#define BUTTON_LEFT   16  // Y

// Display: 128x160
SPIClass tftSPI = SPIClass(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST); // SPI, CS, DC, RST

void setup() {
  Serial.begin(9600);

  tftSPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); // SCK, MISO, MOSI, CS

  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(40000000);
}

void setBricks(uint16_t color1) {
  tft.setRotation(1);
  tft.fillScreen(ST77XX_WHITE);
  tft.fillRect(5, 50, 10, 5, color1);
  tft.fillRect(20, 50, 10, 5, color1);
  tft.fillRect(35, 50, 10, 5, color1);
  tft.fillRect(50, 50, 10, 5, color1);
  tft.fillRect(65, 50, 10, 5, color1);
  tft.fillRect(80, 50, 10, 5, color1);
  tft.fillRect(95, 50, 10, 5, color1);
  tft.fillRect(110, 50, 10, 5, color1);
  tft.fillRect(125, 50, 10, 5, color1);
  tft.fillRect(140, 50, 10, 5, color1);
  tft.fillRect(110, 50, 10, 5, color1);
}

void setPlayer(uint16_t color2) {
  tft.setRotation(1);
  tft.fillRect(40, 115, 30, 5, color2);
}

void loop() {
  setBricks(ST77XX_BLUE);
  setPlayer(ST77XX_ORANGE);
  int posY = 40 + 60*(millis()%1000)/1000.0f;
  int posX = 40 + 60*(millis()%1000)/1000.0f;
  tft.fillCircle(posX, posY, 3, ST77XX_BLACK);
  tft.setCursor(30, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.print("Breaking Out Inc.");
  delay(30);
}