// Import von h-Dateien (Klassen)
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Farbcodes
#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

#define GREY      RGB(127, 127, 127)

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

//
// Konstanten
//
// Originale Display Größe: 128x160 | Invertiert: WIDTH = 160, 
const int TFT_WIDTH = 160;
const int TFT_HEIGH = 128;

// Spielfeld
const int WALL_UP = 10;
const int WALL_RIGHT = TFT_WIDTH - 2;
const int WALL_LEFT = 2;

// Spieler
const int PLAYER_WIDTH = 30;
const int PLAYER_HEIGH = 5;

// Ball
const int BALL_R = 3;

//
// Globale Variablen
//
int inJYSTDirectionX = 0;

// Startwerte
int ballPosX = (TFT_WIDTH / 2);
int ballPosY = 115;
int ballDirectionX = 1;
int ballDirectionY = -1;

int playerPosX = (TFT_WIDTH / 2) - (PLAYER_WIDTH / 2); // Mittelpunkt im Spielfeld - Initial
int playerX1Coll = playerPosX;
int playerX2Coll = playerPosX + PLAYER_WIDTH;

bool ballIsMoving = false;
bool gameIsStarted = false;

// Ermöglicht den Zugriff auf grundlegende Funktionen für den TFT-Screen aus den Klassen SPIClass und Adafruit_ST7735
SPIClass tftSPI = SPIClass(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST); // SPI, CS, DC, RST

//
// Main-Funktionen
//

void setup() 
{
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

void loop() 
{
  if (digitalRead(BUTTON_RIGHT) == HIGH)
    gameIsStarted = true;
  if (gameIsStarted == true)
  {
    updateDisplay();
    // Lese das Eingangssignal des Joysticks (0 bis 4095)
    inJYSTDirectionX = 4095 - analogRead(JOYSTICK_X);

    // Zeichne aktuelle Spielobjekte ins Spielfeld
    drawBricks(ST77XX_BLUE);
    drawPlayer(ST77XX_BLACK);
    calcPlayerCollider();
    drawBall();

    while (ballIsMoving == false)
    {
      if (digitalRead(BUTTON_UP) == HIGH) 
      {
        ballIsMoving = true;
      }
    }

    // Game-Over
    if (ballHitsDeathzone() == true)
    {
      showGameoverScreen();
      gameIsStarted = false;
      while(1)
      {
        if (digitalRead(BUTTON_RIGHT) == HIGH)
        {
          tft.fillScreen(ST77XX_WHITE);
          break;
        }
      }
      exit(0);
    }
    delay(30);
  }
}

//
// Funktionen
//

// -- Allgemeine Funktionen --
void showStartScreen() 
{
  tft.setRotation(1);
  tft.setCursor(30, (TFT_HEIGH  / 2) - 10);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("Breakout Game");

  tft.setCursor(30, (TFT_HEIGH / 2) + 10);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("Press BUTTON-RIGHT");

  tft.setCursor(30, (TFT_HEIGH  / 2) + 20);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("to start!");
}

void updateDisplay() 
{
  tft.setRotation(1);

  // Titelbar
  tft.fillRect(0, 0, TFT_WIDTH, 9, GREY);
  tft.setCursor((TFT_WIDTH / 2) - 30, 1);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.print("Breakout v.1");

  // Setze Spieler-Lebensbalken
  tft.fillRect(2, 2, 4, 5, ST77XX_RED);
  tft.fillRect(8, 2, 4, 5, ST77XX_RED);
  tft.fillRect(14, 2, 4, 5, ST77XX_RED);

  // Todeszone
  tft.fillRect(0, 125, TFT_WIDTH, 3, ST77XX_ORANGE);

  // Aktualisiert das Spielfeld unter der Titelbar
  tft.fillRect(0, 9, TFT_WIDTH, TFT_HEIGH - 12, ST77XX_WHITE);
}

void showGameoverScreen()
{
  tft.setRotation(1);
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(25, (TFT_HEIGH  / 2) - 10);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2.5);
  tft.println("GAME OVER");

  tft.setCursor(30, (TFT_HEIGH  / 2) + 20);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("Press BUTTON-RIGHT");

  tft.setCursor(30, (TFT_HEIGH  / 2) + 30);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.println("to restart.");
}

