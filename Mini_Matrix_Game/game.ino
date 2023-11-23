#include <LedControl.h>
#include <EEPROM.h>

// EEPROM MAP
// 0 - 3 - EASY1
// 4 - 7 - EASY2
// 8 - 11 - EASY3
// 12 - 15 - MEDIUM1
// 16 - 19 - MEDIUM2
// 20 - 23 - MEDIUM3
// 24 - 27 - HARD1
// 28 - 31 - HARD2
// 32 - 35 - HARD3
// 36 - 39 - N11
// 40 - 43 - N12
// 44 - 47 - N13
// 48 - 51 - N21
// 52 - 55 - N22
// 56 - 59 - N23
// 60 - 63 - N31
// 64 - 67 - N32
// 68 - 71 - N33

const byte startScore = 0;
const byte floatSize = 4;
const byte numberOfSavings = 3;
const byte startNicknames = 36;

const byte yPin = A0;                                       // used PINs
const byte xPin = A1;

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const byte buttonPin = 8;

const byte matrixSize = 8;
char matrix[matrixSize][matrixSize];

const int BAUD = 9600;

const byte NONE = 0;                                        // game levels
const byte EASY = 1;
const byte MEDIUM = 2;
const byte HARD = 3;

byte LEVEL = NONE;

const byte wallTypes = 2;
const char walls[wallTypes] = {' ', '#'};                   // no wall / wall

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);   // DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 5;                                  // brightness of the led matrix

byte xPos = NONE;                                           // variables to track the current and previous positions of the joystick-controlled LED
byte yPos = NONE;
byte xLastPos = NONE;
byte yLastPos = NONE;

const int minThreshold = 200;                               // thresholds for detecting joystick movement
const int maxThreshold = 600;

byte playerRow;                                             // user's coordinates
byte playerCol;

int bombRow = -1;                                           // bomb's coordinates
int bombCol = -1;

const int userBlinkingInterval = 500;                       // user blinking speed
const byte bombBlinkingInterval[3] = {50, 100, 200};        // bomb blinking speed

unsigned long currentTime;
unsigned long startTime = millis();

unsigned long startBombingTime;
unsigned long bombPlacedTime;

unsigned long userStartTime;
unsigned long userWinTime;

bool blinkState = false;                                    // blink states
bool bombBlinkState = false;

const unsigned int joystickSensitivity = 50;                // joystick settings
bool joystickMoved = false;

unsigned int minimumThreshold = 350;
unsigned int maximumThreshold = 650;

const unsigned int debounceDelay = 50;                      // button settingss
bool buttonPressed = false;
unsigned long buttonPressedTime;

const byte ASCII = 48;

bool won = false;

const byte numberOfCharacters = 4;
struct Username {
  char name[numberOfCharacters] = "";
};

bool selectedUsername = false;
Username username;

bool printed = false;
const byte delayPeriod = 100;
int count = 0;                                              // counter for non-space, non-enter characters
bool delayed = false;
unsigned long delayTime;

char input[numberOfCharacters];

unsigned long getRandomSeed() {                             // generating a random seed
  unsigned long seed = NONE;
  seed = millis();
  for (int i = NONE; i < matrixSize; ++i)
      seed = seed + analogRead(i);
  return seed;
}

void setup() {
  Serial.begin(BAUD);
  pinMode(buttonPin, INPUT_PULLUP);

  lc.shutdown(NONE, false);                                 // turn off power saving, enables display
  
  lc.setIntensity(NONE, matrixBrightness);                  // sets brightness (NONE~15 possible values)
  randomSeed(getRandomSeed());                              // seed the random number generator
  
  setStartPosition();                                       // sets the user's start position
}

void selectUsername() {
  if(!printed) {
    printNameMatrix();
    Serial.println(F("Hello! Please, enter your to-be username (3 characters only - EXC)!"));
    printed = true;
  }

  if (!selectedUsername) {
    if (count < numberOfCharacters - 1) {
      if (Serial.available()) {
        char incomingChar = Serial.read();
        if ((incomingChar >= 'a' && incomingChar <= 'z') || (incomingChar >= 'A' && incomingChar <= 'Z')) {
          input[count] = incomingChar;
          count++;
        }
      }
    } else {
      if(!delayed) {
        delayTime = millis();
        delayed = true;
      } else if (millis() - delayTime >= delayPeriod) {       // implementing a sort of delay to clear the buffer
        if (Serial.available())
          Serial.read();
        else {
          selectedUsername = true;
          printed = false;
          input[numberOfCharacters - 1] = '\0';
          strncpy(username.name, input, numberOfCharacters);  // storing the value in username 
        }
      }
    } 
  }
}

