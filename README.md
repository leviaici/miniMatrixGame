# Bombentziu

## Introduction
&emsp;Welcome to Bombentziu, an exhilarating 8x8 LED Matrix Game enhanced with an LCD menu for seamless customization and live updates during the player's game (username, time and bombs available at the time).

## About The Game

<details>
<summary><h3>Backstory Of The Game</h3></summary>

#### Why did I choose this game idea?
&emsp;I chose to develop Bombentziu, a Bomberman-style game because of its timeless appeal and straightforward yet entertaining gameplay. The nostalgic memories of playing this type of game with friends sparked my enthusiasm to recreate that shared joy and strategic excitement in a modern gaming experience.<br><br>

#### Where did I encounter some problems?
  <ol>- Generating the randomly spawned walls so it can be fun to play more than once.</ol>
  <ol>- Dividing the whole map into four different rooms and increasing the size of the map from 8x8 to 16x16.</ol>
  <ol>- Updating the whole map and the blinkings on the 8x8 LED Matrix.</ol>
  <ol>- Implementing the maximum number of bombs at a time for each difficulty (once one difficulty was ready, all of them were).</ol>
</details>

<details>
<summary><h3>Game Details</h3></summary>
&emsp;Players can tailor their experience by adjusting settings like username, difficulty, LCD and LED Matrix brightness, and toggle sounds on or off.
<br>&emsp;Once customized, players find themselves in a dynamic map divided into four rooms (each of the different room has a corner wall that cannot be destroyed, and doesn't need to be destroyed, to help the user guide himself and to let him know in what room it currently is), where strategic bomb placement (the upper, lower, right and left walls will be affected and will start blinking once a bomb is placed - if walls exist in the bomb's range) is key to demolishing randomly spawned walls. The higher the difficulty, the more walls appear, and the fewer bombs are available at a time.
<br>&emsp;Each time a bomb explodes, another one will appear in the player's inventory (EASY - maximum 3, MEDIUM - maximum 2, HARD - maximum 1, all at a time). Navigate through empty pathways as you cannot go through the walls unless they are destroyed, destroy all walls to complete the level, and aim for the highscore by finishing quickly.
<br>&emsp;Discover the unique charm of Bombentziu as you master the art of explosive strategy!
</details>

<details>
<summary><h3>Features</h3></summary>

<ol><br><b>Dynamic Difficulty:</b> Choose the difficulty level (1 to 3) at the start to control the complexity of the game.<br></ol>
<ol><b>Randomly Generated Map:</b> The game generates a unique map on the LED matrix for each playthrough, adding variety and challenge.<br></ol>
<ol><b>4 Different Rooms:</b> Each level has 4 different rooms (the harder the level, the more walls spawned on each room) that you can navigate through using the joystick. If there is not a wall blocking you from doing this, you can go from one room to another via the margins of the current room you are in. Each room has a corner "wall" that lets you know what room you are in at the moment.<br></ol>
<ol><b>Joystick Control:</b> Navigate through the maze using a joystick, providing a responsive and intuitive user experience.<br></ol>
<ol><b>Bombs Away!:</b> Deploy bombs by clicking on the physical button to clear walls and make your way through the maze.<br></ol>
<ol><b>EEPROM High Scores:</b> Your best times are saved in the Arduino's EEPROM. If you beat your previous high score, it will be updated.<br></ol>
<ol><b>Customize:</b> Set the brightnesses for LCD or Matrix using Settings Menu. Toggle the sound on/off. All these are saved in EEPROM.<br></ol>
<ol><b>About:</b> Some words about me, the creator.<br></ol>
<ol><b>How To Play:</b> Small tutorial of how to use and play the game.<br></ol>
<ol><b>Live Updates!:</b> Whenever the user is playing the game, live updates will be displayed on the LCD (such like username, number of available bombs and his time).<br></ol>
<ol><b>Game Reset:</b> After completing the game, press the physical button to go back to the main menu where you can start a new challenge.</ol></details>


<details>
<summary><h3>Menu Structure</h3></summary>

