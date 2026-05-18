module board;

using namespace std;

Pawn::Pawn(Colour c) : Piece{c} {}
char Pawn::getSymbol() const { return colour == Colour::WHITE ? 'P' : 'p'; }

vector<Move> Pawn::calculatePaths(Board& b, Position pos) {
    vector<Move> out;
    int fwd = (colour == Colour::WHITE) ? 1 : -1;
    int home = (colour == Colour::WHITE) ? 1 : 6;
    int promRow = (colour == Colour::WHITE) ? 7 : 0;

    auto addMv = [&](Position from, Position to, bool ep) {
        if (to.row == promRow) {
            for (char ch : {'Q','R','B','N'}) {
                Move m; m.start = from; m.end = to;
                m.promotionType = ch;
                out.push_back(m);
            }
        } else {
            Move m; m.start = from; m.end = to;
            m.isEnPassant = ep;
            out.push_back(m);
        }
    };

    // one square forward
    int nr = pos.row + fwd;
    if (onBoard(nr, pos.col) && b.getPieceAt({nr, pos.col}) == nullptr) {
        addMv(pos, {nr, pos.col}, false);
        //2 squares forward
        int nr2 = pos.row + 2*fwd;
        if (pos.row==home && b.getPieceAt({nr2, pos.col}) == nullptr)
            out.push_back(Move{pos, {nr2, pos.col}});
    }

    // diagonal captures + en passant
    for (int dc : {-1, 1}) {
        int nc = pos.col + dc;
        if (!onBoard(nr, nc)) continue;

        Piece *tgt = b.getPieceAt({nr, nc});
        if (tgt != nullptr && tgt->getColour() != colour) {
            addMv(pos, {nr, nc}, false);
            continue;   // square taken, cant ep here
        }
        if (tgt != nullptr) continue;
        Piece *beside = b.getPieceAt({pos.row, nc});
        if (beside == nullptr || beside->getColour() == colour) continue;
        char sym = beside->getSymbol();
        if (sym != 'P' && sym != 'p') continue;

        Move prev = b.getLastMove();
        int diff = prev.start.row - prev.end.row;
        //double pawn push last turn = can take en passant
        if (prev.end.row == pos.row && prev.end.col == nc
                && (diff == 2 || diff == -2)) {
            addMv(pos, {nr, nc}, true);
        }
    }
    return out;
}

