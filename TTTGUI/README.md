# Tic-Tac-Toe Game with AI

This is a classic Tic-Tac-Toe game where players can challenge both AI and other players. The AI has three difficulty levels: Easy, Medium, and Hard. The game uses a graphical user interface (GUI) powered by the Raylib library.

### Features

- **Three difficulty levels for AI**: 
  - **Easy**: Uses Imperfect Minimax AI.
  - **Medium**: Uses Q-Learning AI.
  - **Hard**: Uses Perfect Minimax AI.
- **PvP Mode**: Play against another player.
- **PvAI Mode**: Play against the AI.
- **Winning Condition**: Align three symbols (X or O) horizontally, vertically, or diagonally to win.

### Instructions for Running the Code

#### Prerequisites

Before you can run the game, ensure you have the following installed:

- **Raylib**: The C graphics library used for the game interface. You can follow the [Raylib installation guide](https://www.raylib.com) to install Raylib on your system.

#### Running the Game

##### Navigate to the Project Folder
1. Open your terminal (or command prompt).
2. Navigate to the folder where the project files(TTTGUI) are stored.

##### Compile the Game
To compile the source code, run the `compile.bat` batch file. This will automatically compile the source code and create the executable.

```bash
compile.bat 
```

##### Run the Game
After running the compile.bat, the executable ttt_gui.exe will be ready to run.
```bash
./ttt_gui.exe
```

#####Gameplay Instructions

Select Difficulty:

1. Easy: AI uses Imperfect Minimax for a simpler challenge.

2. Medium: AI uses Q-Learning for a more strategic challenge.

3. Hard: AI uses Perfect Minimax for the toughest challenge.

PvP Mode: Challenge another player in a two-player game mode.

PvAI Mode: Challenge the AI with your selected difficulty.

Winning Conditions: To win, align three of your symbols (X or O) either horizontally, vertically, or diagonally on the grid.
