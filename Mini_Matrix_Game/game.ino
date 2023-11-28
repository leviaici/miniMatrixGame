#include "LedControl.h"
#include <LiquidCrystal.h>
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
// 72 - LCDB
// 73 - MTXB
// 74 - SND

const byte startScore = 0;
const byte floatSize = 4;
const byte numberOfSavings = 3;
const byte startNicknames = 36;
const byte startLCDB = 72;
const byte startMTXB = 73;
const byte startSound = 74;

const byte rs = 9;
const byte en = 13;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte lcdBrightnessPin = 3;

const byte yPin = A0;                                       // used PINs
const byte xPin = A1;

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const byte buttonPin = 8;

// const byte buzzerPin = 2;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);   // DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte lcdBrightness = EEPROM.read(startLCDB);
byte matrixBrightness = EEPROM.read(startMTXB);
bool sounds = EEPROM.read(startSound);

const byte matrixSize = 8;

char matrix[matrixSize][matrixSize];

const byte wallTypes = 2;
const char walls[wallTypes] = {' ', '#'};                   // no wall / wall

const byte NONE = 0;
const byte EASY = 1;
const byte MEDIUM = 2;
const byte HARD = 3;

byte LEVEL = EASY;

byte xPos = NONE;                                           // variables to track the current and previous positions of the joystick-controlled LED
byte yPos = NONE;
byte xLastPos = NONE;
byte yLastPos = NONE;

const int BAUD = 9600;
const byte ASCII = 48;

byte menuOption = 0;             // menu option selected
byte submenuOption = 0;          // submenu option selected
byte highscoreOption = 0;
byte settingsOption = 0;

bool menuPrinted = false;        // if the text was printed for each (menu, submenu or option)
bool submenuPrinted = false;
bool functionPrinted = false;

bool inSubmenu = false;          // location of the user 
bool inFunction = false;
unsigned long currentTime = millis();

const unsigned int joystickSensitivity = 50;                // joystick settings
bool joystickMoved = false;
bool joystickLeftMoved = false;
bool joystickRightMoved = false;

unsigned int minimumThreshold = 350;
unsigned int maximumThreshold = 650;

byte playerRow;                                             // user's coordinates
byte playerCol;

byte availableBombs = 1;
int bombRow = -1;                                           // bomb's coordinates
int bombCol = -1;

const int userBlinkingInterval = 500;                       // user blinking speed
const byte bombBlinkingInterval[3] = {50, 100, 200};        // bomb blinking speed

unsigned long startTime = millis();

unsigned long startBombingTime;
unsigned long bombPlacedTime;

unsigned long userStartTime;
unsigned long userWinTime;

bool blinkState = false;                                    // blink states
bool bombBlinkState = false;

const unsigned int debounceDelay = 50;                      // button settingss
bool buttonPressed = false;
unsigned long buttonPressedTime;

const byte numberOfCharacters = 4;
struct Username {
  char name[numberOfCharacters] = "";
};
Username username;
byte usernameIndex = 0;

byte aboutRow = 0;
byte howToRow = 0;

const byte aboutRows = 6;
const byte menuOptions = 6;
const byte settingsOptions = 4;
const byte howToRows = 7;
const byte maximumLCDBrightness = 255;
const byte maximumMTXBrightness = 15;

bool menuing = false;
bool settingUp = false;
bool gaming = false;
bool won = false;

bool printed = false;
const byte delayPeriod = 100;
int count = 0;                                              // counter for non-space, non-enter characters
bool delayed = false;
unsigned long delayTime;

char input[numberOfCharacters];

float score = -1;

bool firstEndScreen = false;
bool secondEndScreen = false;
bool displayedScreen = false;

bool modified = false;

bool userPrinted = false;
bool firstRun = false;

bool seeded = false;
bool positioned = false;

