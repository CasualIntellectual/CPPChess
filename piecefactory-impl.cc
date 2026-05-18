module board;

using namespace std;

unique_ptr<Piece> PieceFactory::createPiece(char type, Colour c) {
    if (type == 'K' || type == 'k') {
        return make_unique<King>(c);
    }
    else if (type == 'Q' || type == 'q') {
        return make_unique<Queen>(c);
    } else if (type == 'B' || type == 'b') {
        return make_unique<Bishop>(c);
    } else if (type == 'R' || type == 'r') {
        return make_unique<Rook>(c);
    } else if (type == 'N' || type == 'n') {
        return make_unique<Knight>(c);
    } else if (type == 'P' || type == 'p') {
        return make_unique<Pawn>(c);
    }

    return nullptr;
}
