module text_display;

import std;
import types;
import observer;
import board;
import pieces;

TextDisplay::TextDisplay(Board& board) : board{board} {}

void TextDisplay::update() {
    for (int y = 7; y >= 0; --y) {
        std::cout << (y + 1) << ' ';
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getPiece(Position{x, y});
            if (piece) {
                std::cout << piece->symbol();
            } else {
                std::cout << ((x + y) % 2 == 0 ? '_' : ' ');
            }
        }
        std::cout << '\n';
    }

    std::cout << "\n  abcdefgh\n";
}
