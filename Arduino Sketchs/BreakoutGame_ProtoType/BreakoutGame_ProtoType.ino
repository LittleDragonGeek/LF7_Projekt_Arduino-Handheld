#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_CS    19
#define TFT_RST   21
#define TFT_DC    22

SPIClass tftSPI = SPIClass(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST); // SPI, CS, DC, RST

void setup() {
  Serial.begin(9600);

  tftSPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); // SCK, MISO, MOSI, CS

  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(40000000);
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_WHITE);
  tft.fillRect(40, 150, 30, 5, color2);
  tft.fillRect(10, 50, 5, 5, color1);
  tft.fillRect(20, 50, 5, 5, color1);
  tft.fillRect(30, 50, 5, 5, color1);
  tft.fillRect(40, 50, 5, 5, color1);
  tft.fillRect(50, 50, 5, 5, color1);
  tft.fillRect(60, 50, 5, 5, color1);
  tft.fillRect(70, 50, 5, 5, color1);
  tft.fillRect(80, 50, 5, 5, color1);
  tft.fillRect(90, 50, 5, 5, color1);
  tft.fillRect(100, 50, 5, 5, color1);
  tft.fillRect(110, 50, 5, 5, color1);
}

void loop() {
  testfillrects(ST77XX_BLUE, ST77XX_ORANGE);
  int posY = 60 + 90*(millis()%1000)/1000.0f;
  tft.fillCircle(60, posY, 3, ST77XX_BLACK);
  tft.setCursor(15, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.print("Breaking Out Inc.");
  delay(30);
}