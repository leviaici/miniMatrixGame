# miniMatrixGame

<details>
<summary><h2>Homework 7: Mini Matrix Game + Menu (Update of Homework 6)</h2></summary>
<b>Technical Task -></b>
Small game on the 8x8 matrix. The basic idea of the game is that it generates random walls on the map (50% - 75% of the map) and then you move around with the player and destroy them.

<br><b>Features</b>

<ol><br><b>Dynamic Difficulty:</b> Choose the difficulty level (1 to 3) at the start to control the complexity of the maze.<br></ol>
<ol><b>Randomly Generated Map:</b> The game generates a unique map on the LED matrix for each playthrough, adding variety and challenge.<br></ol>
<ol><b>Joystick Control:</b> Navigate through the maze using a joystick, providing a responsive and intuitive user experience.<br></ol>
<ol><b>Bombs Away!:</b> Deploy bombs by clicking on the physical button to clear walls and make your way through the maze.<br></ol>
<ol><b>EEPROM High Scores:</b> Your best times are saved in the Arduino's EEPROM. If you beat your previous high score, it will be updated.<br></ol>
<ol><b>Customize:</b> Set the brightnesses for LCD or Matrix using Settings Menu. Toggle the sound on/off. All these are saved in EEPROM.<br></ol>
<ol><b>About:</b> Some words about me, the creator.<br></ol>
<ol><b>How To Play:</b> Small tutorial of how to use and play the game.<br></ol>
<ol><b>Live Updates!:</b> Whenever the user is playing the game, live updates will be displayed on the LCD (such like username, number of available bombs and his time).<br></ol>
<ol><b>Game Reset:</b> After completing the maze, press the physical button to reset the game and start a new challenge.<br></ol>


<br><b><h3>Menu Structure</h3></b>

<ol><br><b>Start Game:</b> Switching to the game mode from the current menu mode.<br></ol>
<ol><b>Level:</b> The game comes with 3 different difficulties (Easy, Medium, Hard). Choose your desired one.<br></ol>
<ol><b>Highscore:</b> Navigate through the highscores (TOP 3 best times) for your selected difficulty. If you change the difficulty, the highscores will be shown for the new selected level. Once you want to leave the submenu, swipe left.<br></ol>
<ol><b>Settings:</b> Select the submenu you would like to personalize.<br><br>
  <ol><b>- Set username:</b> Select your to-be username. You can switch from character to character using swipe left/right gestures. If you want to change the selected character, user swipe up/down gestures on the joystick. The selected character will be blinking. Click the button when ready.</ol>
  <ol><b>- LCD Brightness:</b> Swipe up/down to select your desired LCD brightness. You can either click the button or swipe left when ready.</ol>
  <ol><b>- MTX Brightness:</b> Swipe up/down to select your desired Matrix brightness. You can either click the button or swipe left when ready.</ol>
  <ol><b>- Sounds:</b> The printed value represents the current value for the sound effects. Once you press it, the opposite will be toggled and printed.</ol>
</ol>
<ol><b>About:</b> Some words about me, the creator. You can swipe up/down to scroll the text. Once you want to leave the submenu, swipe left.<br></ol>
<ol><b>How To Play:</b> Small tutorial of how to use and play the game. You can swipe up/down to scroll the text. Once you want to leave the submenu, swipe left.<br></ol>

<br><b><h3>How to Use</h3></b>

<ol><b>Power On:</b> Connect your Arduino Uno and power it on.<br></ol>
<ol><b>Set Username:</b> Choose the username (3 characters maximum) by entering the corresponding username using the joystick (Settings -> Set username).<br></ol>
<ol><b>Set Difficulty:</b> Choose the difficulty level (1 to 3) by entering the corresponding levev using the button.<br></ol>
<ol><b>Start the game:</b> Press the button to start the game via the menu.<br></ol>
<ol><b>Navigate the Maze:</b> Use the joystick to move through the maze.<br></ol>
<ol><b>Place Bombs:</b> Click the physical button to place bombs strategically and clear walls.<br></ol>
<ol><b>Beat the High Score:</b> Your best times are saved in EEPROM. Beat your previous high scores!<br></ol>
<ol><b>Game Reset:</b> After completing the maze, you are free to navigate the menu so you can start the game once again.<br></ol>
  
<br><b><h3>To do/use list:</h3></b><br>
<img src = 'https://github.com/leviaici/miniMatrixGame/blob/main/Mini_Matrix_Game/IMG_6648.jpeg' align="right" width = 300>
  
- [X] Arduino UNO Board
- [X] Potentiometer
- [X] LCD
- [X] Joystick
- [X] 8x8 LED Matrix
- [X] MAX7219
- [X] Push-Button
- [X] Resistors, capacitors and wires as needed
- [X] Breadboard 
- [X] Arduino Code
- [X] Yotube Link: https://www.youtube.com/watch?v=WrsUc29QS9I
- [X] Setup photo
</details><br>
