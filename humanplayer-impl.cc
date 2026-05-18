module player;

import <iostream>;
import <string>;


using namespace std;

HumanPlayer::HumanPlayer(Colour c) : Player{c} {}

Move HumanPlayer::makeMove(Board& b) {
    string src, dst;
    cin >> src >> dst;
    if (cin.fail()) return Move{};
    if (src.length() < 2 || dst.length() < 2) return Move{};

    Position from{src[1] - '1', src[0] - 'a'};
    Position to{dst[1] - '1', dst[0] - 'a'};

    char promo = '\0';
    Piece* pc = b.getPieceAt(from);
    if (pc != nullptr && pc->getColour() == colour) {
        char sym = pc->getSymbol();
        if ((sym == 'P' || sym == 'p') &&
                (to.row == 0 || to.row == 7)) {
            string pr;
            cin >> pr;
            if (!pr.empty()) {
                promo = pr[0];
                if (promo >= 'a' && promo <= 'z')
                    promo -= 32;
            }
        }
    }

    // need exact Move from getValidMoves so castle/ep flags are right
    if (pc != nullptr && pc->getColour() == colour) {
        auto legal = pc->getValidMoves(b, from);
        for (size_t i = 0; i < legal.size(); i++) {
            if (legal[i].end == to && legal[i].promotionType == promo)
                return legal[i];
        }
    }
    
    return Move{from, to, promo, false, false};
}
