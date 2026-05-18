module board;

using namespace std;

Piece::Piece(Colour c) : colour{c} {}
Colour Piece::getColour() const { return colour; }
bool Piece::getHasMoved() const { return hasMoved; }
void Piece::setMoved(bool val) { hasMoved = val; }

bool Piece::onBoard(int r, int c) {
    return r>=0 && r<8 && c>=0 && c<8;
}

// NVI: get raw paths then toss anything that leaves king in check
vector<Move> Piece::getValidMoves(Board& b, Position pos) {
    auto raw = calculatePaths(b, pos);
    vector<Move> out;
    for (auto& m : raw) {
        b.tryMove(m);
        if (!b.isCheck(colour)) out.push_back(m);
        b.undoMove();
    }
    return out;
}


vector<Move> Piece::genSlidingMoves(Board& b, Position pos,
        const int dr[], const int dc[], int n) {
    vector<Move> mvs;
    for (int i = 0; i < n; ++i) {
        int r = pos.row + dr[i];
        int c = pos.col + dc[i];
        while (onBoard(r, c)) {
            Piece* p = b.getPieceAt({r, c});
            if (p == nullptr) {
                mvs.push_back(Move{pos, {r, c}});
            } else {
                if (p->getColour() != colour)
                    mvs.push_back(Move{pos, {r, c}});
                break;
            }
            r += dr[i];
            c += dc[i];
        }
    }
    return mvs;
}
