export module types;

import std;

export enum Color { White, Black };
export enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King };

export class Position {
public:
    Position(int x, int y);
    static Position fromNotation(const std::string& notation); // "e2" -> Position(4, 1)
    std::string toNotation() const;                            // Position(4, 1) -> "e2"
    bool operator==(const Position& other) const;
    int getX() const;
    int getY() const;
private:
    int x, y;
};
