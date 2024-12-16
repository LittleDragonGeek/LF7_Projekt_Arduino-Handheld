// Import von h-Dateien (Klassen)
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <string.h>

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

//
// Globale Variablen
//
int playerPosX = (TFT_WIDTH / 2) - (30 / 2); // Mittelpunkt im Spielfeld - Initial
int playerX1Coll = playerPosX;
int playerX2Coll = playerPosX + 30;

int inJYSTDirectionX = 0;

// Ball Start-Position
int ballPosX = (TFT_WIDTH / 2) - 5;
int ballPosY = 115;


bool gameIsStarted = false;
bool ballIsMoving = false;

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
  pinMode(BUTTON_RIGHT, INPUT_PULLDOWN);
  pinMode(BUTTON_DOWN, INPUT_PULLDOWN);
  pinMode(BUTTON_UP, INPUT_PULLDOWN);
  pinMode(BUTTON_LEFT, INPUT_PULLDOWN);

  // Setzt einmalig Display zurück
  tft.fillScreen(ST77XX_WHITE);

  // zeigt Startbildschirm an
  showStartScreen();
}

// Hauptschleife
void loop() {
  if (digitalRead(BUTTON_RIGHT) == 1)
  {
    gameIsStarted = true;
  }
  if (gameIsStarted == true)
  {
    updateDisplay();
    // Lese Pins
    //Serial.println(analogRead(JOYSTICK_X));
    inJYSTDirectionX = 4095 - analogRead(JOYSTICK_X);
    //Serial.println(analogRead(JOYSTICK_Y));
    //Serial.println(analogRead(JOYSTICK_B));

    drawBricks(ST77XX_BLUE);

    drawPlayer(ST77XX_BLACK);
    calcPlayerCollider();
    drawBall();

    while (ballIsMoving == false)
    {
      if (digitalRead(BUTTON_UP) == 1) 
      {
        ballIsMoving = true;
      }
      Serial.println(ballIsMoving);
    }

    delay(30);
  }
}

//
// Funktionen
//

// Allgemeine Funktionen
void showStartScreen() {
  tft.setRotation(1);
  tft.setCursor(30, (TFT_HEIGH  / 2));
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("Breaking Out Inc.");
  tft.setCursor(30, (TFT_HEIGH  / 2) + 10);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("Press BUTTON-RIGHT");
  tft.setCursor(30, (TFT_HEIGH  / 2) + 20);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("to start!");
}

void updateDisplay() {
  tft.setRotation(1);
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(30, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.print("Breaking Out Inc.");
}

// Ingame Funktionen
void drawBricks(uint16_t color) {
  tft.fillRect(12, 50, 10, 5, color);
  tft.fillRect(27, 50, 10, 5, color);
  tft.fillRect(42, 50, 10, 5, color);
  tft.fillRect(57, 50, 10, 5, color);
  tft.fillRect(72, 50, 10, 5, color);
  tft.fillRect(87, 50, 10, 5, color);
  tft.fillRect(102, 50, 10, 5, color);
  tft.fillRect(117, 50, 10, 5, color);
  tft.fillRect(132, 50, 10, 5, color);
  //tft.fillRect(132, 50, 10, 5, color);
  //tft.fillRect(137, 50, 10, 5, color);
}

void drawBall() {
  tft.fillCircle(ballPosX, ballPosY, 3, ST77XX_RED);
  Serial.println((String)"x-Pos: " + ballPosX);
  Serial.println((String)"y-Pos: " + ballPosY);
  if (ballIsMoving == true)
  {
    ballPosX = (TFT_WIDTH / 2) + 60*(-millis()%1000)/1000.0f;
    ballPosY = 70 + 60*(-millis()%1000)/1000.0f;
  }
}

void ballMovement() {

}

void drawPlayer(uint16_t color) {
  tft.setRotation(1);
  if (inDirectionX < 1800 || inDirectionX > 1900)
    playerPosX = 1 + ((4096 - inDirectionX) / 32); 
  else
  {
    // Setzt den Spieler wieder in die Mitte des Spielfelds zurück
    playerPosX = (TFT_WIDTH / 2) - (30 / 2);
  }
  tft.fillRect(playerPosX, 120, 30, 5, color);
}

// Berechnet die zwei oberen x-Eckpunkte, um die Hitbox des Spielers zu ermitteln
void calcPlayerCollider() {
  playerX1Coll = playerPosX;
  playerX2Coll = playerPosX + 30;
  //Serial.println(playerX1Coll);
  //Serial.println(playerX2Coll);
}