void setup() {
  lc.shutdown(NONE, false);                                 // turn off power saving, enables display
  
  lc.setIntensity(NONE, matrixBrightness);                  // sets brightness (NONE~15 possible values)
  matrixOnOff(0);

  lcd.begin(16, 2);
  analogWrite(lcdBrightnessPin, lcdBrightness);
  initialMessage();

  if(!firstRun) {
    pinMode(buttonPin, INPUT_PULLUP);
    // pinMode(buzzerPin, OUTPUT);
    strcpy(username.name, "AAA\0");
    firstRun = true;
    // Serial.begin(BAUD);
  }
}

void initialMessage() {
  lcd.setCursor(NONE, NONE);
  lcd.print(F("   Bombentziu"));
  lcd.setCursor(NONE, 1);
  lcd.print(F(" <Press Button>"));
}

void displayFirstEndScreen() {
  if(!displayedScreen) {
    displayedScreen = true;
    lcd.clear();
    lcd.setCursor(NONE, NONE);
    lcd.print(F("Congrats on"));
    lcd.setCursor(NONE, 1);
    lcd.print(F("completing lvl "));
    switch(LEVEL) {
      case EASY:
        lcd.print(F("1"));
        break;
      case MEDIUM:
        lcd.print(F("2"));
        break;
      case HARD:
        lcd.print(F("3"));
        break;
    }
  }
  if(verifyButtonPress()) {
    displayedScreen = false;
    firstEndScreen = false;
    secondEndScreen = true;
  }
}

void displaySecondEndScreen() {
  if(!displayedScreen) {
    displayedScreen = true;
    lcd.clear();
    lcd.setCursor(NONE, NONE);
    lcd.print(F("Just ")); // Just 1234.78s!
    lcd.print(score);
    lcd.print(F(" s!"));
    lcd.setCursor(NONE, 1);
    if(modified) {
      lcd.print("You are in TOP 3");
      modified = false;
    } else lcd.print("Not in TOP 3");
  }
  if(verifyButtonPress()) {
    displayedScreen = false;
    secondEndScreen = false;
  }
}

void loop() {
  currentTime = millis();
  if(menuing) {
    mainMenu();
    if(!inSubmenu)
      upOrDownMovement(NONE);
  } else if(!gaming){
    waitForMenu();
  } else if(gaming) {
    if(settingUp) {
      if(!seeded) {
        randomSeed(getRandomSeed());                              // seed the random number generator
        seeded = true;
      } else if(!positioned) {
        setStartPosition();                                       // sets the user's start position
      } else {
        fillMatrix();
        printMatrix();
        seeded = false;
        positioned = false;
        settingUp = false;
      }
    } else {
      if(!won) {
        updateUserBlinking();
        movement();
        bombing();
        liveDisplaying();
      } else {
        if(firstEndScreen)
          displayFirstEndScreen();
        else if(secondEndScreen)
          displaySecondEndScreen();
        else reset();
      }
    }
  }
}

void liveDisplaying() {
  if(!userPrinted) {
    userPrinted = true;
    lcd.clear();
    lcd.setCursor(NONE, NONE);  // LEV-Bombs left:1
    lcd.print(String(username.name));
    lcd.print(F("-Bombs left:"));
    lcd.setCursor(NONE, 1);
    lcd.print(F("Time: "));
  }
  lcd.setCursor(15, NONE);;
  lcd.print(availableBombs);
  lcd.setCursor(6, 1);
  lcd.print(String((currentTime - userStartTime) / 1000) + "s");
}

// ---------------------------------------------------------------------------------------
// --------------------------------------- Game Menu -------------------------------------
// -----------------------------------------------------------------------------------------

void waitForMenu() {
  byte buttonState = digitalRead(buttonPin);
  if (!buttonState) {
    if (!buttonPressed) {                                                     // verifying if the button was pressed
      buttonPressedTime = currentTime;                                        // starting recording the time when the button was first pressed
      buttonPressed = true;                                                   // saving the fact that it was pressed
    }
  } else {
    if (buttonPressed && currentTime - buttonPressedTime >= debounceDelay) {
      menuing = true;
      lcd.clear();
    }
    buttonPressed = false;                                                    // resetting the value for the next press
  }
}

