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

const byte rs = A4;                                         // used PINs
const byte en = 13;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 3;
const byte d7 = 4;
const byte lcdBrightnessPin = 5;

const byte yPin = A0;
const byte xPin = A1;

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const byte buttonPin = 8;

const byte buzzerPin = 9;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte lcdBrightness = EEPROM.read(startLCDB);
byte matrixBrightness = EEPROM.read(startMTXB);
bool sounds = EEPROM.read(startSound);

const byte matrixSize = 16;
const byte cameraSize = 8;
char matrix[matrixSize][matrixSize] = {
  {'!', '!', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!', '!', '!'},
  {'!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!'},
  {'!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!'},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!'},
  {'!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!'},
  {'!', '!', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!', '!', '!'},
};

const byte wallTypes = 2;
const char walls[wallTypes] = {' ', '#'};                   // no wall / wall
const char limitWall = '!';

const byte NONE = 0;                                        // levels
const byte EASY = 1;
const byte MEDIUM = 2;
const byte HARD = 3;

byte LEVEL = EASY;                                          // default level

byte xPos = NONE;                                           // variables to track the current and previous positions of the joystick-controlled LED
byte yPos = NONE;
byte xLastPos = NONE;
byte yLastPos = NONE;

const int BAUD = 9600;
const byte ASCII = 48;

byte menuOption = 0;                                        // menu option selected
byte submenuOption = 0;                                     // submenu option selected
byte highscoreOption = 0;
byte settingsOption = 0;
byte resetHighscoreOption = 0;

bool menuPrinted = false;                                   // if the text was printed for each (menu, submenu or function itself)
bool submenuPrinted = false;
bool functionPrinted = false;

bool inSubmenu = false;                                     // location of the user in the menu
bool inFunction = false;
unsigned long currentTime = millis();

const unsigned int joystickSensitivity = 50;                // joystick settings
unsigned int minimumThreshold = 350;
unsigned int maximumThreshold = 650;

bool joystickMoved = false;
bool joystickLeftMoved = false;
bool joystickRightMoved = false;

byte playerRow;                                             // user's coordinates
byte playerCol;

byte availableBombs = 1;                                    // number of available bombs at a certain time
byte maximumAvailableBombs;                                 // number of maximum available bombs
int bombRow[HARD] = {-1, -1, -1};
int bombCol[HARD] = {-1, -1, -1};

const int userBlinkingInterval = 500;                       // user blinking speed
const byte bombBlinkingInterval[3] = {50, 100, 200};        // currentBomb blinking speed

const byte placeBombSound = 600;                            // buzzer tones and tone time
const byte menuSound = 500;
const byte explodeSound = 350;
const byte soundTime = 200;

unsigned long startTime = millis();

unsigned long startBombingTime[HARD];
unsigned long bombPlacedTime[HARD];

unsigned long userStartTime;
unsigned long userWinTime;

bool blinkState = false;                                    // blink states
bool bombBlinkState[HARD] = {false, false, false};

const unsigned int debounceDelay = 50;                      // button settingss
bool buttonPressed = false;
unsigned long buttonPressedTime;

const byte numberOfCharacters = 4;
struct Username {
  char name[numberOfCharacters] = "";
};
Username username;
byte usernameIndex = 0;

byte aboutRow = 0;                                          // selected row in about / how to play section
byte howToRow = 0;

const byte aboutRows = 6;                                   // maximum counter for rows / options in certain functions
const byte menuOptions = 6;
const byte settingsOptions = 5;
const byte howToRows = 11;
const byte resetHighscoreOptions = 2;
const byte maximumLCDBrightness = 255;
const byte maximumMTXBrightness = 15;

bool menuing = false;                                       // current state of the game
bool settingUp = false;
bool gaming = false;
bool won = false;

bool printed = false;
const byte delayPeriod = 100;
int count = 0;                                              // counter for non-space, non-enter characters
bool delayed = false;
unsigned long delayTime;

char input[numberOfCharacters];

float score = -1;                                           // player score

bool firstEndScreen = false;
bool secondEndScreen = false;
bool displayedScreen = false;

bool modified = false;

bool userPrinted = false;
bool firstRun = false;

bool seeded = false;
bool positioned = false;

byte currentBomb = 0;

byte camera = 0;

void setup() {
  lc.shutdown(NONE, false);                                 // turn off power saving, enables display
  
  lc.setIntensity(NONE, matrixBrightness);                  // sets brightness (NONE~15 possible values)
  matrixOnOff(0);

  lcd.begin(16, 2);
  analogWrite(lcdBrightnessPin, lcdBrightness);
  initialMessage();

  if(!firstRun) {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(buzzerPin, OUTPUT);
    strcpy(username.name, "AAA\0");
    firstRun = true;
    // Serial.begin(BAUD);
  }
}

void displayBombentziuMatrix() {
  byte bombentziuMatrix[matrixSize][matrixSize] = {
    {0, 0, 0, 0, 0, 1, 1, 1}, 
    {0, 0, 0, 0, 1, 0, 1, 1}, 
    {0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 0, 1, 1, 1, 1, 0, 0} 
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, bombentziuMatrix[row][col]); 
}

void initialMessage() {
  lcd.setCursor(NONE, NONE);
  lcd.print(F("   Bombentziu"));
  lcd.setCursor(NONE, 1);
  lcd.print(F(" <Press Button>"));
  displayBombentziuMatrix();
}

void displayFirstEndScreen() {
  if(!displayedScreen) {                            // verifying if already printed so we prevent multiple useless printings
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
  if(verifyButtonPress()) {                         // verifying button press so we can switch to the next (and last) end screen
    displayedScreen = false;
    firstEndScreen = false;
    secondEndScreen = true;
  }
}

void displaySecondEndScreen() {
  if(!displayedScreen) {                            // verifying if already printed so we prevent multiple useless printings
    displayedScreen = true;
    lcd.clear();
    lcd.setCursor(NONE, NONE);
    lcd.print(F("Just "));
    lcd.print(score);
    lcd.print(F(" s!"));
    lcd.setCursor(NONE, 1);
    if(modified) {
      lcd.print("You are in TOP 3");
      modified = false;
      displayHighscoresMatrix();
    } else {
      lcd.print("Not in TOP 3");
      displayNoHighscoresMatrix();
    }
  }
  if(verifyButtonPress()) {                         // verifying button press so we can switch to the next state (menu state once again)
    displayedScreen = false;
    secondEndScreen = false;
  }
}

void loop() {
  currentTime = millis();
  if(menuing) {                                                   // user currently in menu
    mainMenu();
    if(!inSubmenu)
      upOrDownMovement(NONE);
  } else if(!gaming){
    waitForMenu();                                                // user currently in waiting screen
  } else if(gaming) {
    if(settingUp) {
      if(!seeded) {
        randomSeed(getRandomSeed());                              // seed the random number generator
        seeded = true;
      } else if(!positioned) {
        setStartPosition();                                       // sets the user's start position
      } else {
        fillMatrix();                                             // getting the matrix ready for the game to start
        printMatrix();
        seeded = false;
        positioned = false;
        settingUp = false;
      }
    } else {
      if(!won) {                                                  // player currently playing the game
        updateUserBlinking();
        movement();
        bombing();
        liveDisplaying();
      } else {                                                    // game ended
        if(firstEndScreen)                                        // displaying the last two screens before resetting
          displayFirstEndScreen();
        else if(secondEndScreen)
          displaySecondEndScreen();
        else reset();                                             // going back to the menu
      }
    }
  }
}

void liveDisplaying() {                                           // live displayings of the user's game stats (user + available bombs + time)
  if(!userPrinted) {
    userPrinted = true;
    lcd.clear();
    lcd.setCursor(NONE, NONE);
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
      menuing = true;                                                         // going to the menu
      lcd.clear();
      if(sounds)
        tone(buzzerPin, menuSound, soundTime);
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
      if(sounds)
        tone(buzzerPin, menuSound, soundTime);
      return true;
    }
    buttonPressed = false;                                                    // resetting the value for the next press
  }
  return false;
}

void displayMainMenuMatrix() {
  byte mainMenuMatrix[matrixSize][matrixSize] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {1, 1, 1, 0, 0, 1, 1, 1}, 
    {0, 1, 1, 0, 0, 1, 1, 0}, 
    {0, 1, 1, 0, 0, 1, 1, 0}, 
    {0, 0, 1, 0, 0, 1, 0, 0}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 1, 0, 0, 0, 0, 1, 1}, 
    {0, 1, 1, 1, 1, 1, 1, 0} 
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, mainMenuMatrix[row][col]); 
}

