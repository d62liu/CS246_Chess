export module text_display;

import std;
import observer;
import board;

export class TextDisplay : public Observer {
    Board& board;
public:
    TextDisplay(Board& board);
    void update() override;
};
