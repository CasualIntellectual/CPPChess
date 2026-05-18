module board;

import <vector>;
import <memory>;


Board::Board() {
    grid.resize(8);
    for (int i = 0; i < 8; ++i) {
        grid[i].resize(8);
    }
}

Board::~Board() {}


void Board::init() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            grid[r][c] = nullptr;
        }
    }
    undoStack.clear();
    posHistory.clear();
    lastMove = Move{};

    char order[] = {'R','N','B','Q','K','B','N','R'};
    for (int c=0;c<8;c++) {
        grid[0][c] = PieceFactory::createPiece(order[c], Colour::WHITE);
        grid[1][c] = PieceFactory::createPiece('P', Colour::WHITE);
        grid[6][c] = PieceFactory::createPiece('P', Colour::BLACK);
        grid[7][c] = PieceFactory::createPiece(order[c], Colour::BLACK);
    }

    notifyObservers();
}

Piece* Board::getPieceAt(Position pos) const {
    if(pos.row < 0 || pos.row > 7 || pos.col < 0 || pos.col > 7)
        return nullptr;
    return grid[pos.row][pos.col].get();
}

Move Board::getLastMove() const { return lastMove; }

void Board::notifyObservers() { Subject::notifyObservers();}

void Board::setupAdd(char type, Position pos) {
    Colour col = (type >= 'A' && type <= 'Z') ? Colour::WHITE : Colour::BLACK;
    grid[pos.row][pos.col] = PieceFactory::createPiece(type, col);
}

void Board::setupRemove(Position pos) {
    grid[pos.row][pos.col] = nullptr;
}

bool Board::verifySetup() const {
    int wk = 0, bk = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece *p = grid[r][c].get();
            if (!p) continue;
            char s = p->getSymbol();
            if (s == 'K') ++wk;
            if (s == 'k') ++bk;
            if ((s == 'P' || s == 'p') && (r == 0 || r == 7))
                return false;
        }
    }
    if (wk != 1 || bk != 1) return false;

    // isCheck isnt const but we need it here + const_cast is ugly but works
    Board &self = const_cast<Board&>(*this);
    if (self.isCheck(Colour::WHITE) || self.isCheck(Colour::BLACK))
        return false;
    return true;
}

bool Board::isSquareAttacked(Position pos, Colour byColour) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece* p = grid[r][c].get();
            if (!p || p->getColour() != byColour) continue;

            char sym = p->getSymbol();

            if (sym == 'K' || sym == 'k') {
                int dr = pos.row - r, dc = pos.col - c;
                if (dr >= -1 && dr <= 1 && dc >= -1 && dc <= 1
                        && (dr != 0 || dc != 0))
                    return true;
                continue;
            }

            if (sym == 'P' || sym == 'p') {
                int fwd = (byColour == Colour::WHITE) ? 1 : -1;
                if (pos.row == r + fwd
                        && (pos.col == c - 1 || pos.col == c + 1))
                    return true;
                continue;
            }

            auto mvs = p->calculatePaths(*this, {r, c});
            for (auto &mv : mvs) {
                if (mv.end == pos) return true;
            }
        }
    }
    return false;
}

