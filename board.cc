export module board;

import <vector>;
import <memory>;
import <string>;

using namespace std;

export enum class Colour { WHITE, BLACK, NONE};

export struct Position {
    int row = 0;
    int col = 0;

    bool operator==(const Position& o) const {
        return row == o.row && col == o.col;
    }
    bool operator!=(const Position& o) const {
        return !(*this == o);
    }
};

export struct Move {
    Position start;
    Position end;
    char promotionType = '\0';
    bool isEnPassant = false;
    bool isCastle = false;
};

export class Observer {
public:
    virtual void notify() = 0;
    virtual ~Observer() = default;
};

export class Subject {
    vector<Observer*> observers;
public:
    virtual ~Subject() = default;
    void attach(Observer* o);
    void detach(Observer* o);
    virtual void notifyObservers();
};

export class Board; // fwd decl - gcc modules needs this before Piece uses Board&

export class Piece {
    friend class Board;
  protected:
    Colour colour;
    bool hasMoved = false;
    virtual vector<Move> calculatePaths(Board& b, Position pos) = 0;
    vector<Move> genSlidingMoves(Board& b, Position pos,
        const int dr[], const int dc[], int n);
    static bool onBoard(int r, int c);
  public:
    Piece(Colour c);
    virtual ~Piece() = default;
    Colour getColour() const;
    bool getHasMoved() const;
    void setMoved(bool val);
    virtual char getSymbol() const = 0;
    vector<Move> getValidMoves(Board& b, Position pos);
};

export class King : public Piece {
  protected:
    vector<Move> calculatePaths(Board& b, Position pos) override;
  public:
    King(Colour c);
    char getSymbol() const override;
};

export class Queen : public Piece {
  protected:
    vector<Move> calculatePaths(Board& b, Position pos) override;
  public:
    Queen(Colour c);
    char getSymbol() const override;
};

export class Bishop : public Piece {
  protected:
    vector<Move> calculatePaths(Board& b, Position pos) override;
  public:
    Bishop(Colour c);
    char getSymbol() const override;
};

export class Rook : public Piece {
  protected:
    vector<Move> calculatePaths(Board& b, Position pos) override;
  public:
    Rook(Colour c);
    char getSymbol() const override;
};

export class Knight : public Piece {
  protected:
    vector<Move> calculatePaths(Board& b, Position pos) override;
  public:
    Knight(Colour c);
    char getSymbol() const override;
};

export class Pawn : public Piece {
  protected:
    vector<Move> calculatePaths(Board& b, Position pos) override;
  public:
    Pawn(Colour c);
    char getSymbol() const override;
};

export class PieceFactory {
  public:
    static unique_ptr<Piece> createPiece(char type, Colour c);
};

struct UndoInfo { //inshallah this works
    Move move;
    Move prevLastMove;
    unique_ptr<Piece> captured;
    bool pieceHadMoved = false;
    bool wasCastle = false;
    Position rookFrom;
    Position rookTo;
    bool rookHadMoved = false;
    bool wasEP = false;
    Position epPos;
    unique_ptr<Piece> epCaptured;
    bool wasPromo = false;
    unique_ptr<Piece> origPiece;
};

export class Board : public Subject {
    vector<vector<unique_ptr<Piece>>> grid;
    Move lastMove;
    vector<UndoInfo> undoStack;
    vector<string> posHistory; //tracks positions for 3fold repetition
  public:
    Board();
    ~Board() override;

    void init();

    void movePiece(Move m);
    bool isCheck(Colour c);
    bool isCheckmate(Colour c);
    bool isStalemate(Colour c);

    void setupAdd(char type, Position pos);
    void setupRemove(Position pos);
    bool verifySetup() const;
    void notifyObservers() override;

    Piece* getPieceAt(Position pos) const;
    Move getLastMove() const;

    void tryMove(Move m);
    void undoMove();
    bool isSquareAttacked(Position pos, Colour byColour);

    string getBoardString(Colour turn);
    void recordPosition(Colour turn);
    bool isRepetitionDraw();
    void clearHistory();
};
