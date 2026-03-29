export module strategy;

import std;
import types;
import board;

export class Strategy {
public:
    virtual ~Strategy() = default;
    virtual Move pickMove(Board& board, Color color) = 0;
};

export class Level1Strategy : public Strategy {
    std::mt19937 rng{std::random_device{}()};
public:
    Move pickMove(Board& board, Color color) override;
};

export class Level2Strategy : public Strategy {
    std::mt19937 rng{std::random_device{}()};
public:
    Move pickMove(Board& board, Color color) override;
};

export class Level3Strategy : public Strategy {
    std::mt19937 rng{std::random_device{}()};
public:
    Move pickMove(Board& board, Color color) override;
};

export class Level4Strategy : public Strategy {
public:
    Move pickMove(Board& board, Color color) override;
};

export std::unique_ptr<Strategy> makeStrategy(PlayerType type);
