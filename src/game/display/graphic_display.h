export module graphic_display;

import std;
import observer;
import board;

export class GraphicDisplay : public Observer {
    struct Impl;
    std::unique_ptr<Impl> impl;
    Board& board;
public:
    GraphicDisplay(Board& board);
    ~GraphicDisplay();
    void update() override;
};