void printMainMenuCommands() {
  lcd.clear();
  switch(menuOption) {
    case 0:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">Start Game"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" Level: "));
      printLevelMenu();
      break;
    case 1:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">Level: "));
      printLevelMenu();
      lcd.setCursor(NONE, 1);
      lcd.print(F(" Highscores"));
      break;
    case 2:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">Highscores"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" Settings"));
      break;
    case 3:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">Settings"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" About"));
      break;
    case 4:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">About"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" How to Play"));
      break;
    case 5:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(" About"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(">How to Play"));
      break;
  }
}

void printLevelMenu() {
  switch(LEVEL) {
    case EASY:
      lcd.print(F("ESY"));
      break;
    case MEDIUM:
      lcd.print(F("MED"));
      break;
    case HARD:
      lcd.print(F("HRD"));
      break;
  }
}

bool verifyButtonPress() {
  byte buttonState = digitalRead(buttonPin);
  if (!buttonState) {
    if (!buttonPressed) {                                                     // verifying if the button was pressed
      buttonPressedTime = currentTime;                                        // starting recording the time when the button was first pressed
      buttonPressed = true;                                                   // saving the fact that it was pressed
    }
  } else {
    if (buttonPressed && currentTime - buttonPressedTime >= debounceDelay) {
      buttonPressed = false;
      return true;
    }
    buttonPressed = false;                                                    // resetting the value for the next press
  }
  return false;
}

void mainMenu() {
  if (!menuPrinted) {                      // if menu wasn't printed, we are printing it (same for the other submenus and functions)
    printMainMenuCommands();
    menuPrinted = true;
  }

  if(!inSubmenu)
    if (verifyButtonPress())
      inSubmenu = true;
    
  if(inSubmenu) {
    switch (menuOption) {
      case 0:
        startGame(); // gaming = true;
        lcd.clear();
        break;
      case 1:
        setDifficulty();
        break;
      case 2:
        highscorePrintings();
        break;
      case 3:
        settings();
        break;
      case 4:
        about();
        break;
      case 5:
        howToPlay();
        break;
    }
  }
}

void startGame() {
  gaming = true;
  settingUp = true;
  menuing = false;
  inSubmenu = false;
  goToMainMenu();
}

void setDifficulty() {
  if(LEVEL + 1 > HARD)
    LEVEL = EASY;
  else LEVEL += 1;
  lcd.setCursor(8, 0);
  printLevelMenu();
  inSubmenu = false;
}

void printHowToPlay() {
  lcd.clear();
  switch (howToRow) {
    case 0:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("Firstly, you"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("need to select"));
      break;
    case 1:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("the desired lvl."));
      lcd.setCursor(NONE, 1);
      lcd.print(F("You can do it by"));
      break;
    case 2:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("selecting it via"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("the menu. You"));
      break;
    case 3:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("can also select"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("your username"));
      break;
    case 4:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("using the"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("settings menu."));
      break;
    case 5:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("github.com/"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("leviaici for"));
      break;
    case 6:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("more details"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("about the game."));
      break;
    case 7:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("Swipe left to"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("leave the about."));
      break;
  }
}

void howToPlay() {
  if(!submenuPrinted) {
    printHowToPlay();
    submenuPrinted = true;
  }
  upOrDownMovement(7);
  if(leftMovement()) {
    inSubmenu = false;
    howToRow = 0;
    submenuPrinted = false;
    goToMainMenu();
  }
}

void printAbout() {
  lcd.clear();
  switch (aboutRow) {
    case 0:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("Hi! Welcome to"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("Bombentziu!"));
      break;
    case 1:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("This is my (LEV)"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("uni project for"));
      break;
    case 2:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("robotics! Hope"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("you like it!"));
      break;
    case 3:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("github.com/"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("leviaici for"));
      break;
    case 4:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("more cool repos"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("or games/apps."));
      break;
    case 5:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("Swipe left to"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("leave the about."));
      break;
  }
}

