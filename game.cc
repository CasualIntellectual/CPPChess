export module game;

import board;
import player;
import <memory>;
import <vector>;
import <string>;

using namespace std;

export class Game {
    unique_ptr<Board> board;
    vector<unique_ptr<Player>> players;
    Colour currentTurn = Colour::WHITE;
    bool gameActive=false;
    bool customSetup=false;
  public:
    Game();
    ~Game();

    void initGame(const string& whiteType, const string& blackType);
    bool playTurn();
    void resign(Colour c);
    bool isGameOver() const;

    bool isGameActive() const;
    Board& getBoard();
    Colour getCurrentTurn() const;
    void setTurn(Colour c);
    void setCustomSetup(bool val);
};
