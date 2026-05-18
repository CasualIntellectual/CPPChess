module board;

using namespace std;

Queen::Queen(Colour c) : Piece{c} {}
char Queen::getSymbol() const { return colour == Colour::WHITE ? 'Q' : 'q'; }

vector<Move> Queen::calculatePaths(Board& b, Position pos) {
    int dr[] = {-1,-1,-1,0,0,1,1,1};
    int dc[] = {-1,0,1,-1,1,-1,0,1};
    return genSlidingMoves(b, pos, dr, dc, 8);
}
