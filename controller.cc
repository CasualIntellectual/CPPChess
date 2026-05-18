export module controller;

import game;
import board;
import <string>;

using namespace std;

export class CommandInterpreter {
    Game* game;
    bool inGame = false;
    float whiteScore=0;
    float blackScore = 0;


    Position parsePos(const string& s);
    void setupMode();
  public:
    CommandInterpreter(Game *g);
    void run();
};