void printNameMatrix() {
  lc.clearDisplay(NONE);                                    // clear screen
  byte waitingMatrix[matrixSize][matrixSize] = {
    {0, 0, 1, 1, 1, 1, 0, 0}, 
    {0, 1, 1, 0, 0, 1, 1, 0}, 
    {0, 0, 0, 0, 0, 1, 1, 0}, 
    {0, 0, 0, 1, 1, 1, 0, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, waitingMatrix[row][col]);
}

void printWaitingMatrix() {
  lc.clearDisplay(NONE);                                    // clear screen
  byte waitingMatrix[matrixSize][matrixSize] = {
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 0, 1, 0, 0, 1, 0, 1}, 
    {1, 0, 1, 0, 0, 1, 0, 1}, 
    {1, 0, 1, 0, 0, 1, 0, 1}, 
    {1, 0, 0, 1, 1, 0, 0, 1}, 
    {1, 0, 0, 1, 1, 0, 0, 1}
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, waitingMatrix[row][col]);
}

void selectLevel() {
  if(!printed) {
    printWaitingMatrix();
    Serial.println(F("Select a difficulty (1-3). 1 - EASY, 2 - MEDIUM, 3 - HARD"));
    printed = true;
  }
  if(!LEVEL) {
    if(Serial.available() > 0) {
      int readValue = Serial.parseInt();
      Serial.read();
      if(readValue && readValue <= HARD) {
        LEVEL = readValue;
        printed = false;
        fillMatrix();
        printMatrix();
      } else Serial.println(F("Difficulty not implemented. You must put a value between 1 and 3.\nSelect a difficulty (1-3). 1 - EASY, 2 - MEDIUM, 3 - HARD"));
    }
  }
}

void setStartPosition() {
  clearMatrix();

  playerCol = random(matrixSize);
  playerRow = random(matrixSize);

  matrix[playerRow][playerCol] = 'S';
}

void loop() {
  currentTime = millis();
  if(!LEVEL) {
    if(!selectedUsername) {
      selectUsername();
    } else selectLevel();
  } else if(!won) {
    updateUserBlinking();
    movement();
    bombing();
  } else {
    waitForReset();
  }
}

void waitForReset() {
  byte buttonState = digitalRead(buttonPin);
  if (!buttonState) {
    if (!buttonPressed) {                                                     // verifying if the button was pressed
      buttonPressedTime = currentTime;                                        // starting recording the time when the button was first pressed
      buttonPressed = true;                                                   // saving the fact that it was pressed
    }
  } else {
    if (buttonPressed && currentTime - buttonPressedTime >= debounceDelay) {  // resetting the game
      LEVEL = NONE;                                         
      won = false;
      selectedUsername = false;
      delayed = false;
      count = NONE;
      setup();
    }
    buttonPressed = false;                                                    // resetting the value for the next press
  }
}