void drawBricks(uint16_t color) 
{
  tft.fillRect(12, 50, 10, 5, color);
  tft.fillRect(27, 50, 10, 5, color);
  tft.fillRect(42, 50, 10, 5, color);
  tft.fillRect(57, 50, 10, 5, color);
  tft.fillRect(72, 50, 10, 5, color);
  tft.fillRect(87, 50, 10, 5, color);
  tft.fillRect(102, 50, 10, 5, color);
  tft.fillRect(117, 50, 10, 5, color);
  tft.fillRect(132, 50, 10, 5, color);
}

void drawBall() 
{
  tft.fillCircle(ballPosX, ballPosY, BALL_R, ST77XX_RED);
  if (ballIsMoving == true)
    moveBall();
}

void drawPlayer(uint16_t color) 
{
  tft.setRotation(1);
  // Ermittelt die Stick-Position und interpretiert es in die Spielerbewegung um
  if (inJYSTDirectionX < 2050 || inJYSTDirectionX > 2250)
    playerPosX = 1 + ((4096 - inJYSTDirectionX) / 32);

  // Setzt den Spieler wieder in die Mitte des Spielfelds zurück
  else
    playerPosX = (TFT_WIDTH / 2) - (PLAYER_WIDTH / 2);

  tft.fillRect(playerPosX, 120, PLAYER_WIDTH, PLAYER_HEIGH, color);
}

// -- Gameplay Funktionen --
void moveBall() 
{
  // Ball-Bewegung
  ballPosX += ballDirectionX;
  ballPosY += ballDirectionY;

  // Ball-Richtung hoch-rechts [+x, -y]
  if ((ballPosX == WALL_LEFT + (BALL_R / 2) && ballDirectionX == -1 && ballDirectionY == -1) || 
    (ballHitsPlayer() == true && ballDirectionX == 1 && ballDirectionY == 1))
  {
    ballDirectionX = 1;
    ballDirectionY = -1;
  }
  // Ball-Richtung runter-rechts [+x, +y]
  else if ((ballPosX == WALL_LEFT + (BALL_R / 2) && ballDirectionX == -1 && ballDirectionY == 1) || 
    (ballPosY == WALL_UP + (BALL_R / 2) && ballDirectionX == 1 && ballDirectionY == -1))
  {  
    ballDirectionX = 1;
    ballDirectionY = 1;
  }
  // Ball-Richtung runter-links [-x, +y]
  else if ((ballPosX == WALL_RIGHT - (BALL_R / 2) && ballDirectionX == 1 && ballDirectionY == 1) || 
    (ballPosY == WALL_UP + (BALL_R / 2) && ballDirectionX == -1 && ballDirectionY == -1))
  {  
    ballDirectionX = -1;
    ballDirectionY = 1;
  }
  // Ball-Richtung hoch-links [-x, -y]
  else if ((ballPosX == WALL_RIGHT - (BALL_R / 2) && ballDirectionX == 1 && ballDirectionY == -1) || 
    (ballHitsPlayer() == true && ballDirectionX == -1 && ballDirectionY == 1))
  {
    ballDirectionX = -1;
    ballDirectionY = -1;
  }
}

bool ballHitsPlayer() 
{
  bool bHP = false;
  if (ballPosY == (120 - (PLAYER_HEIGH / 2) - (BALL_R / 2)) && 
    (ballPosX >= playerX1Coll && ballPosX <= playerX2Coll))
    bHP = true;
  return bHP;
}

bool ballHitsDeathzone()
{
  bool bhDz = false;
  if (ballPosY > 120)
    bhDz = true;
  return bhDz;
}

// Berechnet die zwei oberen x-Eckpunkte, um die Hitbox des Spielers zu ermitteln
void calcPlayerCollider() 
{
  playerX1Coll = playerPosX;
  playerX2Coll = playerPosX + PLAYER_WIDTH;
}