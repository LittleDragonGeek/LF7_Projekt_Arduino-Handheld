#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_CS    19
#define TFT_RST   21
#define TFT_DC    22

// Button Pins for Left and Right control
#define BUTTON_LEFT 32
#define BUTTON_RIGHT 33

SPIClass tftSPI = SPIClass(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST); // SPI, CS, DC, RST

int carX = 60; // Starting X position of the car
int carY = 150; // Y position of the car
int roadWidth = 80; // Width of the road
int roadLeft = 20; // Left boundary of the road
int roadRight = 100; // Right boundary of the road
int score = 0;

int obstacleY = 0;
int obstacleX = 40; // Obstacle starting X position

void setup() {
  Serial.begin(9600);

  tftSPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); // SCK, MISO, MOSI, CS
  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(40000000);

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  tft.fillScreen(ST77XX_WHITE);
}

void drawRoad() {
  // Draw the road (two lines on the sides)
  tft.fillRect(roadLeft, 0, roadWidth, 240, ST77XX_GRAY);
}

void drawCar() {
  // Draw the car
  tft.fillRect(carX, carY, 20, 30, ST77XX_RED);
}

void drawObstacle() {
  // Draw the obstacle
  tft.fillRect(obstacleX, obstacleY, 20, 20, ST77XX_BLACK);
}

void moveCar() {
  // Move the car based on button press
  if (digitalRead(BUTTON_LEFT) == LOW && carX > roadLeft) {
    carX -= 5;
  }
  if (digitalRead(BUTTON_RIGHT) == LOW && carX < roadRight - 20) {
    carX += 5;
  }
}

void moveObstacle() {
  // Move the obstacle down
  obstacleY += 5;
  
  // If the obstacle goes out of the screen, reset it
  if (obstacleY > 240) {
    obstacleY = 0;
    obstacleX = random(roadLeft, roadRight - 20);
    score++;
  }
}

bool checkCollision() {
  // Simple collision detection between the car and the obstacle
  if (obstacleY + 20 > carY && obstacleY < carY + 30) {
    if (obstacleX + 20 > carX && obstacleX < carX + 20) {
      return true;
    }
  }
  return false;
}

void displayScore() {
  tft.setCursor(15, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1.5);
  tft.print("Score: ");
  tft.print(score);
}

void loop() {
  tft.fillScreen(ST77XX_WHITE);  // Clear screen

  drawRoad();
  moveCar();
  drawCar();
  moveObstacle();
  drawObstacle();
  
  if (checkCollision()) {
    tft.fillScreen(ST77XX_RED); // Game Over screen
    tft.setCursor(40, 100);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.print("GAME OVER!");
    tft.setCursor(40, 120);
    tft.print("Score: ");
    tft.print(score);
    while(1); // Stop the game
  }
  
  displayScore();  // Show score
  delay(30);  // Control the game speed
}
