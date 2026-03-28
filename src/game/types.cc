module types;

import std;

Position::Position(int x, int y) : x{x}, y{y} {}
int Position::getX() const { return x; }
int Position::getY() const { return y; }
bool Position::operator==(const Position& other) const { return x == other.x && y == other.y; }

Position Position::fromNotation(const std::string& s) {
    return Position{s[0] - 'a', s[1] - '1'};
}

std::string Position::toNotation() const {
    return std::string{(char)('a' + x), (char)('1' + y)};
}
