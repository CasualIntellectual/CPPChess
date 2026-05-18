module player;

using namespace std;

Level4Player::Level4Player(Colour c) : ComputerPlayer{c} {}

// piece values from chessprogramming Wiki Piece Value page (https://www.chessprogramming.org/Piece_Value)
static int valOf(char sym) {
    if (sym >= 'a' && sym <= 'z') sym -= 32;
    if (sym == 'P') return 100;
    else if (sym == 'N') return 320;
    else if (sym == 'B') return 330;
    else if (sym == 'R') return 500;
    else if (sym == 'Q') return 900;
    else if (sym == 'K') return 20000;
    return 0;
}

static int eval(Board& b, Colour us) {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for(int c=0;c<8;c++){
            Piece* p = b.getPieceAt({r, c});
            if (!p) continue;
            int v = valOf(p->getSymbol());
            if (p->getColour() == us) score += v;
            else score -= v;
        }
    }
    return score;
}

static vector<Move> getLegalMoves(Board& b, Colour side) {
    vector<Move> result;
    for(int r=0;r<8;r++){
        for (int c = 0; c < 8; c++) {
            Piece *p = b.getPieceAt({r, c});
            if (!p || p->getColour() != side) continue;
            auto mvs = p->getValidMoves(b, {r, c});
            result.insert(result.end(), mvs.begin(), mvs.end());
        }
    }
    return result;
}

// minimax search. chessprogramming.org/Minimax explains the recursion idea
static int minimax(Board& b, int depth, bool isMax,
        Colour us, int alpha, int beta) {
    Colour them = (us == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
    Colour toMove = isMax ? us : them;

    if (b.isCheckmate(toMove)) {
        return isMax ? -100000 : 100000;
    }
    if (b.isStalemate(toMove)) return 0;

    if (depth <= 0) {
        return eval(b, us);
    }

    auto moves = getLegalMoves(b, toMove);
    if (moves.empty()) {
        return isMax ? -100000 : 100000;
    }

    if (isMax) {
        int best = -999999;
        for (auto& m : moves) {
            b.tryMove(m);
            int v = minimax(b, depth - 1, false, us, alpha, beta);
            b.undoMove();
            if (v > best) best = v;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;  //alpha-beta prune (chessprogramming.org/Alpha-Beta)
        }
        return best;
    }
    else {
        int best = 999999;
        for (auto& m : moves) {
            b.tryMove(m);
            int v = minimax(b, depth - 1, true, us, alpha, beta);
            b.undoMove();
            if (v < best) best = v;
            if (best < beta) beta = best;
            if (beta <= alpha) break;  // same prune, min side
        }
        return best;
    }
}

Move Level4Player::selectMove(Board& b, vector<Move>& moves) {
    auto& rng = global_random_engine();

    int bestScore = -999999;
    vector<Move> bestMoves;

    for (auto& m : moves) {
        b.tryMove(m);
        int s = minimax(b, 2, false, colour, -999999, 999999);
        b.undoMove();

        if (s > bestScore) {
            bestScore = s;
            bestMoves.clear();
            bestMoves.push_back(m);
        }
        else if (s == bestScore) {
            bestMoves.push_back(m);
        }
    }

    return bestMoves[rng() % bestMoves.size()];
}