void mainMenu() {
  if (!menuPrinted) {                      // if menu wasn't printed, we are printing it (same for the other submenus and functions)
    printMainMenuCommands();
    displayMainMenuMatrix();
    menuPrinted = true;
  }

  if(!inSubmenu)
    if (verifyButtonPress())
      inSubmenu = true;
    
  if(inSubmenu) {
    switch (menuOption) {
      case 0:
        lcd.clear();
        startGame();
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
  if(LEVEL + 1 > HARD)          // verifying so it won't go to level 4 as it doesn't exist
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
      lcd.print(F("After that, you"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("have to start"));
      break;
    case 6:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("the game. Start"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("destroying the"));
      break;
    case 7:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("walls ASAP to"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("finish the game."));
      break;
    case 8:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("github.com/"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("leviaici for"));
      break;
    case 9:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("more details"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("about the game."));
      break;
    case 10:
      lcd.setCursor(NONE, NONE);
      lcd.print(F("Swipe left to"));
      lcd.setCursor(NONE, 1);
      lcd.print(F("leave the about."));
      break;
  }
}

void displayHowToPlayMatrix() {
  byte howToPlayMatrix[matrixSize][matrixSize] = {
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
      lc.setLed(NONE, row, col, howToPlayMatrix[row][col]); 
}

void howToPlay() {
  if(!submenuPrinted) {         // verifying if the submenu was printed for preventing multiple useless printings
    printHowToPlay();
    displayHowToPlayMatrix();
    submenuPrinted = true;
  }
  upOrDownMovement(7);          // verifying if the user wants to go upper or lower with the printing 
  if(leftMovement()) {          // verifying if the user wants to go back to the previous menu
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

void displayAboutMatrix() {
  byte aboutMatrix[matrixSize][matrixSize] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 1, 1, 0, 0, 1, 1, 0}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {0, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 0, 1, 1, 1, 1, 0, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}
  };

  for(int row = NONE; row < matrixSize; row++) 
      for(int col = NONE; col < matrixSize; col++) 
        lc.setLed(NONE, row, col, aboutMatrix[row][col]); 
}

void about() {
  if(!submenuPrinted) {
    printAbout();
    displayAboutMatrix();
    submenuPrinted = true;
  }
  upOrDownMovement(6);         // same logic as the howToPlay and the other ones
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
      lcd.print(F(">Sounds: "));
      lcd.print(sounds ? F("ON") : F("OFF"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(" Reset Highscore"));
      break;
    case 4:
      lcd.setCursor(NONE, NONE);
      lcd.print(F(" Sounds: "));
      lcd.print(sounds ? F("ON") : F("OFF"));
      lcd.setCursor(NONE, 1);
      lcd.print(F(">Reset Highscore"));
      break;
  } 
}

void saveSettingsData() {     // saving all the hardware settings in EEPROM
  EEPROM.update(startLCDB, lcdBrightness);
  EEPROM.update(startMTXB, matrixBrightness);
  EEPROM.update(startSound, sounds);
}

void displaySettingsMatrix() {
  byte settingsMatrix[matrixSize][matrixSize] = {
    {0, 0, 1, 1, 1, 1, 0, 0}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 0, 0}, 
    {1, 1, 1, 1, 1, 0, 0, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 0, 1, 1, 1, 1, 0, 0}, 
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, settingsMatrix[row][col]); 
}

void settings() {
  if(!submenuPrinted) {
    printSettings();
    displaySettingsMatrix();
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
      case 4:
        resetHighscore();
        break;
    }
  } else {
    upOrDownMovement(2);
    if(leftMovement()) {
      saveSettingsData();   // saving the hardware settings once we leave the settings menu for preventing multiple useless savings
      inSubmenu = false;
      submenuPrinted = false;
      settingsOption = 0;
      goToMainMenu();
    }
  }
}

void printResetHighscore(byte toUpdate) {
  if(!toUpdate) {
    lcd.clear();
    lcd.setCursor(NONE, NONE);
    lcd.print(F("Reset highscore?"));
    lcd.setCursor(NONE, 1);
    lcd.print(F("   <YES>  NO    "));
  } else {
    switch(resetHighscoreOption) {
      case 0:
        lcd.setCursor(NONE, 1);
        lcd.print(F("   <YES>  NO    "));
        break;
      case 1:
        lcd.setCursor(NONE, 1);
        lcd.print(F("    YES  <NO>   "));
        break;
    }
  }
}

void resetHighscore() {
  if(!functionPrinted) {
    printResetHighscore(false);
    functionPrinted = true;
  }

  if(leftMovement())              // verifying if the user wants to go to the option that it's in the left of the currently one
    verifyOptionInResetHighscore(-1);
  else if(rightMovement())        // verifying if the user wants to go to the option that it's in the right of the currently one
    verifyOptionInResetHighscore(1);

  if(verifyButtonPress()) {       // leaving the submenu when we press the button
    lcd.noBlink();                // stopping the lcd blinking once we leave the submenu
    verifyResetHighscore();
    submenuPrinted = false;
    goToSubmenu();
  }
}

void deleteHighscore() {
  float highscore = 0.0;
  Username user;
  strcpy(user.name, "AAA\0");
  for (int i = 0; i < numberOfSavings; i++) {
    EEPROM.put((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscore);                        
    EEPROM.put(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, user.name);   
  }
}

void verifyResetHighscore() {
  if(!resetHighscoreOption)
      deleteHighscore();
  resetHighscoreOption = 0;
}

void verifyOptionInResetHighscore(int distance) {
  if(resetHighscoreOption + distance >= NONE && resetHighscoreOption + distance < resetHighscoreOptions) {
    resetHighscoreOption += distance;
    printResetHighscore(true);
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
    lcd.blink();                              // blinking the character user is currently at (like a cursor)
  } else {
    lcd.setCursor(13 + usernameIndex, 1);
    lcd.blink();                              // blinking the character user is currently at (like a cursor)
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

  upOrDownMovement(5);            // verifying up or down movements of the joystick as the user might want to switch to a lower/upper char in ASCII
  
  if(leftMovement())              // verifying if the user wants to go to the char that it's in the left of the currently one
    verifyCursorChange(-1);       // verifying if the move is possible
  else if(rightMovement())        // verifying if the user wants to go to the char that it's in the right of the currently one
    verifyCursorChange(1);        // verifying if the move is possible

  if(verifyButtonPress()) {       // leaving the submenu when we press the button
    lcd.noBlink();                // stopping the lcd blinking once we leave the submenu
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

void matrixOnOff(bool print) {            // turning on/off the matrix, depends on the case (print value)
  for(int row = NONE; row < matrixSize; row++)
    for(int col = NONE; col < matrixSize; col++)
      lc.setLed(0, row, col, print);
}

void printBrightness(bool toUpdate, byte brightnessCase) {        // brightnessCase 1 - matrix menu, brightnessCase 0 - lcd menu
  if(!toUpdate) {                                                 // toUpdate 1 - means that the display is just updated and there is no need to clear the whole display
    if(brightnessCase) {
      matrixOnOff(1);                                             // turning on the matrix so the user can see the differences between the levels of the light intensity
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

  if(!brightnessCase)                                             // verifying the up or down joystick movements for each case (lcd/matrix menus)
    upOrDownMovement(3);
  else upOrDownMovement(4);

  if(leftMovement() || verifyButtonPress()) {                     // leaving the menu with a button press or swiping left gesture on joystick
    if(brightnessCase)                                            // if user in matrix menu, the matrix will shut down as it's not needed to be turned on anymore
      matrixOnOff(0);
    submenuPrinted = false;
    goToSubmenu();
  }
}

void setSound() {             // toggling on/off the sound and modifying the printings on the submenu
  sounds = !sounds;
  lcd.setCursor(9,1);
  lcd.print(sounds ? F("ON ") : F("OFF"));
  inFunction = false;
}

void goToMainMenu() {                       // resetting the menu to reach main menu
  lcd.clear();
  submenuOption = 0;
  menuPrinted = false;
  inSubmenu = false;
}

void goToSubmenu() {                        // resetting the menu to reach submenu
  lcd.clear();
  inFunction = false;
  functionPrinted = false;
}

bool verifyHighscore(int highscoreIndex) {
  float highscore;
  EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + highscoreIndex * floatSize, highscore);
  if(highscore)
    return true;
  return false;
}

void printHighscores() {      // printing the top 3 highscores for the currently selected level
  if(verifyHighscore(0)) {
    lcd.clear();
    float highscore;
    Username user;
    EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + 1 * floatSize, highscore);                   // getting the highscore from eeprom
    EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + 1 * floatSize, user.name);  // getting the username of the highscore's owner from eeprom

    switch(highscoreOption) {
      case 0:
        lcd.setCursor(NONE, 1);
        if(verifyHighscore(1)) {
          lcd.print(F("2."));
          lcd.print(user.name);
          lcd.print(": " + String(highscore) + "s");
        } else lcd.print(F("NO HIGHSCORE"));
        EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + 0 * floatSize, highscore);
        EEPROM.get(startNicknames + (LEVEL - 1)* floatSize * numberOfSavings + 0 * floatSize, user.name);
        lcd.setCursor(NONE, NONE);
        lcd.print(F("1."));
        lcd.print(user.name);
        lcd.print(": " + String(highscore) + "s");
        break;
      case 1:
        lcd.setCursor(NONE, NONE);
        if(verifyHighscore(1)) {
          lcd.print(F("2."));
          lcd.print(user.name);
          lcd.print(": " + String(highscore) + "s");
        } else lcd.print(F("NO HIGHSCORE"));
        EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + 2 * floatSize, highscore);
        EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + 2 * floatSize, user.name);
        lcd.setCursor(NONE, 1);
        if(verifyHighscore(2)) {
          lcd.print(F("3."));
          lcd.print(user.name);
          lcd.print(": " + String(highscore) + "s");
        } else lcd.print(F("NO HIGHSCORE"));
        break;
    }
  } else {
    lcd.setCursor(NONE, NONE);
    lcd.print(F("  NO HIGHSCORE  "));
    lcd.setCursor(NONE, 1);
    lcd.print(F("   SWIPE LEFT   "));
  }
}

void displayNoHighscoresMatrix() {
  byte noHighscoresMatrix[matrixSize][matrixSize] = {
    {1, 0, 1, 0, 0, 1, 0, 1}, 
    {0, 1, 0, 0, 0, 0, 1, 0}, 
    {0, 1, 0, 0, 0, 0, 1, 0}, 
    {0, 1, 0, 0, 0, 0, 1, 0}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 1, 1, 0, 0, 1, 1, 1}, 
    {1, 1, 0, 0, 0, 0, 1, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, noHighscoresMatrix[row][col]); 
}

void displayHighscoresMatrix() {
  byte highscoresMatrix[matrixSize][matrixSize] = {
    {0, 1, 0, 1, 1, 0, 1, 0}, 
    {1, 0, 1, 1, 1, 1, 0, 1}, 
    {1, 0, 1, 1, 1, 1, 0, 1}, 
    {1, 0, 1, 1, 1, 1, 0, 1}, 
    {0, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 0, 1, 1, 1, 1, 0, 0}, 
    {0, 1, 1, 1, 1, 1, 1, 0}, 
  };

  for(int row = NONE; row < matrixSize; row++) 
    for(int col = NONE; col < matrixSize; col++) 
      lc.setLed(NONE, row, col, highscoresMatrix[row][col]); 
}

void highscorePrintings() {
  if(!submenuPrinted) {
    printHighscores();
    displayHighscoresMatrix();
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

bool leftMovement() {                                           // verifying left movements for joystick
  unsigned int xValue = analogRead(xPin);

  if (xValue + joystickSensitivity < minimumThreshold) {        // verifying the joystick motion for x value
    if(!joystickLeftMoved)
      joystickLeftMoved = true;
  } else if(joystickLeftMoved) {
    joystickLeftMoved = false;
    if(sounds)
      tone(buzzerPin, menuSound, soundTime);
    return true;
  }
  return false;
}

bool rightMovement() {                                          // verifying right movements for joystick
  unsigned int xValue = analogRead(xPin);

  if (xValue + joystickSensitivity > maximumThreshold) {        // verifying the joystick motion for x value
    if(!joystickRightMoved)
      joystickRightMoved = true;
  } else if(joystickRightMoved) {
    joystickRightMoved = false;
    if(sounds)
      tone(buzzerPin, menuSound, soundTime);
    return true;
  }
  return false;
}

void upOrDownMovement(byte whereToTest) {                        // verifying up/down movements for joystick
  unsigned int yValue = analogRead(yPin);  

  if (yValue + joystickSensitivity < minimumThreshold) {         // verifying the joystick motion for y value
    verifyUpOrDownMotion(whereToTest, 1);                        // and the fact that the user could go in the desired direction
  } else if (yValue - joystickSensitivity > maximumThreshold) {
    verifyUpOrDownMotion(whereToTest, -1);
  } else joystickMoved = false;
}

void quickSound() {
  if(sounds)
      tone(buzzerPin, menuSound, soundTime);
}

void verifyUpOrDownMotion(byte whereToTest, int distance) {
  if (!joystickMoved) {                                             // verifying if it's not true so it won't move more than 1 place at a time
    switch(whereToTest) {
      case 6: // about message
        if(aboutRow + distance >= NONE && aboutRow + distance < aboutRows) {
          quickSound();
          aboutRow += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
        break;
      case 7: // how to play
        if(howToRow + distance >= NONE && howToRow + distance < howToRows) {
          quickSound();
          howToRow += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
      case 0: // main menu options
        if (menuOption + distance >= NONE && menuOption + distance < menuOptions) {  // verifying that it can go to that path
          quickSound();
          menuOption += distance;
          lcd.clear();
          menuPrinted = false;
          joystickMoved = true;                                         // setting it true so it won't move more than 1 place at a time
        }
        break;
      case 1: // highscore options
        if (highscoreOption + distance == NONE || highscoreOption + distance == 1) {
          quickSound();
          highscoreOption += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
        break;
      case 2: // settings options
        if (settingsOption + distance >= NONE && settingsOption + distance < settingsOptions) {
          quickSound();
          settingsOption += distance;
          lcd.clear();
          submenuPrinted = false;
          joystickMoved = true;
        }
        break;
      case 3: // lcd brightness
        if(lcdBrightness - distance >= NONE && lcdBrightness - distance <= maximumLCDBrightness) {
          quickSound();
          lcdBrightness -= distance;
          analogWrite(lcdBrightnessPin, lcdBrightness);
          printBrightness(true, 0);
        }
        break;
      case 4: // matrix brightness
        if(matrixBrightness - distance >= NONE && matrixBrightness - distance <= maximumMTXBrightness) {
          quickSound();
          matrixBrightness -= distance;
          joystickMoved = true;
          lc.setIntensity(NONE, matrixBrightness);
          printBrightness(true, 1);
        }
        break;
      case 5: // username
        quickSound();
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
  for (int i = NONE; i < cameraSize; ++i)
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

  playerCol = random(1, cameraSize);
  playerRow = random(1, cameraSize);

  matrix[playerRow][playerCol] = 'S';
  positioned = true;
}

void reset() {                                                // resetting the game into menu state
  won = false;
  delayed = false;
  count = NONE;
  score = -1;
  firstEndScreen = false;
  secondEndScreen = false;
  menuing = true;
  inSubmenu = false;
  userPrinted = false;
  currentBomb = 0;
  camera = 0;
  for(int i = NONE; i < HARD; i++) {
    bombRow[i] = -1;
    bombCol[i] = -1;
    bombBlinkState[i] = false;
  }
  setup();
}

bool checkBomb() {
  for(int i = NONE; i < maximumAvailableBombs; i++)
    if(bombRow[i] == playerRow && bombCol[i] == playerCol)
      return false;
  return true;
}

byte checkFreeSpot() {
  for(int i = NONE; i < maximumAvailableBombs; i++)
    if(bombRow[i] == -1)
      return i;
}

void updateBombLED(byte explosionRow, byte explosionCol, bool ledState) {
  int cameraOfBomb = int(explosionRow / cameraSize) * 2 + int(explosionCol / cameraSize);
  if(cameraOfBomb == camera)
    switch(camera) {
      case 0:
        lc.setLed(NONE, explosionRow, explosionCol, ledState);
        break;
      case 1:
        lc.setLed(NONE, explosionRow, explosionCol - cameraSize, ledState);
        break;
      case 2:
        lc.setLed(NONE, explosionRow - cameraSize, explosionCol, ledState);
        break;
      case 3:
        lc.setLed(NONE, explosionRow - cameraSize, explosionCol - cameraSize, ledState);
        break;
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
    if (buttonPressed && currentTime - buttonPressedTime >= debounceDelay) { // placing the currentBomb
      if(availableBombs && checkBomb()) {
        if(sounds)
          tone(buzzerPin, placeBombSound, soundTime);
        byte freeSpot = checkFreeSpot();
        availableBombs -= 1;
        bombRow[freeSpot] = playerRow;
        bombCol[freeSpot] = playerCol;
        matrix[bombRow[freeSpot]][bombCol[freeSpot]] = '3';
        startBombingTime[freeSpot] = millis();
        bombPlacedTime[freeSpot] = millis();
      }
    }
    buttonPressed = false; // resetting the value for the next press
  }
  if(availableBombs < maximumAvailableBombs) {
    if(bombRow[currentBomb] != -1) {
      if(currentTime - bombPlacedTime[currentBomb] >= 1000 && matrix[bombRow[currentBomb]][bombCol[currentBomb]] > '0') {                              // modifying the speed of the currentBomb blinking
        matrix[bombRow[currentBomb]][bombCol[currentBomb]] -= 1;
        bombPlacedTime[currentBomb] = currentTime;
      }
      if(matrix[bombRow[currentBomb]][bombCol[currentBomb]] <= '0') {                                   // if the currentBomb exploded, emptying the spot and clearing the walls
        bombed(currentBomb);
      } else if(currentTime - startBombingTime[currentBomb] >= bombBlinkingInterval[matrix[bombRow[currentBomb]][bombCol[currentBomb]] - 1 - ASCII]) {
          bombBlinkState[currentBomb] = !bombBlinkState[currentBomb];                                   // walls that will be affected will start blinking
          if(bombRow[currentBomb] - 1 >= NONE && matrix[bombRow[currentBomb] - 1][bombCol[currentBomb]] == walls[1]) {
            updateBombLED(bombRow[currentBomb] - 1, bombCol[currentBomb], bombBlinkState[currentBomb]);
          }
          if(bombRow[currentBomb] + 1 < matrixSize && matrix[bombRow[currentBomb] + 1][bombCol[currentBomb]] == walls[1]) {
            updateBombLED(bombRow[currentBomb] + 1, bombCol[currentBomb], bombBlinkState[currentBomb]);
          }
          if(bombCol[currentBomb] - 1 >= NONE && matrix[bombRow[currentBomb]][bombCol[currentBomb] - 1] == walls[1]) {
            updateBombLED(bombRow[currentBomb], bombCol[currentBomb] - 1, bombBlinkState[currentBomb]);
          }
          if(bombCol[currentBomb] + 1 < matrixSize && matrix[bombRow[currentBomb]][bombCol[currentBomb] + 1] == walls[1]) {
            updateBombLED(bombRow[currentBomb], bombCol[currentBomb] + 1, bombBlinkState[currentBomb]);
          }
          startBombingTime[currentBomb] = currentTime;
      }
    }
    if(currentBomb + 1 == maximumAvailableBombs)
      currentBomb = NONE;
    else currentBomb += 1;
  }
}

void bombed(int currentBomb) {                                               // clearing the destroyed walls
  if(bombRow[currentBomb] - 1 >= NONE && matrix[bombRow[currentBomb] - 1][bombCol[currentBomb]] == walls[1]) {
    updateBombLED(bombRow[currentBomb] - 1, bombCol[currentBomb], false);
    matrix[bombRow[currentBomb] - 1][bombCol[currentBomb]] = walls[NONE];
  }
  if(bombRow[currentBomb] + 1 < matrixSize && matrix[bombRow[currentBomb] + 1][bombCol[currentBomb]] == walls[1]) {
    updateBombLED(bombRow[currentBomb] + 1, bombCol[currentBomb], false);
    matrix[bombRow[currentBomb] + 1][bombCol[currentBomb]] = walls[NONE];
  }
  if(bombCol[currentBomb] - 1 >= NONE && matrix[bombRow[currentBomb]][bombCol[currentBomb] - 1] == walls[1]) {
    updateBombLED(bombRow[currentBomb], bombCol[currentBomb] - 1, false);
    matrix[bombRow[currentBomb]][bombCol[currentBomb] - 1] = walls[NONE];
  }
  if(bombCol[currentBomb] + 1 < matrixSize && matrix[bombRow[currentBomb]][bombCol[currentBomb] + 1] == walls[1]) {
    updateBombLED(bombRow[currentBomb], bombCol[currentBomb] + 1, false);
    matrix[bombRow[currentBomb]][bombCol[currentBomb] + 1] = walls[NONE];
  }

  matrix[bombRow[currentBomb]][bombCol[currentBomb]] = walls[NONE];
  bombRow[currentBomb] = -1;
  availableBombs += 1;
  if(sounds)
    tone(buzzerPin, explodeSound, soundTime);

  if (checkWin()) {                                           // checking user's win
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

void updateUserOnLED(bool on) {
  switch(camera) {
    case 0:
      lc.setLed(NONE, playerRow, playerCol, on);
      break;
    case 1:
      lc.setLed(NONE, playerRow, playerCol - cameraSize, on);
      break;
    case 2:
      lc.setLed(NONE, playerRow - cameraSize, playerCol, on);
      break;
    case 3:
      lc.setLed(NONE, playerRow - cameraSize, playerCol - cameraSize, on);
      break;
  }
}

void updateUserBlinking() {
  if (currentTime - startTime >= userBlinkingInterval) { // blinking state of the cursor
    blinkState = !blinkState;
    updateUserOnLED(blinkState);
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

void verifyChangingCamera() {
  int actualCamera = int(playerRow / cameraSize) * 2 + int(playerCol / cameraSize);
  if(actualCamera != camera) {
    camera = actualCamera;
    printMatrix();
  }
}

void verifyJoystickMotion(int posY, int posX) {
  if (!joystickMoved) {                   // verifying if it's not true so it won't move more than 1 place at a time
    if (verifyModifyState(posY, posX)) {  // verifying that it can go to that path
      updateUserOnLED(false);
      // lc.setLed(NONE, playerRow, playerCol, false);
      playerRow += posY;
      playerCol += posX;
      verifyChangingCamera();
      updateUserOnLED(true);
      // lc.setLed(NONE, playerRow, playerCol, true);
      startTime = currentTime;
      joystickMoved = true;               // setting it true so it won't move more than 1 place at a time
    }
  }
}

bool verifyModifyState(int posY, int posX) {
  if(matrix[playerRow + posY][playerCol + posX] == walls[1] || matrix[playerRow + posY][playerCol + posX] == limitWall)      // verifying if a wall is there
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
      numHash = 130;
      maximumAvailableBombs = 3;
      availableBombs = 3;
      break;
    case 2:
      numHash = 170;
      maximumAvailableBombs = 2;
      availableBombs = 2;
      break;
    case 3:
      numHash = 200;
      maximumAvailableBombs = 1;
      availableBombs = 1;
      break;
  }

  for (int i = NONE; i < numHash; i++) {    // filling the matrix randomly with the desired number of walls
    int row = random(1, matrixSize - 1);                // first camera
    int col = random(1, matrixSize - 1);
    if(matrix[row][col] == walls[NONE])     // verifying if we don't put a wall on the player starting position or an already existing wall
      matrix[row][col] = walls[1];
    else i -= 1;
  }

  matrix[playerRow][playerCol] = ' ';       // clearing the user's place in matrix
  userStartTime = millis();
}

void clearMatrix() {
  for (int i = 1; i < matrixSize - 1; i++)         // emptying the matrix
    for (int j = 1; j < matrixSize - 1; j++) 
      matrix[i][j] = walls[NONE];
}

void printMatrix() {
  switch(camera) {
    case 0:
      for (int row = NONE; row < cameraSize; row++)
        for (int col = NONE; col < cameraSize; col++)
          lc.setLed(NONE, row, col, matrix[row][col] == walls[1] || matrix[row][col] == limitWall ? true : false);  // turns on LED at col, row
      break;
    case 1:
      for (int row = NONE; row < cameraSize; row++)
        for (int col = cameraSize; col < matrixSize; col++)
          lc.setLed(NONE, row, col - cameraSize, matrix[row][col] == walls[1] || matrix[row][col] == limitWall ? true : false);  // turns on LED at col, row
      break;
    case 2:
      for (int row = cameraSize; row < matrixSize; row++)
        for (int col = NONE; col < cameraSize; col++)
          lc.setLed(NONE, row - cameraSize, col, matrix[row][col] == walls[1] || matrix[row][col] == limitWall ? true : false);  // turns on LED at col, row
      break;
    case 3:
      for (int row = cameraSize; row < matrixSize; row++)
        for (int col = cameraSize; col < matrixSize; col++)
          lc.setLed(NONE, row - cameraSize, col - cameraSize, matrix[row][col] == walls[1] || matrix[row][col] == limitWall ? true : false);  // turns on LED at col, row
      break;
  }
}

bool checkWin() {
  for (int row = 1; row < matrixSize - 1; row++)                                                                   // checking for any walls so it's not a win
    for (int col = 1; col < matrixSize - 1; col++)
      if (matrix[row][col] != ' ')
        return false;
  modified = false;
  userWinTime = millis();
  score = float(userWinTime - userStartTime) / 1000;                                                              // displaying the time as seconds
  float highscores[numberOfSavings];
  Username usernames[numberOfSavings];
  for (int i = 0; i < numberOfSavings; i++) {
    EEPROM.get((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscores[i]);                         // retrieve the existing high scores from EEPROM
    EEPROM.get(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, usernames[i].name);    // retrieve the usernames of the owners of the high scores from EEPROM
  }

  for(int i = 0; i < numberOfSavings; i++)
    if(score < highscores[i] || !highscores[i]) {
      modified = true;                                                                                            // the highscores are modified
      for(int j = numberOfSavings - 1; j > i; j--) {
        highscores[j] = highscores[j - 1];
        strncpy(usernames[j].name, usernames[j-1].name, numberOfCharacters);
      }
      highscores[i] = score;
      strncpy(usernames[i].name, username.name, numberOfCharacters);
      break;
    }

  if(modified)                                                                                                     // if the highscores are modified, we are updating it to eeprom
    for (int i = 0; i < numberOfSavings; i++) {
      EEPROM.put((LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, highscores[i]);                        // updating the EEPROM with the new highscores
      EEPROM.put(startNicknames + (LEVEL - 1) * floatSize * numberOfSavings + i * floatSize, usernames[i].name);   // updating the EEPROM with the new usernames
    }

  firstEndScreen = true;                                                                                           // switching to the first end screen
  return true;
}
