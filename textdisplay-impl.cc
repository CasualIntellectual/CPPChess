module display;

import <iostream>;

using namespace std;

//chess unicode glyphs are in misc symbols block U+2600 range (unicode.org charts PDF)
string TextDisplay::toUnicode(char sym) {
    if (sym == 'K' || sym == 'k') {
        return "\u2654";
    } else if (sym == 'Q' || sym == 'q') {
        return "\u2655";
    } else if (sym == 'R' || sym == 'r') {
        return "\u2656";
    } else if (sym == 'B' || sym == 'b') {
        return "\u2657";
    } else if (sym == 'N' || sym == 'n') {
        return "\u2658";
    } else if (sym == 'P' || sym == 'p') {
        return "\u2659";
    } else {
        return "\u00B7";
    }
}

TextDisplay::TextDisplay(Board* b) : board{b} {}

void TextDisplay::notify() {
    cout << "   ";
    for (int c = 0; c < 8; ++c)
        cout << " " << (char)('a' + c) << "  ";
    cout << endl;

    cout << "  \u250C";
    for(int c=0;c<8;c++){
        cout << "\u2500\u2500\u2500";
        if (c < 7) cout << "\u252C";
    }
    cout << "\u2510" << endl;

    for (int r = 7; r >= 0; --r) {
        cout << (r + 1) << " \u2502";
        for (int c = 0; c < 8; c++) {
            Piece* p = board->getPieceAt({r, c});
            if (p) {
                bool black = p->getColour() == Colour::BLACK;
                // ECMA-48 CSI colour codes (\033[ ... m) for terminal text. see ecma-international.org/publications-and-standards/standards/ecma-48
                cout << (black ? "\033[91m" : "\033[97m");
                cout << " " << toUnicode(p->getSymbol()) << " ";
                cout << "\033[0m";
            } else {
                cout << " \u00B7 ";
            }
            cout << "\u2502";
        }
        cout << " " << (r + 1) << endl;

        if (r > 0) {
            cout << "  \u251C";
            for (int j = 0; j < 8; ++j) {
                cout << "\u2500\u2500\u2500";
                if (j < 7) cout << "\u253C";
            }
            cout << "\u2524" << endl;
        }
    }

    cout << "  \u2514";
    for (int c = 0; c < 8; c++) {
        cout << "\u2500\u2500\u2500";
        if (c < 7) cout << "\u2534";
    }
    cout << "\u2518" << endl;

    cout << "   ";
    for (int c = 0; c < 8; ++c)
        cout << " " << (char)('a' + c) << "  ";
    cout << endl << endl;
}
