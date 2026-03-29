#pragma once
#include <string>
#include <stdexcept>

#ifdef HAVE_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

class XWindow {
#ifdef HAVE_X11
    Display* display;
    Window   window;
    GC       gc;
    unsigned long allocColor(int r, int g, int b);
#endif
    int width, height;
public:
    enum Color {
        White = 0, Black = 1,
        LightSquare = 2, DarkSquare = 3,
        HighlightSquare = 4
    };
    static const int NUM_COLORS = 5;
    unsigned long colors[NUM_COLORS] = {};

    XWindow(int width = 680, int height = 680);
    ~XWindow();
    void fillRectangle(int x, int y, int w, int h, Color c);
    void drawString(int x, int y, const std::string& s, Color c);
    void flush();
};