bool Board::isCheck(Colour c) {
    Position kingPos{-1, -1};
    for (int r = 0; r < 8; r++) {
        for (int j = 0; j < 8; j++) {
            Piece *p = grid[r][j].get();
            if (p && (p->getSymbol() == 'K' || p->getSymbol() == 'k')
                    && p->getColour() == c) {
                kingPos = {r, j};
                break;
            }
        }
        if (kingPos.row != -1) break;
    }
    if (kingPos.row == -1) return false;

    Colour enemy = (c == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
    return isSquareAttacked(kingPos, enemy);
}

bool Board::isCheckmate(Colour c) {
    if (!isCheck(c)) return false;

    for (int r = 0; r < 8; r++) {
        for (int j = 0; j < 8; j++) {
            Piece *p = grid[r][j].get();
            if (!p || p->getColour() != c) continue;
            if (!p->getValidMoves(*this, {r, j}).empty())
                return false;
        }
    }
    return true;
}

bool Board::isStalemate(Colour c) {
    if (isCheck(c)) return false;

    for (int r = 0; r < 8; r++) {
        for (int j = 0; j < 8; j++) {
            Piece *p = grid[r][j].get();
            if (!p || p->getColour() != c) continue;
            if (!p->getValidMoves(*this, {r, j}).empty())
                return false;
        }
    }
    return true;
}

void Board::movePiece(Move m) {
    Piece *pc = grid[m.start.row][m.start.col].get();
    Colour pcCol = pc->getColour();

    if (m.isCastle) {
        grid[m.end.row][m.end.col] =
            std::move(grid[m.start.row][m.start.col]);
        grid[m.end.row][m.end.col]->setMoved(true);

        // g file = col 6: rook from corner (7 or 0) slides to 5 or 3
        int rFrom = (m.end.col == 6) ? 7 : 0;
        int rTo   = (m.end.col == 6) ? 5 : 3;
        grid[m.end.row][rTo] = std::move(grid[m.end.row][rFrom]);
        grid[m.end.row][rTo]->setMoved(true);

    } else if (m.isEnPassant) {
        grid[m.end.row][m.end.col] =
            std::move(grid[m.start.row][m.start.col]);
        grid[m.end.row][m.end.col]->setMoved(true);
        //pawn we capture sat on start row, same file as end, not on end square
        grid[m.start.row][m.end.col] = nullptr;

    } else if (m.promotionType != '\0') {
        grid[m.start.row][m.start.col] = nullptr;
        grid[m.end.row][m.end.col] =
            PieceFactory::createPiece(m.promotionType, pcCol);
        grid[m.end.row][m.end.col]->setMoved(true);

    } else {
        grid[m.end.row][m.end.col] =
            std::move(grid[m.start.row][m.start.col]);
        grid[m.end.row][m.end.col]->setMoved(true);
    }

    lastMove = m;
    notifyObservers();
}

void Board::tryMove(Move m) {
    UndoInfo info;
    info.move = m;
    info.prevLastMove = lastMove;
    info.wasCastle = m.isCastle;
    info.wasEP = m.isEnPassant;
    info.wasPromo = (m.promotionType != '\0');

    Piece *pc = grid[m.start.row][m.start.col].get();
    info.pieceHadMoved = pc->getHasMoved();

    if (m.isCastle) {
        int rCol = (m.end.col == 6) ? 7 : 0;
        int rNew = (m.end.col == 6) ? 5 : 3;
        info.rookFrom = {m.end.row, rCol};
        info.rookTo = {m.end.row, rNew};
        info.rookHadMoved = grid[m.end.row][rCol]->getHasMoved();

        grid[m.end.row][m.end.col] =
            std::move(grid[m.start.row][m.start.col]);
        grid[m.end.row][m.end.col]->setMoved(true);
        grid[m.end.row][rNew] = std::move(grid[m.end.row][rCol]);
        grid[m.end.row][rNew]->setMoved(true);

    } else if (m.isEnPassant) {
        info.epPos = {m.start.row, m.end.col};
        info.epCaptured = std::move(grid[m.start.row][m.end.col]);

        grid[m.end.row][m.end.col] =
            std::move(grid[m.start.row][m.start.col]);
        grid[m.end.row][m.end.col]->setMoved(true);

    } else if (m.promotionType != '\0') {
        Colour pcCol = pc->getColour();
        info.origPiece = std::move(grid[m.start.row][m.start.col]);
        info.captured = std::move(grid[m.end.row][m.end.col]);

        grid[m.end.row][m.end.col] =
            PieceFactory::createPiece(m.promotionType, pcCol);
        grid[m.end.row][m.end.col]->setMoved(true);

    } else {
        info.captured = std::move(grid[m.end.row][m.end.col]);
        grid[m.end.row][m.end.col] =
            std::move(grid[m.start.row][m.start.col]);
        grid[m.end.row][m.end.col]->setMoved(true);
    }

    lastMove = m;
    undoStack.push_back(std::move(info));
}

void Board::undoMove() {
    if (undoStack.empty()) return;

    UndoInfo info = std::move(undoStack.back());
    undoStack.pop_back();

    Move m = info.move;
    lastMove = info.prevLastMove;

    if (info.wasCastle) {
        grid[m.start.row][m.start.col] =
            std::move(grid[m.end.row][m.end.col]);
        grid[m.start.row][m.start.col]->setMoved(info.pieceHadMoved);

        grid[info.rookFrom.row][info.rookFrom.col] =
            std::move(grid[info.rookTo.row][info.rookTo.col]);
        grid[info.rookFrom.row][info.rookFrom.col]
            ->setMoved(info.rookHadMoved);

    } else if (info.wasEP) {
        grid[m.start.row][m.start.col] =
            std::move(grid[m.end.row][m.end.col]);
        grid[m.start.row][m.start.col]->setMoved(info.pieceHadMoved);
        grid[info.epPos.row][info.epPos.col] = std::move(info.epCaptured);

    } else if (info.wasPromo) {
        grid[m.end.row][m.end.col] = std::move(info.captured);
        grid[m.start.row][m.start.col] = std::move(info.origPiece);
        grid[m.start.row][m.start.col]->setMoved(info.pieceHadMoved);

    } else {
        grid[m.start.row][m.start.col] =
            std::move(grid[m.end.row][m.end.col]);
        grid[m.start.row][m.start.col]->setMoved(info.pieceHadMoved);
        grid[m.end.row][m.end.col] = std::move(info.captured);
    }
}

// std::string here bc extra imports here broke gcc for us
std::string Board::getBoardString(Colour turn) {
    std::string s = "";
    for(int r=0;r<8;r++){
        for(int c=0;c<8;c++){
            Piece* p = grid[r][c].get();
            if(p) s += p->getSymbol();
            else s += '.';
        }
    }
    if(turn == Colour::WHITE) s += 'W';
    else s += 'B';
    return s;
}

void Board::recordPosition(Colour turn) {
    std::string state = getBoardString(turn);
    posHistory.push_back(state);
}

bool Board::isRepetitionDraw() {
    if(posHistory.size() < 3) return false;
    std::string current = posHistory.back();
    int count = 0;
    for(int i = 0; i < (int)posHistory.size(); i++){
        if(posHistory[i] == current) count++;
    }
    if(count >= 3) return true;
    return false;
}

void Board::clearHistory() {
    posHistory.clear();
}
