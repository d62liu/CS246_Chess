#include "window.h"

#ifdef HAVE_X11

unsigned long XWindow::allocColor(int r, int g, int b) {
    XColor c;
    Colormap cmap = DefaultColormap(display, DefaultScreen(display));
    c.red = static_cast<unsigned short>(r * 256);
    c.green = static_cast<unsigned short>(g * 256);
    c.blue = static_cast<unsigned short>(b * 256);
    c.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, cmap, &c);
    return c.pixel;
}

XWindow::XWindow(int w, int h) : width{w}, height{h} {
    display = XOpenDisplay(nullptr);
    if (!display) throw std::runtime_error("Cannot open X display");

    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(
        display, RootWindow(display, screen),
        0, 0, width, height, 2,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );
    XStoreName(display, window, "Chess");
    XMapWindow(display, window);
    XSync(display, False);

    gc = XCreateGC(display, window, 0, nullptr);

    colors[White] = WhitePixel(display, screen);
    colors[Black] = BlackPixel(display, screen);
    colors[LightSquare] = allocColor(240, 217, 181);
    colors[DarkSquare] = allocColor(181, 136,  99);
    colors[HighlightSquare] = allocColor(130, 151, 105);
}

XWindow::~XWindow() {
    if (display) {
        XFreeGC(display, gc);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }
}

void XWindow::fillRectangle(int x, int y, int w, int h, Color c) {
    XSetForeground(display, gc, colors[c]);
    XFillRectangle(display, window, gc, x, y, w, h);
}

void XWindow::drawString(int x, int y, const std::string& s, Color c) {
    XSetForeground(display, gc, colors[c]);
    XDrawString(display, window, gc, x, y, s.c_str(), static_cast<int>(s.size()));
}

void XWindow::flush() { XFlush(display); }

#else

XWindow::XWindow(int, int) {
    throw std::runtime_error(
        "Graphical display unavailable: install XQuartz and rebuild with -DHAVE_X11");
}
XWindow::~XWindow() {}
void XWindow::fillRectangle(int, int, int, int, Color) {}
void XWindow::drawString(int, int, const std::string&, Color) {}
void XWindow::flush() {}

#endif
