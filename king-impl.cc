module board;

using namespace std;

King::King(Colour c) : Piece{c} {}
char King::getSymbol() const { return colour == Colour::WHITE ? 'K' : 'k'; }

vector<Move> King::calculatePaths(Board& b, Position pos) {
    vector<Move> out;

    //normal 1-square moves in all 8 directions
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = pos.row + dr;
            int nc = pos.col + dc;
            if (!onBoard(nr, nc)) continue;
            Piece *occ = b.getPieceAt({nr, nc});
            if (occ == nullptr || occ->getColour() != colour)
                out.push_back(Move{pos, {nr, nc}});
        }
    }

    // castling
    if (!hasMoved && !b.isCheck(colour)) {
        int row = (colour == Colour::WHITE) ? 0 : 7;
        Colour enemy = (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

        // kingside
        Piece *kr = b.getPieceAt({row, 7});
        if (kr != nullptr && !kr->getHasMoved()) {
            char s = kr->getSymbol();
            if (s == 'R' || s == 'r') {
                bool clear = b.getPieceAt({row, 5}) == nullptr
                          && b.getPieceAt({row, 6}) == nullptr;
                if (clear && !b.isSquareAttacked({row, 5}, enemy)
                          && !b.isSquareAttacked({row, 6}, enemy)) {
                    Move m;
                    m.start = pos; m.end = {row, 6};
                    m.isCastle = true;
                    out.push_back(m);
                }
            }
        }
        // queenside
        Piece *qr = b.getPieceAt({row, 0});
        if (qr != nullptr && !qr->getHasMoved()) {
            char s = qr->getSymbol();
            if (s == 'R' || s == 'r') {
                bool clear = b.getPieceAt({row, 1}) == nullptr
                          && b.getPieceAt({row, 2}) == nullptr
                          && b.getPieceAt({row, 3}) == nullptr;
                if (clear && !b.isSquareAttacked({row, 2}, enemy)
                          && !b.isSquareAttacked({row, 3}, enemy)) {
                    Move m;
                    m.start = pos; m.end = {row, 2};
                    m.isCastle = true;
                    out.push_back(m);
                }
            }
        }
    }
    return out;
}