void about() {
  if(!submenuPrinted) {
    printAbout();
    submenuPrinted = true;
  }
  upOrDownMovement(6);
  if(leftMovement()) {
    inSubmenu = false;
    aboutRow = 0;
    submenuPrinted = false;
    goToMainMenu();
  }
}

void printSettings() {
  lcd.clear();
  switch(settingsOption) {
    case 0:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">Set username"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" LCD Brightness"));
      break;
    case 1:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">LCD Brightness"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" MTX Brightness"));
      break;
    case 2:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(">MTX Brightness"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" Sounds: "));
      lcd.print(sounds ? F("ON") : F("OFF"));
      break;
    case 3:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(" MTX Brightness"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(">Sounds: "));
      lcd.print(sounds ? F("ON") : F("OFF"));
      break;
  } 
}

void saveSettingsData() {
  EEPROM.update(startLCDB, lcdBrightness);
  EEPROM.update(startMTXB, matrixBrightness);
  EEPROM.update(startSound, sounds);
}

void settings() {
  if(!submenuPrinted) {
    printSettings();
    submenuPrinted = true;
  }

  if(!inFunction)
    if(verifyButtonPress())
      inFunction = true;

  if(inFunction) {
    switch(settingsOption) {
      case 0:
        setUsername();
        break;
      case 1:
        setBrightness(0);
        break;
      case 2:
        setBrightness(1);
        break;
      case 3:
        setSound();
        break;
    }
  } else {
    upOrDownMovement(2);
    if(leftMovement()) {
      saveSettingsData();
      inSubmenu = false;
      submenuPrinted = false;
      settingsOption = 0;
      goToMainMenu();
    }
  }
}

void printUsername(byte toUpdate) {
  if(!toUpdate) {
    lcd.clear();
    lcd.setCursor(NONE, NONE);
    lcd.print(F("When done, click"));
    lcd.setCursor(NONE, 1);
    lcd.print(F("button. USR: "));
    lcd.print(String(username.name));
    lcd.setCursor(13, 1);
    lcd.blink();
  } else {
    lcd.setCursor(13 + usernameIndex, 1);
    lcd.blink();
    lcd.print(username.name[usernameIndex]);
    lcd.setCursor(13 + usernameIndex, 1);
    lcd.blink();
  }
}

void setUsername() {
  if(!functionPrinted) {
    printUsername(false);
    functionPrinted = true;
  }

  upOrDownMovement(5);
  
  if(leftMovement())
    verifyCursorChange(-1);
  else if(rightMovement())
    verifyCursorChange(1);

  if(verifyButtonPress()) {
    lcd.noBlink();
    submenuPrinted = false;
    goToSubmenu();
  }
}

void verifyCursorChange(int distance) {
  if(usernameIndex + distance >= 0 && usernameIndex + distance < 3) {
    usernameIndex += distance;
    printUsername(true);
  }
}

void matrixOnOff(bool print) {
  for(int row = NONE; row < matrixSize; row++)
        for(int col = NONE; col < matrixSize; col++)
          lc.setLed(0, row, col, print);
}

void printBrightness(bool toUpdate, byte brightnessCase) {
  if(!toUpdate) {
    if(brightnessCase) {
      matrixOnOff(1);
    }

    lcd.clear();
    lcd.print(F(" Scroll up/down "));
    lcd.setCursor(0, 1);
    lcd.print(F("Done-Button: "));
  } else {
    lcd.setCursor(13, 1);
    lcd.print(F("   "));
    lcd.setCursor(13, 1);
  }
  if(!brightnessCase)
    lcd.print(lcdBrightness);
  else lcd.print(matrixBrightness);
}

void setBrightness(byte brightnessCase) {
  if(!functionPrinted) {
    printBrightness(false, brightnessCase);
    functionPrinted = true;
  }

  if(!brightnessCase)
    upOrDownMovement(3);
  else upOrDownMovement(4);

  if(leftMovement() || verifyButtonPress()) {
    if(brightnessCase)
      matrixOnOff(0);
    submenuPrinted = false;
    goToSubmenu();
  }
}

