module;
#include <X11/Xlib.h>
export module xwindow;

import <map>;
import <string>;

using namespace std;

export class Xwindow {
    Display* d;
    Window w;
    int s;
    GC gc;
    unsigned long colours[12];

    struct PixmapEntry {
        Pixmap pm;
        int width, height;
    };
    map<string, PixmapEntry> pixmaps_;  // key -> loaded png pixmap

    Pixmap backbuf_;  //offscreen buffer: we draw here then flush() copies to w
    int width_, height_;

  public:
    Xwindow(int width=500, int height=500);
    ~Xwindow();
    Xwindow(const Xwindow&) = delete;
    Xwindow& operator=(const Xwindow&) = delete;

    enum {White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Brown,
          Ivory=10, DarkBrown=11};  // last two = board square colours from display.cc

    void fillRectangle(int x, int y, int width, int height, int colour=Black);
    void fillCircle(int cx, int cy, int r, int colour=Black);
    void fillEllipse(int cx, int cy, int rx, int ry, int colour=Black);
    void drawString(int x, int y, string msg, int colour=Black);

    bool loadPNG(const string &key, const string &filepath,
                 int targetW, int targetH,
                 unsigned char bgR=255, unsigned char bgG=255, unsigned char bgB=255);
    bool drawPixmap(int x, int y, const string &key);

    bool nextEvent(int &x, int &y);
    void flush();
};
