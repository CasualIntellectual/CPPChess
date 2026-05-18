module player;

using namespace std;

Level3Player::Level3Player(Colour c) : ComputerPlayer{c} {}

// give each move a numeric score. higher = better
static int rateMv(Board& b, Move& m, Colour us) {
    Colour them = (us == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
    int score = 0;
    bool cap = (b.getPieceAt(m.end) != nullptr) || m.isEnPassant;
    if (cap) score += 4;

    b.tryMove(m);
    if (b.isCheck(them)) score += 3;
    if (!b.isSquareAttacked(m.end, them)) score += 2;
    b.undoMove();

    return score;
}

Move Level3Player::selectMove(Board& b, vector<Move>& moves) {
    auto& rng = global_random_engine();

    int best = -1;
    for (auto& m : moves) {
        int s = rateMv(b, m, colour);
        if (s > best) best = s;
    }
    
    //collect all moves tied at the best score
    vector<Move> top;
    for (auto& m : moves) {
        if (rateMv(b, m, colour) == best) top.push_back(m);
    }
    return top[rng() % top.size()];
}
