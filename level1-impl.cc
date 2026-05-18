module player;

using namespace std;

Level1Player::Level1Player(Colour c) : ComputerPlayer{c} {}

Move Level1Player::selectMove(Board& b, vector<Move>& moves) {
    auto& rng = global_random_engine();
    int idx = rng() % moves.size();
    return moves[idx];
}
