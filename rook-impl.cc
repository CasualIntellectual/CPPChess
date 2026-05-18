module board;

using namespace std;

Rook::Rook(Colour c) : Piece{c} {}
char Rook::getSymbol() const { return colour == Colour::WHITE ? 'R' : 'r'; }

vector<Move> Rook::calculatePaths(Board& b, Position pos) {
    int dr[] = {-1, 1, 0, 0};
    int dc[] = { 0, 0,-1, 1};
    return genSlidingMoves(b, pos, dr, dc, 4);
}
