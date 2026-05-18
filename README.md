# Chess in C++

<img width="821" height="856" alt="Screenshot 2026-05-18 120748" src="https://github.com/user-attachments/assets/f1e2a9dc-8120-4804-99b7-82eab72f4150" />

## Overview
This chess program employs the use of modern C++ design patterns, graphical display, and modular design.

 The “physical” chess model is simulated with an 8x8 Board implemented as a 2D vector storing Piece objects. The Board stores the gamestate and move history, acting as a primary hub to simulate gameplay. Each piece is created using the Factory pattern for encapsulation and customizability, with their respective type method returning a vector of Move structs that detail available board paths and interactions. The board then uses the provided Moves and their coordinate information to analyze game state. 
 
 For example, the board can detect captures where the end position of a move covers a preexisting piece or checks where the King piece is in the open path of another piece. Like Pieces, Players are also produced using the Factory method, and either provide an interface for the user to interact with or assign a pre-arranged behavior pattern to a computer player. These computer players filter through their available Move vectors and select moves that best satisfy outcomes such as prioritizing captures.

<img width="453" height="460" alt="Screenshot 2026-05-18 120937" src="https://github.com/user-attachments/assets/2cb5f5ad-079e-470a-841d-794ea161e5d5" />

The display is given in the form of an ASCII-art design or XWindow graphical display, the latter of which loads png files of chess pieces for a more appealing visual appearance. The displays also exploit the Observer design pattern so that calls to notifyObserver() by the Subject (Board) allow for automatic updates whenever the game state changes. 

 Game logic and flow is controlled by the CommandInterpreter, which provides a command-line interface for the user. The CI sets up the game, providing the necessary Players, Board, and Pieces. The game loop allows the player to select the difficulty level, colour, in-game actions (move, resign, offer draw).

<img width="723" height="318" alt="Screenshot 2026-05-18 120841" src="https://github.com/user-attachments/assets/6833546b-f24e-4872-b965-e31fc3347531" />


The combination of these program components exemplifies MVC architecture, with the Model (board) storing the chess game state, View (graphical display) providing visual updates, and Controller (CI) managing control flow between the Model and the View. 

## Notes on graphical display


This program uses PuTTy and XMing to display the board using X-Forwarding. 

<img width="200" height="160" alt="images" src="https://github.com/user-attachments/assets/c509f316-d9bf-4265-97c0-278a80c8e273" />

<img width="380" height="200" alt="1_f6Q7_hMy3dWOIHg1KcdRXQ" src="https://github.com/user-attachments/assets/8edcce07-7a22-4d82-9f95-a0be1a470898" />

The program is designed to run on the University of Waterloo's CS student Linux servers

## Credit

Credit to Prabhgun Bhatia and Armaan Gupta for help on this program