void bombing() {
  byte buttonState = digitalRead(buttonPin);
  if (!buttonState) {
    if (!buttonPressed) {                                                    // verifying if the button was pressed
      buttonPressedTime = currentTime;                                       // starting recording the time when the button was first pressed
      buttonPressed = true;                                                  // saving the fact that it was pressed
    }
  } else {
    if (buttonPressed && currentTime - buttonPressedTime >= debounceDelay) { // placing the bomb
      if(bombRow == -1) {
        bombRow = playerRow;
        bombCol = playerCol;
        matrix[bombRow][bombCol] = '3';
        startBombingTime = millis();
        bombPlacedTime = millis();
      }
    }
    buttonPressed = false; // resetting the value for the next press
  }
  if(bombRow != - 1) {
    if(currentTime - bombPlacedTime >= 1000) {                              // modifying the speed of the bomb blinking
      matrix[bombRow][bombCol] -= 1;
      printMatrix();
      bombPlacedTime = currentTime;
    }
    if(matrix[bombRow][bombCol] == '0') {                                   // if the bomb exploded, emptying the spot and clearing the walls
      matrix[bombRow][bombCol] = walls[NONE];
      bombed();
      bombRow = -1;
    } else if(currentTime - startBombingTime >= bombBlinkingInterval[matrix[bombRow][bombCol] - 1 - ASCII]) {
        bombBlinkState = !bombBlinkState;                                   // walls that will be affected will start blinking
        if(bombRow - 1 >= NONE && matrix[bombRow - 1][bombCol] == walls[1]) {
          lc.setLed(NONE, bombRow - 1, bombCol, bombBlinkState);
        }
        if(bombRow + 1 < matrixSize && matrix[bombRow + 1][bombCol] == walls[1]) {
          lc.setLed(NONE, bombRow + 1, bombCol, bombBlinkState);
        }
        if(bombCol - 1 >= NONE && matrix[bombRow][bombCol - 1] == walls[1]) {
          lc.setLed(NONE, bombRow, bombCol - 1, bombBlinkState);
        }
        if(bombCol + 1 < matrixSize && matrix[bombRow][bombCol + 1] == walls[1]) {
          lc.setLed(NONE, bombRow, bombCol + 1, bombBlinkState);
        }
        startBombingTime = currentTime;
    }
  }
}

void bombed() {
  if(bombRow - 1 >= NONE) {
    lc.setLed(NONE, bombRow - 1, bombCol, false);
    matrix[bombRow - 1][bombCol] = walls[NONE];
  }
  if(bombRow + 1 < matrixSize) {
    lc.setLed(NONE, bombRow + 1, bombCol, false);
    matrix[bombRow + 1][bombCol] = walls[NONE];
  }
  if(bombCol - 1 >= NONE) {
    lc.setLed(NONE, bombRow, bombCol - 1, false);
    matrix[bombRow][bombCol - 1] = walls[NONE];
  }
  if(bombCol + 1 < matrixSize) {
    lc.setLed(NONE, bombRow, bombCol + 1, false);
    matrix[bombRow][bombCol + 1] = walls[NONE];
  }

  if (checkWin()) {
    won = true;
    displayWinLED();
  }
}

void displayWinLED() {
  byte winMatrix[matrixSize][matrixSize] = {
    {1, 1, 0, 0, 0, 0, 1, 1}, 
    {1, 1, 0, 0, 0, 0, 1, 1}, 
    {1, 1, 0, 0, 0, 0, 1, 1}, 
    {1, 1, 0, 0, 0, 0, 1, 1}, 
    {1, 1, 0, 1, 1, 0, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 0, 0, 1, 1, 1}, 
    {1, 1, 0, 0, 0, 0, 1, 1} 
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, winMatrix[row][col]);
}

void updateUserBlinking() {
  if (currentTime - startTime >= userBlinkingInterval) { // blinking state of the cursor
    blinkState = !blinkState;
    lc.setLed(NONE, playerRow, playerCol, blinkState);
    startTime = currentTime;
  }
}

void movement() {
  unsigned int xValue = analogRead(xPin);
  unsigned int yValue = analogRead(yPin);

  if (xValue + joystickSensitivity < minimumThreshold) {        // verifying the joystick motion for x value
    verifyJoystickMotion(NONE, -1);                             // and the fact that the user could go in the desired direction
  } else if (xValue - joystickSensitivity > maximumThreshold) {
    verifyJoystickMotion(NONE, 1);
  } else if (yValue + joystickSensitivity < minimumThreshold) { // verifying the joystick motion for y value
    verifyJoystickMotion(1, NONE);                              // and the fact that the user could go in the desired direction
  } else if (yValue - joystickSensitivity > maximumThreshold) {
    verifyJoystickMotion(-1, NONE);
  } else joystickMoved = false;
}

