module;
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>
#include <unistd.h>
module xwindow;

import <iostream>;
import <cstdlib>;
import <string>;
import <vector>;

using namespace std;

// xlib api usage i mostly checked against Tronche's Xlib notes (tronche.com/gui/x/xlib)

// how many 0 bits at bottom of mask. used when packing rgb into X pixel format
static int trailingZeros(unsigned long mask) {
    int n = 0;
    while (mask && !(mask & 1)) {
        mask >>= 1;
        ++n;
    }
    return n;
}

static int countBits(unsigned long mask) {
    int n = 0;
    while (mask) {
        n += mask & 1;
        mask >>= 1;
    }
    return n;
}

Xwindow::Xwindow(int width, int height) {
    d = XOpenDisplay(nullptr);
    if (!d) {
        cerr << "Cannot open display" << endl;
        exit(1);
    }
    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                            BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapRaised(d, w);

    width_ = width;
    height_ = height;
    // pixmap same size as window. all drawing goes here first (less flicker)
    backbuf_ = XCreatePixmap(d, w, width, height,
                              DefaultDepth(d, DefaultScreen(d)));
    gc = XCreateGC(d, backbuf_, 0, (XGCValues *)0);

    XFlush(d);
    XFlush(d); //sometimes need 2 flushes for window to show up right on mac/xquartz

    XColor xcolour;
    Colormap cmap;
    char color_vals[12][12] = {"white", "black", "red", "green", "blue", "cyan",
                               "yellow", "magenta", "orange", "brown",
                               "#F0D9B5", "#B58863"};
    cmap = DefaultColormap(d, DefaultScreen(d));
    for (int i = 0; i < 12; ++i) {
        XParseColor(d, cmap, color_vals[i], &xcolour);
        XAllocColor(d, cmap, &xcolour);
        colours[i] = xcolour.pixel;
    }
    XSetForeground(d, gc, colours[Black]);

    XSizeHints hints;
    hints.flags = (USPosition | PSize | PMinSize | PMaxSize);
    hints.height = hints.base_height = hints.min_height = hints.max_height = height;
    hints.width = hints.base_width = hints.min_width = hints.max_width = width;
    XSetNormalHints(d, w, &hints);  //user cant resize (min=max)

    usleep(1000);
}

Xwindow::~Xwindow() {
    for (auto it = pixmaps_.begin(); it != pixmaps_.end(); it++)
        XFreePixmap(d, it->second.pm);
    XFreePixmap(d, backbuf_);
    XFreeGC(d, gc);
    XCloseDisplay(d);
}

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XFillRectangle(d, backbuf_, gc, x, y, width, height);
    XSetForeground(d, gc, colours[Black]);  // reset so we dont leak wrong colour into next call
}

void Xwindow::fillCircle(int cx, int cy, int r, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XFillArc(d, backbuf_, gc, cx-r, cy-r, 2*r, 2*r, 0, 360*64); // angles are in 1/64ths of a degree
    XSetForeground(d, gc, colours[Black]);
}

void Xwindow::fillEllipse(int cx, int cy, int rx, int ry, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XFillArc(d, backbuf_, gc, cx-rx, cy-ry, 2*rx, 2*ry, 0, 360*64);
    XSetForeground(d, gc, colours[Black]);
}

void Xwindow::drawString(int x, int y, string msg, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XDrawString(d, backbuf_, gc, x, y, msg.c_str(), (int)msg.length());
    XSetForeground(d, gc, colours[Black]);
}

