module display;

import <string>;

using namespace std;

string GraphicalDisplay::imgKey(char sym, bool white, bool light) {
    string k;
    k += (white ? 'w' : 'b');
    char pc = sym;
    if(pc>='A'&&pc<='Z')
        pc += 32;
    k += pc;
    k += (light ? 'l' : 'd'); // light vs dark square tint for png
    return k;
}

GraphicalDisplay::GraphicalDisplay(Board* b) : board{b}, win{WINSZ, WINSZ} {
    string names[] = {"wp","wr","wn","wb","wq","wk",
        "bp","br","bn","bb","bq","bk"};
    string files[] = {"white-pawn",   "white-rook",   "white-knight",
                      "white-bishop", "white-queen",  "white-king",
                      "black-pawn",   "black-rook",   "black-knight",
                      "black-bishop", "black-queen",  "black-king"};

    for(int i=0;i<12;++i){
        string path = "png/" + files[i] + ".png";
        win.loadPNG(names[i] + "l", path, IMG, IMG, 240, 217, 181);
        win.loadPNG(names[i] + "d", path, IMG, IMG, 181, 136, 99);
    }
}

void GraphicalDisplay::notify() {
    win.fillRectangle(0,0,WINSZ,WINSZ,Xwindow::Black);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int sx = BORDER + c*SQ;
            int sy = BORDER + (7 - r) * SQ;

            bool light = (r+c)%2==0;
            int sqCol = light ? Xwindow::Ivory : Xwindow::DarkBrown;
            win.fillRectangle(sx, sy, SQ, SQ, sqCol);

            Piece* p = board->getPieceAt({r, c});
            if (p != nullptr) {
                bool isW = (p->getColour() == Colour::WHITE);
                string key = imgKey(p->getSymbol(), isW, light);
                if (!win.drawPixmap(sx+IMG_PAD, sy+IMG_PAD, key)) {
                    int tc = isW ? Xwindow::Blue : Xwindow::Red;
                    win.drawString(sx + SQ/2 - 4, sy + SQ/2 + 6,
                        string(1, p->getSymbol()), tc);
                }
            }
        }
    }

    for (int c = 0; c < 8; c++)
        win.drawString(BORDER + c * SQ + SQ/2 - 4, WINSZ - 10,
                       string(1, 'a' + c), Xwindow::White);
    for (int r = 0; r < 8; ++r)
        win.drawString(10, BORDER + (7 - r) * SQ + SQ/2 + 6,
                       string(1, (char)('1' + r)), Xwindow::White);

    win.flush();
}