void verifyJoystickMotion(int posY, int posX) {
  if (!joystickMoved) {                   // verifying if it's not true so it won't move more than 1 place at a time
    if (verifyModifyState(posY, posX)) {  // verifying that it can go to that path
      lc.setLed(NONE, playerRow, playerCol, false);
      playerRow += posY;
      playerCol += posX;
      lc.setLed(NONE, playerRow, playerCol, true);
      startTime = currentTime;
      joystickMoved = true;               // setting it true so it won't move more than 1 place at a time
    }
  }
}

bool verifyModifyState(int posY, int posX) {
  if(matrix[playerRow + posY][playerCol + posX] == walls[1])      // verifying if a wall is there
    return false;
  if(playerRow + posY >= matrixSize)                              // next, verifying if it's not going after the matrix limits
    return false;
  if(playerRow + posY < NONE)
    return false;
  if(playerCol + posX >= matrixSize)
    return false;
  if(playerCol + posX < NONE)
    return false;
  return true;
}

void fillMatrix() {
  lc.clearDisplay(NONE);                    // clear screen
  int numHash;

  switch (LEVEL) {                          // deciding the number of walls for every level
    case 1:
      numHash = 32;
      break;
    case 2:
      numHash = 45;
      break;
    case 3:
      numHash = 55;
      break;
  }

  for (int i = NONE; i < numHash; i++) {    // filling the matrix randomly with the desired number of walls
    int row = random(8);
    int col = random(8);
    if(matrix[row][col] == walls[NONE])     // verifying if we don't put a wall on the player starting position or an already existing wall
      matrix[row][col] = walls[1];
    else i -= 1;
  }

  matrix[playerRow][playerCol] = ' ';       // clearing the user's place in matrix
  userStartTime = millis();
}

void clearMatrix() {
  for (int i = NONE; i < matrixSize; i++)         // emptying the matrix
    for (int j = NONE; j < matrixSize; j++) 
      matrix[i][j] = walls[NONE];
}

void printMatrix() {
  for (int row = NONE; row < matrixSize; row++)
    for (int col = NONE; col < matrixSize; col++)
      lc.setLed(NONE, row, col, matrix[row][col] == walls[1] ? true : false);  // turns on LED at col, row
}

bool checkWin() {
  for (int row = NONE; row < matrixSize; row++)                                             // checking for any walls so it's not a win
    for (int col = NONE; col < matrixSize; col++)
      if (matrix[row][col] != ' ')
        return false;
  userWinTime = millis();
  float score = float(userWinTime - userStartTime) / 1000;                                  // displaying the time as seconds
  Serial.print(F("You've completed the level in just "));
  Serial.print(score);
  Serial.println(F(" seconds!"));

  float highscores[numberOfSavings];
  Username usernames[numberOfSavings];
  for (int i = 0; i < numberOfSavings; i++) {
    EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscores[i]);                         // retrieve the existing high scores from EEPROM
    EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, usernames[i].name);    // retrieve the usernames of the owners of the high scores from EEPROM
  }

  for(int i = 0; i < numberOfSavings; i++)
    if(score < highscores[i] || !highscores[i]) {
      for(int j = numberOfSavings - 1; j > i; j--) {
        highscores[j] = highscores[j - 1];
        strncpy(usernames[j].name, usernames[j-1].name, numberOfCharacters);
      }
      highscores[i] = score;
      strncpy(usernames[i].name, username.name, numberOfCharacters);
      break;
    }

  for (int i = 0; i < numberOfSavings; i++) {
    EEPROM.put((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscores[i]);   // updating the EEPROM with the new highscores
    EEPROM.put(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, usernames[i].name);   // updating the EEPROM with the new usernames
  }
  
  checkHighscores();                                                                        // printing existing highscores for user
  return true;
}

void checkHighscores() {
  for(int levels = 0; levels < numberOfSavings; levels++) {
    Serial.println("Level " + String(levels + 1));
    for(int i = NONE; i < numberOfSavings; i++) {
      float highscore;
      Username user;
      EEPROM.get((levels) * floatSize * numberOfSavings + i * floatSize, highscore);
      EEPROM.get(startNicknames + (levels) * floatSize * numberOfSavings + i * floatSize, user.name);
      Serial.print(String(i + 1) + ". ");
      Serial.print(user.name);
      Serial.println(": " + String(highscore) + "s");
    }
  }
  Serial.println(F("Press the button to reset the game and start once again!"));
}
