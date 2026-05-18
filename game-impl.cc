module game;

import <string>;

using namespace std;

Game::Game() : board{std::make_unique<Board>()} {}

Game::~Game() {}

void Game::initGame(const string& whiteType, const string& blackType) {
    players.clear();

    if (!customSetup) {
        board->init();
        currentTurn = Colour::WHITE;
    }
    else {
        board->clearHistory();
    }
    customSetup = false;

    //record starting position for repetition tracking
    board->recordPosition(currentTurn);

    players.push_back(PlayerFactory::createPlayer(whiteType, Colour::WHITE));
    players.push_back(PlayerFactory::createPlayer(blackType, Colour::BLACK));

    gameActive = true;
}

bool Game::playTurn() {
    int idx = (currentTurn == Colour::WHITE) ? 0 : 1;
    Move m = players[idx]->makeMove(*board);

    Piece* pc = board->getPieceAt(m.start);
    if (!pc || pc->getColour() != currentTurn) return false;

    auto legal = pc->getValidMoves(*board, m.start);
    bool matched = false;
    //human move struct might not know castle/ep flags so we match full Move from legal list
    for (size_t i = 0; i < legal.size(); i++) {
        if (legal[i].end == m.end
                && legal[i].promotionType == m.promotionType
                && legal[i].isCastle == m.isCastle
                && legal[i].isEnPassant == m.isEnPassant) {
            matched = true;
            break;
        }
    }
    if (!matched) return false;

    board->movePiece(m);
    currentTurn = (currentTurn == Colour::WHITE)
        ? Colour::BLACK : Colour::WHITE;
    return true;
}

void Game::resign(Colour c) {
    gameActive = false;
}

bool Game::isGameOver() const { return !gameActive; }

bool Game::isGameActive() const { return gameActive; }

Board& Game::getBoard() { return *board; }

Colour Game::getCurrentTurn() const { return currentTurn; }

void Game::setTurn(Colour c) { currentTurn = c; }

void Game::setCustomSetup(bool val) { customSetup = val; }