void setSound() {
  sounds = !sounds; //+ EEPROM
  lcd.setCursor(9,1);
  lcd.print(sounds ? F("ON ") : F("OFF"));
  inFunction = false;
}

void goToMainMenu() {                       // resetting the menu to reach main menu
  lcd.clear();
  submenuOption = 0;
  menuPrinted = false;
  // submenuPrinted = false;
  inSubmenu = false;
}

void goToSubmenu() {                        // resetting the menu to reach submenu
  lcd.clear();
  // submenuPrinted = false;
  inFunction = false;
  functionPrinted = false;
}

void printHighscores() {
  lcd.clear();
  float highscore;
  Username user;
  EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + 1 * floatSize, highscore);
  EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + 1 * floatSize, user.name);

  switch(highscoreOption) {
    case 0:
      lcd.setCursor(NONE, 1);
      lcd.print(F("2."));
      lcd.print(user.name);
      lcd.print(": " + String(highscore) + "s");
      EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + 0 * floatSize, highscore);
      EEPROM.get(startNicknames + (LEVEL - 1)* floatSize * numberOfSavings + 0 * floatSize, user.name);
      lcd.setCursor(NONE, NONE);
      lcd.print(F("1."));
      lcd.print(user.name);
      lcd.print(": " + String(highscore) + "s");
      break;
    case 1:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("2."));
      lcd.print(user.name);
      lcd.print(": " + String(highscore) + "s");
      EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + 2 * floatSize, highscore);
      EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + 2 * floatSize, user.name);
      lcd.setCursor(NONE, 1);
      lcd.print(F("3."));
      lcd.print(user.name);
      lcd.print(": " + String(highscore) + "s");
      break;
  }
}

void highscorePrintings() {
  if(!submenuPrinted) {
    printHighscores();
    submenuPrinted = true;
  }
  upOrDownMovement(1);
  if(leftMovement()) {
    inSubmenu = false;
    submenuPrinted = false;
    highscoreOption = 0;
    goToMainMenu();
  }
}

bool leftMovement() {
  unsigned int xValue = analogRead(xPin);

  if (xValue + joystickSensitivity < minimumThreshold) {        // verifying the joystick motion for x value
    if(!joystickLeftMoved)
      joystickLeftMoved = true;
  } else if(joystickLeftMoved) {
    joystickLeftMoved = false;
    return true;
  }
  return false;
}

bool rightMovement() {
  unsigned int xValue = analogRead(xPin);

  if (xValue + joystickSensitivity > maximumThreshold) {        // verifying the joystick motion for x value
    if(!joystickRightMoved)
      joystickRightMoved = true;
  } else if(joystickRightMoved) {
    joystickRightMoved = false;
    return true;
  }
  return false;
}

void upOrDownMovement(byte whereToTest) {
  unsigned int yValue = analogRead(yPin);  

  if (yValue + joystickSensitivity < minimumThreshold) {         // verifying the joystick motion for y value
    verifyUpOrDownMotion(whereToTest, 1);                        // and the fact that the user could go in the desired direction
  } else if (yValue - joystickSensitivity > maximumThreshold) {
    verifyUpOrDownMotion(whereToTest, -1);
  } else joystickMoved = false;
}