<ol><br><b>Start Game:</b> Switching to the game mode from the current menu mode.<br></ol>
<ol><b>Level:</b> The game comes with 3 different difficulties (Easy, Medium, Hard). Choose your desired one.<br></ol>
<ol><b>Highscore:</b> Navigate through the highscores (TOP 3 best times) for your selected difficulty. If you change the difficulty, the highscores will be shown for the new selected level. Once you want to leave the submenu, swipe left.<br></ol>
<ol><b>Settings:</b> Select the submenu you would like to personalize.<br><br>
  <ol><b>- Set username:</b> Select your to-be username. You can switch from character to character using swipe left/right gestures. If you want to change the selected character, user swipe up/down gestures on the joystick. The selected character will be blinking. Click the button when ready.</ol>
  <ol><b>- LCD Brightness:</b> Swipe up/down to select your desired LCD brightness. You can either click the button or swipe left when ready.</ol>
  <ol><b>- MTX Brightness:</b> Swipe up/down to select your desired Matrix brightness. You can either click the button or swipe left when ready.</ol>
  <ol><b>- Sounds:</b> The printed value represents the current value for the sound effects. Once you press it, the opposite will be toggled and printed.</ol>
  <ol><b>- Highscore Reset:</b> Once you press it, a text will be printed and the game will wait for your input if you are sure you want to reset the highscores for the currently selected difficulty. If you press "YES", the highscores of the current difficulty will be deleted from EEPROM.</ol>
</ol>
<ol><b>About:</b> Some words about me, the creator. You can swipe up/down to scroll the text. Once you want to leave the submenu, swipe left.<br></ol>
<ol><b>How To Play:</b> Small tutorial of how to use and play the game. You can swipe up/down to scroll the text. Once you want to leave the submenu, swipe left.</ol></details>

<details>
<summary><h3>How To Use</h3></summary>

<ol><b>Power On:</b> Connect your Arduino Uno and power it on.<br></ol>
<ol><b>Set Username:</b> Choose the username (3 characters maximum) by entering the corresponding username using the joystick (Settings -> Set username).<br></ol>
<ol><b>Set Difficulty:</b> Choose the difficulty level (1 to 3) by entering the corresponding level using the button.<br></ol>
<ol><b>Start the Game:</b> Press the button to start the game via the menu.<br></ol>
<ol><b>Navigate the Map:</b> Use the joystick to move through the map. You can go from one room to another by using the margins of the current room you are in (the one displayed on the 8x8 LED matrix).<br></ol>
<ol><b>Place Bombs:</b> Click the physical button to place bombs strategically and clear walls. The affected walls will be blinking until the bomb explodes and the walls disappear.<br></ol>
<ol><b>Complete the Game:</b> In order to finish the level, you have to succesfully destroy all the walls from all the four rooms. Try ripping the walls off as fast as possible!<br></ol>
<ol><b>Beat the High Score:</b> Your best times are saved in EEPROM. Beat your previous high scores!<br></ol>
<ol><b>Game Reset:</b> After completing the game, you are free to navigate the menu so you can start the game once again.</ol></details>
  
<details>
<summary><h3>Hardware Used And Showcase</h3></summary>
<img src = 'https://github.com/leviaici/miniMatrixGame/blob/main/Mini_Matrix_Game/IMG_7184.jpeg' align="right" width = 300>
  
- [X] Arduino UNO Board
- [X] Potentiometer
- [X] LCD
- [X] Joystick
- [X] 8x8 LED Matrix
- [X] Buzzer
- [X] MAX7219
- [X] Push-Button
- [X] Resistors, capacitors and wires as needed
- [X] Breadboard
- [X] Yotube Link (video showcase): https://youtu.be/ymxeLCfF6EE

</details>

## Notes

&emsp;Please note that these homework assignments are part of my academic coursework at the University of Bucharest. Feel free to explore the code, documentation, and solutions to gain insights into the exciting world of robotics and computer science.

## Disclaimer
&emsp;This repository is for educational purposes, and you are encouraged to learn from it. However, please avoid direct copying of code for your own coursework.
