module player;

using namespace std;

Level2Player::Level2Player(Colour c) : ComputerPlayer{c} {}

Move Level2Player::selectMove(Board& b, vector<Move>& moves) {
    auto& rng = global_random_engine();
    Colour enemy = (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;

    vector<Move> good; // captures or checks
    vector<Move> rest;

    for (auto& m : moves) {
        bool cap = (b.getPieceAt(m.end) != nullptr) || m.isEnPassant;
        b.tryMove(m);
        bool chk = b.isCheck(enemy);
        b.undoMove();

        if (cap || chk) good.push_back(m);
        else rest.push_back(m);
    }

    auto& pick = good.empty() ? rest : good;
    return pick[rng() % pick.size()];
}

