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

#define JOYSTICK_Y    34
#define JOYSTICK_X    35
#define JOYSTICK_B    32

// Andere Konstanten
// Originale Display Größe: 128x160 | Invertiert: WIDTH = 160, 
const int TFT_WIDTH = 160;
const int TFT_HEIGH = 128;

// Globale Variablen
int playerPosX = (TFT_WIDTH / 2) - (30 / 2); // Mittelpunkt im Spielfeld - Initial

int inDirectionX = 0; // neue Spieler-Pos X?

int ballPosX = (TFT_WIDTH / 2) - 5;
int ballPosY = 115;

int playerX1Coll = playerPosX;
int playerX2Coll = playerPosX + 30;

//
SPIClass tftSPI = SPIClass(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST); // SPI, CS, DC, RST

// Definiere Input- und Outputpins
void setup() {
  Serial.begin(9600);

  tftSPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); // SCK, MISO, MOSI, CS

  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(40000000);

  // Pulldown-Mode beim ESP32 einschalten
  pinMode(17, INPUT_PULLDOWN);
  pinMode(16, INPUT_PULLDOWN);
  pinMode(15, INPUT_PULLDOWN);
  pinMode(14, INPUT_PULLDOWN);
}

// Hauptschleife
void loop() {
  // Lese Pins
  //Serial.println(analogRead(JOYSTICK_X));
  inDirectionX = 4095 - analogRead(JOYSTICK_X);
  //Serial.println(analogRead(JOYSTICK_Y));
  //Serial.println(analogRead(JOYSTICK_B));

  updateDisplay();
  drawBricks(ST77XX_BLUE);
  drawPlayer(ST77XX_BLACK);
  calcPlayerCollider();
  drawBall();

  delay(30);
}

// Funktionen
void updateDisplay() {
  tft.setRotation(1);
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(30, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.print("Breaking Out Inc.");
}

void drawBricks(uint16_t color) {
  tft.fillRect(5, 50, 10, 5, color);
  tft.fillRect(20, 50, 10, 5, color);
  tft.fillRect(35, 50, 10, 5, color);
  tft.fillRect(50, 50, 10, 5, color);
  tft.fillRect(65, 50, 10, 5, color);
  tft.fillRect(80, 50, 10, 5, color);
  tft.fillRect(95, 50, 10, 5, color);
  tft.fillRect(110, 50, 10, 5, color);
  tft.fillRect(125, 50, 10, 5, color);
  tft.fillRect(140, 50, 10, 5, color);
  tft.fillRect(110, 50, 10, 5, color);
}

void drawBall() {
  //int posY = 50 + 60*(-millis()%1000)/1000.0f;
  //int posX = 50 + 60*(-millis()%1000)/1000.0f;
  tft.fillCircle(ballPosX, ballPosY, 3, ST77XX_RED);
}

void drawPlayer(uint16_t color) {
  tft.setRotation(1);

  // Setzt den Spieler wieder in die Mitte des Spielfelds zurück
  playerPosX = (TFT_WIDTH / 2) - (30 / 2);

  if (inDirectionX < 1800 || inDirectionX > 1900)
    playerPosX = 1 + ((4096 - inDirectionX) / 32); //1 + ((4096 - inDirectionX) / 80) * (millis()%tValue)/(tValue * 1.0f)
  tft.fillRect(playerPosX, 120, 30, 5, color);
}

void playerControl() {
  // X < 1500 = -x
  // X > 2500 = +x
  // Bewegung bis zur Koordinate x = 1
  int posX = 1 + 0*(-millis()%1000)/1000.0f;
  //tft.fillRect(posX, 120, 30, 5, color);
}

// Berechnet die zwei oberen x-Eckpunkte, um die Hitbox des Spielers zu ermitteln
void calcPlayerCollider() {
  playerX1Coll = playerPosX;
  playerX2Coll = playerPosX + 30;
  Serial.println(playerX1Coll);
  Serial.println(playerX2Coll);
}