module types;

import std;

Position::Position(int x, int y) : x{x}, y{y} {}
int Position::getX() const { return x; }
int Position::getY() const { return y; }
bool Position::operator==(const Position& other) const { return x == other.x && y == other.y; }

Position Position::fromNotation(const std::string& s) {
    if (s.size() < 2) return Position{-1, -1};
    int x = s[0] - 'a';
    int y = s[1] - '1';
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return Position{-1, -1};
    return Position{x, y};
}

std::string Position::toNotation() const {
    return std::string{(char)('a' + x), (char)('1' + y)};
}
