module board;

using namespace std;

Bishop::Bishop(Colour c) : Piece{c} {}
char Bishop::getSymbol() const { return colour == Colour::WHITE ? 'B' : 'b'; }


vector<Move> Bishop::calculatePaths(Board& b, Position pos) {
    int dr[] = {-1, -1, 1, 1};
    int dc[] = {-1,  1,-1, 1};
    return genSlidingMoves(b, pos, dr, dc, 4);
}

