module board;

using namespace std;

Knight::Knight(Colour c) : Piece{c} {}
char Knight::getSymbol() const { return colour == Colour::WHITE ? 'N' : 'n'; }

vector<Move> Knight::calculatePaths(Board& b, Position pos) {
    vector<Move> out;
    int dr[] = {-2,-2,-1,-1, 1, 1, 2, 2};
    int dc[] = {-1, 1,-2, 2,-2, 2,-1, 1};
    for (int i = 0; i < 8; i++) {
        int r = pos.row + dr[i], c = pos.col + dc[i];
        if (!onBoard(r, c)) continue;
        Piece *occ = b.getPieceAt({r, c});
        if (occ == nullptr || occ->getColour() != colour)
            out.push_back(Move{pos, {r, c}});
    }

    return out;
}

