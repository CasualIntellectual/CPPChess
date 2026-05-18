export module player;

import board;
import <vector>;
import <memory>;
import <string>;
import <random>;

using namespace std;

// shared rng - seed in main so levels dont all pick same "random" every run
export void init_random_seed(unsigned seed);
export default_random_engine& global_random_engine();

export class Player {
  protected:
    Colour colour;
  public:
    Player(Colour c);
    virtual ~Player() = default;
    virtual Move makeMove(Board& b) = 0;
    Colour getColour() const;
};

export class HumanPlayer : public Player {
  public:
    HumanPlayer(Colour c);
    Move makeMove(Board& b) override;
};

export class ComputerPlayer : public Player {
  protected:
    virtual Move selectMove(Board& b, vector<Move>& legalMoves) = 0;
  public:
    ComputerPlayer(Colour c);
    Move makeMove(Board& b) override;
};

export class Level1Player : public ComputerPlayer {
  protected:
    Move selectMove(Board& b, vector<Move>& legalMoves) override;
  public:
    Level1Player(Colour c);
};


export class Level2Player : public ComputerPlayer {
  protected:
    Move selectMove(Board& b, vector<Move>& legalMoves) override;
  public:
    Level2Player(Colour c);
};

export class Level3Player : public ComputerPlayer {
  protected:
    Move selectMove(Board& b, vector<Move>& legalMoves) override;
  public:
    Level3Player(Colour c);
};

export class Level4Player : public ComputerPlayer {
  protected:
    Move selectMove(Board& b, vector<Move>& legalMoves) override;
  public:
    Level4Player(Colour c);
};

export class PlayerFactory {
  public:
    static unique_ptr<Player> createPlayer(const string& type, Colour c);
};