//followed libpng manual examples (libpng.org)
bool Xwindow::loadPNG(const string &key, const string &filepath,
                      int tW, int tH,
                      unsigned char bgR, unsigned char bgG, unsigned char bgB) {
    FILE *fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        cerr << "loadPNG: can't open " << filepath << endl;
        return false;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) { fclose(fp); return false; }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        return false;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    int srcW = png_get_image_width(png, info);
    int srcH = png_get_image_height(png, info);
    png_byte ctype = png_get_color_type(png, info);
    png_byte bdepth = png_get_bit_depth(png, info);

    // libpng boilerplate, normalize to 8bit RGBA
    if (bdepth == 16) png_set_strip_16(png);
    if (ctype == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (ctype == PNG_COLOR_TYPE_GRAY && bdepth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (ctype == PNG_COLOR_TYPE_RGB || ctype == PNG_COLOR_TYPE_GRAY
            || ctype == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (ctype == PNG_COLOR_TYPE_GRAY || ctype == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    png_read_update_info(png, info);

    vector<unsigned char> pixels(srcW * srcH * 4);
    vector<png_bytep> rows(srcH);
    for (int y = 0; y < srcH; y++)
        rows[y] = pixels.data() + y * srcW * 4;
    png_read_image(png, rows.data());
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);

    int depth = DefaultDepth(d, s);
    Visual *vis = DefaultVisual(d, s);
    XImage *ximg = XCreateImage(d, vis, depth, ZPixmap, 0, nullptr, tW, tH, 32, 0);
    if (!ximg) return false;
    vector<char> imgBuf(ximg->bytes_per_line * tH);
    ximg->data = imgBuf.data();

    int rShift = trailingZeros(vis->red_mask);
    int gShift = trailingZeros(vis->green_mask);
    int bShift = trailingZeros(vis->blue_mask);
    int rBits = countBits(vis->red_mask);
    int gBits = countBits(vis->green_mask);
    int bBits = countBits(vis->blue_mask);

    for (int ty = 0; ty < tH; ty++) {
        for (int tx = 0; tx < tW; tx++) {
            int sx = tx * srcW / tW;
            int sy = ty * srcH / tH;
            unsigned char *px = pixels.data() + (sy * srcW + sx) * 4;
            unsigned int r = px[0], g = px[1], b = px[2], a = px[3];

            // alpha blend onto bg colour (Porter-Duff "over" operator)
            r = (r * a + bgR * (255 - a)) / 255;
            g = (g * a + bgG * (255 - a)) / 255;
            b = (b * a + bgB * (255 - a)) / 255;

            unsigned long pixel =
                ((unsigned long)(r >> (8 - rBits)) << rShift) |
                ((unsigned long)(g >> (8 - gBits)) << gShift) |
                ((unsigned long)(b >> (8 - bBits)) << bShift);
            XPutPixel(ximg, tx, ty, pixel);
        }
    }

    Pixmap pm = XCreatePixmap(d, w, tW, tH, depth);
    XPutImage(d, pm, gc, ximg, 0, 0, 0, 0, tW, tH);
    ximg->data = nullptr;
    XDestroyImage(ximg);

    auto it = pixmaps_.find(key);
    if (it != pixmaps_.end()) XFreePixmap(d, it->second.pm);

    PixmapEntry ent;
    ent.pm = pm;
    ent.width = tW;
    ent.height = tH;
    pixmaps_[key] = ent;

    return true;
}

bool Xwindow::drawPixmap(int x, int y, const string &key) {
    auto it = pixmaps_.find(key);
    if (it == pixmaps_.end()) return false;
    XCopyArea(d, it->second.pm, backbuf_, gc, 0, 0,
              it->second.width, it->second.height, x, y);
    return true;
}

void Xwindow::flush() {
    // copy whole back buffer to the real window
    XCopyArea(d, backbuf_, w, gc, 0, 0, width_, height_, 0, 0);
    XFlush(d);
}

bool Xwindow::nextEvent(int &x, int &y) {
    XEvent e;
    while (true) {
        XNextEvent(d, &e);
        if (e.type == ButtonPress) {
            x = e.xbutton.x;
            y = e.xbutton.y;
            return true;
        }
        if (e.type == KeyPress) return false;
        if (e.type == Expose && e.xexpose.count == 0)
            flush();  // redraw when window uncovered
    }
}
