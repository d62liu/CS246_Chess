module;
#include "../../window/window.h"
module graphic_display;

import std;
import observer;
import board;
import types;
import pieces;

static const int PADDING = 40;
static const int SQUARE  = 75;

struct GraphicDisplay::Impl {
    XWindow window;
    Impl() : window{PADDING * 2 + SQUARE * 8, PADDING * 2 + SQUARE * 8} {}
};

GraphicDisplay::GraphicDisplay(Board& b) : board{b} {
    impl = std::make_unique<Impl>();
}

GraphicDisplay::~GraphicDisplay() = default;

void GraphicDisplay::update() {
    if (!impl) return;
    auto& win = impl->window;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int px = PADDING + x * SQUARE;
            int py = PADDING + (7 - y) * SQUARE;

            XWindow::Color sq = ((x + y) % 2 == 0)
                ? XWindow::DarkSquare
                : XWindow::LightSquare;
            win.fillRectangle(px, py, SQUARE, SQUARE, sq);

            ChessPiece* piece = board.getPiece(Position{x, y});
            if (piece) {
                XWindow::Color tc = (piece->getColor() == White)
                    ? XWindow::Black   
                    : XWindow::White;
                std::string sym(1, piece->symbol());
                win.drawString(px + SQUARE / 2 - 5, py + SQUARE / 2 + 8, sym, tc);
            }
        }
    }

    for (int y = 0; y < 8; ++y) {
        int py = PADDING + (7 - y) * SQUARE + SQUARE / 2 + 5;
        win.drawString(10, py, std::to_string(y + 1), XWindow::Black);
    }

    for (int x = 0; x < 8; ++x) {
        int px = PADDING + x * SQUARE + SQUARE / 2 - 4;
        win.drawString(px, PADDING + SQUARE * 8 + 25,
                       std::string(1, static_cast<char>('a' + x)), XWindow::Black);
    }

    win.flush();
}
