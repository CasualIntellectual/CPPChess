export module display;

export import xwindow; // need xwindow module here so display importers get it
import board;
import <string>;

using namespace std;

export class TextDisplay : public Observer {
    Board *board;
    string toUnicode(char sym);
  public:
    TextDisplay(Board *b);
    void notify() override;
};

export class GraphicalDisplay : public Observer {
    static const int SQ = 75;       // square size px
    static const int BORDER = 30;
    static const int WINSZ = 8 * SQ + 2 * BORDER;
    static const int IMG = 70;      // piece png size (bit smaller than SQ)
    static const int IMG_PAD = (SQ - IMG) / 2;

    Board *board;
    Xwindow win;
    string imgKey(char sym, bool white, bool light);
  public:
    GraphicalDisplay(Board* b);
    void notify() override;
};