void verifyUpOrDownMotion(byte whereToTest, int distance) {
  if (!joystickMoved) {                                             // verifying if it's not true so it won't move more than 1 place at a time
    switch(whereToTest) {
      case 7: // how to play
        if(howToRow + distance >= NONE && howToRow + distance < howToRows) {
          howToRow += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
      case 6: // about message
        if(aboutRow + distance >= NONE && aboutRow + distance < aboutRows) {
          aboutRow += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
        break;
      case 0: // main menu options
        if (menuOption + distance >= NONE && menuOption + distance < menuOptions) {  // verifying that it can go to that path
          menuOption += distance;
          lcd.clear();
          menuPrinted = false;
          joystickMoved = true;                                         // setting it true so it won't move more than 1 place at a time
        }
        break;
      case 1: // highscore options
        if (highscoreOption + distance == NONE || highscoreOption + distance == 1) {
          highscoreOption += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
        break;
      case 2: // settings options
        if (settingsOption + distance >= NONE && settingsOption + distance < settingsOptions) {
          settingsOption += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
        break;
      case 3: // lcd brightness
        if(lcdBrightness - distance >= NONE && lcdBrightness - distance <= maximumLCDBrightness) {
          lcdBrightness -= distance;
          analogWrite(lcdBrightnessPin, lcdBrightness);
          printBrightness(true, 0);
        }
        break;
      case 4: // matrix brightness
        if(matrixBrightness - distance >= NONE && matrixBrightness - distance <= maximumMTXBrightness) {
          matrixBrightness -= distance;
          joystickMoved = true;
          lc.setIntensity(NONE, matrixBrightness); // TBA
          printBrightness(true, 1);
        }
        break;
      case 5: // username
        char userChar = username.name[usernameIndex];
        if(userChar - distance < 'A')
          userChar = 'Z';
        else if(userChar - distance > 'Z')
          userChar = 'A';
        else userChar -= distance;
        username.name[usernameIndex] = userChar;
        joystickMoved = true;
        printUsername(true);
        break;
    }
  }
}

// -----------------------------------------------------------------------------------------
// --------------------------------------- Matrix Game -------------------------------------
// -----------------------------------------------------------------------------------------

unsigned long getRandomSeed() {                             // generating a random seed
  unsigned long seed = NONE;
  seed = millis();
  for (int i = NONE; i < matrixSize; ++i)
      seed = seed + analogRead(i);
  return seed;
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

void setStartPosition() {
  clearMatrix();

  playerCol = random(matrixSize);
  playerRow = random(matrixSize);

  matrix[playerRow][playerCol] = 'S';
  positioned = true;
}

void reset() {
  won = false;
  delayed = false;
  count = NONE;
  score = -1;
  firstEndScreen = false;
  secondEndScreen = false;
  menuing = true;
  inSubmenu = false;
  userPrinted = false;
  setup();
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
      if(availableBombs) {
        availableBombs -= 1;
        bombRow = playerRow;
        bombCol = playerCol;
        matrix[bombRow][bombCol] = '3';
        startBombingTime = millis();
        bombPlacedTime = millis();
      }
    }
    buttonPressed = false; // resetting the value for the next press
  }
  if(!availableBombs) {
    if(currentTime - bombPlacedTime >= 1000) {                              // modifying the speed of the bomb blinking
      matrix[bombRow][bombCol] -= 1;
      printMatrix();
      bombPlacedTime = currentTime;
    }
    if(matrix[bombRow][bombCol] == '0') {                                   // if the bomb exploded, emptying the spot and clearing the walls
      matrix[bombRow][bombCol] = walls[NONE];
      bombed();
      availableBombs += 1;
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
  modified = false;
  userWinTime = millis();
  score = float(userWinTime - userStartTime) / 1000;                                  // displaying the time as seconds
  float highscores[numberOfSavings];
  Username usernames[numberOfSavings];
  for (int i = 0; i < numberOfSavings; i++) {
    EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscores[i]);                         // retrieve the existing high scores from EEPROM
    EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, usernames[i].name);    // retrieve the usernames of the owners of the high scores from EEPROM
  }

  for(int i = 0; i < numberOfSavings; i++)
    if(score < highscores[i] || !highscores[i]) {
      modified = true;
      for(int j = numberOfSavings - 1; j > i; j--) {
        highscores[j] = highscores[j - 1];
        strncpy(usernames[j].name, usernames[j-1].name, numberOfCharacters);
      }
      highscores[i] = score;
      strncpy(usernames[i].name, username.name, numberOfCharacters);
      break;
    }

  if(modified)
    for (int i = 0; i < numberOfSavings; i++) {
      EEPROM.put((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscores[i]);   // updating the EEPROM with the new highscores
      EEPROM.put(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, usernames[i].name);   // updating the EEPROM with the new usernames
    }

  firstEndScreen = true;
  return true;
}
