// SNAKE GAME
// BY NOOH ALAVI

#include <LedControl.h>

/*
 * HOW TO SET UP THE ARDUINO UNO
 * 
 * LED MATRIX:
 *  VCC: 5V
 *  GND: GND
 *  DIN: DIGITAL PIN  8
 *  CS: DIGITAL PIN 9
 *  CLK: DIGITAL PIN 10
 * 
 * JOYSTICK:
 *  GND: GND
 *  +5V: 5V
 *  VRx: A0
 *  VRy = A1
 *  SW: DIGITAL PIN 2
 *  
 * BUZZER: 
 *  +: DIGITAL PIN 9  
 *  -: GND
 *  MAKE SURE TO ADD 100 OHM RESISTOR
*/

int DIN = 8;
int CS = 9;
int CLK = 10;

int joyX = A0;
int joyY = A1;
int joyBtn = 2;

int buzzer = 6;

LedControl lc = LedControl(DIN, CLK, CS, 0);
const int lcId = 0;

int appleX = 3;
int appleY = 2;

class SnakeBody {
  public:
    int x;
    int y;
    int prevX;
    int prevY;
    bool isActive = false;
    bool isHead = false;
};

SnakeBody snakeBody[64] = {};
String snakeDir = "Right";
int snakeSpeed = 1 ;

void setup() {
  lc.shutdown(lcId, false);
  lc.clearDisplay(0);

  Serial.begin(9600);
  randomSeed(analogRead(13));

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyBtn, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  snakeBody[0].isActive = true;
  snakeBody[0].isHead = true;
}
void(* resetFunc) (void) = 0;

void loop() {
  updateGame();
  renderGame();
  delay(250);
}

void updateGame() {
  getInput();
  checkDie();
  doMovement();
}


void getInput() {
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);

  Serial.print("Joystick X: ");
  Serial.print(xVal);
  Serial.print(": | Joystick Y: ");
  Serial.print(yVal);
  Serial.println();

  if (xVal > 600) {
    if (snakeDir != "Left") {
      snakeDir = "Right";
    }
  } else if (xVal < 400) {
    if (snakeDir != "Right") {
      snakeDir = "Left";
    }
  }

  if (yVal > 600) {
    if (snakeDir != "Up") {
      snakeDir = "Down";
    }
  } else if (yVal < 400) {
    if (snakeDir != "Down") {
      snakeDir = "Up";
    }
  }
}

bool isColliding(int aX, int aY, int bX, int bY) {
  return (aX == bX && aY == bY);
}

void eatApple() {
  tone(buzzer, 500);
  delay(10);
  noTone(buzzer);
  appleX = random(0, 7);
  appleY = random(0, 7);
}

void doMovement() {

//  if (digitalRead(joyBtn) == LOW && snakeSpeed == 0) {
//    snakeSpeed == 1;
//  }
  
  for (int i = 0; i < 65; i++) {
    snakeBody[i].prevX = snakeBody[i].x;
    snakeBody[i].prevY = snakeBody[i].y;
  }
  
  if (snakeDir == "Right") {
    snakeBody[0].x += snakeSpeed;
  } else if (snakeDir == "Left") {
    snakeBody[0].x -= snakeSpeed;
  } else if (snakeDir == "Up") {
    snakeBody[0].y -= snakeSpeed;
  } else if (snakeDir == "Down") {
    snakeBody[0].y += snakeSpeed;
  }

  for (int i = 0; i < 65; i++) {
    if (snakeBody[i].isHead) {
      continue;
    }
    snakeBody[i].x = snakeBody[i-1].prevX;
    snakeBody[i].y = snakeBody[i-1].prevY ;
  }

  if (snakeBody[0].x < 0) {
    snakeBody[0].x = 0;
    gameOver();
  } else if (snakeBody[0].x > 7) {
    snakeBody[0].x = 7;
    gameOver();
  } else if (snakeBody[0].y < 0) {
    snakeBody[0].y = 0;
    gameOver();
  } else if (snakeBody[0].y > 7) {
    snakeBody[0].y = 7;
    gameOver();
  }

  if (isColliding(snakeBody[0].x, snakeBody[0].y, appleX, appleY)) {
    eatApple();
    growSnake();
  }
}

void growSnake() {
  for (int i = 0; i < 65; i++) {
    if (snakeBody[i].isActive) {
      continue;
    }
    snakeBody[i].isActive = true;
    break;
  }
}

void checkDie() {
  for (int i = 1; i < 65; i++) {
    if (snakeBody[i].isHead || !snakeBody[i].isActive) {
      continue;
    }
    if (isColliding(snakeBody[0].x, snakeBody[0].y, snakeBody[i].x, snakeBody[i].y)) {
      gameOver();
    }
  }
}

void gameOver() {
  tone(buzzer, 1000);
  delay(1000);
  noTone(buzzer);
  resetFunc();
}

void renderGame() {
  lc.clearDisplay(0);

  // Render snake
  for (int i = 0; i < 65; i++) {
    if (snakeBody[i].isActive) {
      lc.setLed(lcId, snakeBody[i].y, snakeBody[i].x, true);
    } else {
      break;
    }
  }

  //Render apple
  lc.setLed(lcId, appleY, appleX, true);
}
