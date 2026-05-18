module player;

import <random>;
using namespace std;

// rng for comp levels, same idea as shuffle example (seed in main)
static default_random_engine g_rng{0};

void init_random_seed(unsigned seed) {
    g_rng.seed(seed);
}

default_random_engine &global_random_engine() {
    return g_rng;
}

Player::Player(Colour c) : colour{c} {}

Colour Player::getColour() const {
    return colour;
}

ComputerPlayer::ComputerPlayer(Colour c) : Player{c} {}

Move ComputerPlayer::makeMove(Board& b) {
    vector<Move> all;

    for(int r=0;r<8;r++){
        for (int c = 0; c < 8; c++) {
            Piece *p = b.getPieceAt({r, c});
            if (p == nullptr) continue;
            if(p->getColour()!=colour)continue;

            auto mvs = p->getValidMoves(b,{r,c});
            all.insert(all.end(), mvs.begin(), mvs.end());
        }
    }

    if (all.empty()){
        return Move{};
    }
    return selectMove(b, all);
}


