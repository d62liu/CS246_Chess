module pieces;

import std;

ChessPiece::ChessPiece(Color color, Position pos, PieceType piece_type)
    : color{color}, pos{pos}, piece_type{piece_type}, has_moved{false} {}

Color ChessPiece::getColor() const { return color; }
PieceType ChessPiece::getPieceType() const { return piece_type; }
Position ChessPiece::getPosition() const { return pos; }
bool ChessPiece::hasMoved() const { return has_moved; }
void ChessPiece::setMoved(bool moved) { has_moved = moved; }
void ChessPiece::setPosition(Position newPos) { pos = newPos; }

Pawn::Pawn(Color color, Position pos) : ChessPiece{color, pos, PieceType::Pawn} {}

std::vector<Position> Pawn::get_valid_moves() const {
    std::vector<Position> moves;
    int x = pos.getX(), y = pos.getY();
    int dir = (color == White) ? 1 : -1;

    // Forward one square (board will reject if blocked)
    moves.push_back(Position{x, y + dir});
    if (!has_moved) moves.push_back(Position{x, y + 2 * dir});
    moves.push_back(Position{x - 1, y + dir});
    moves.push_back(Position{x + 1, y + dir});

    return moves;
}

Rook::Rook(Color color, Position pos) : ChessPiece{color, pos, PieceType::Rook} {}

std::vector<Position> Rook::get_valid_moves() const {
    std::vector<Position> moves;
    int x = pos.getX(), y = pos.getY();
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    for (int d = 0; d < 4; ++d)
        for (int step = 1; step < 8; ++step)
            moves.push_back(Position{x + dx[d] * step, y + dy[d] * step});
    return moves;
}

Knight::Knight(Color color, Position pos) : ChessPiece{color, pos, PieceType::Knight} {}

std::vector<Position> Knight::get_valid_moves() const {
    std::vector<Position> moves;
    int x = pos.getX(), y = pos.getY();
    int offsets[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    for (auto& off : offsets)
        moves.push_back(Position{x + off[0], y + off[1]});
    return moves;
}

Bishop::Bishop(Color color, Position pos) : ChessPiece{color, pos, PieceType::Bishop} {}

std::vector<Position> Bishop::get_valid_moves() const {
    std::vector<Position> moves;
    int x = pos.getX(), y = pos.getY();
    int dx[] = {1, 1, -1, -1};
    int dy[] = {1, -1, 1, -1};
    for (int d = 0; d < 4; ++d)
        for (int step = 1; step < 8; ++step)
            moves.push_back(Position{x + dx[d] * step, y + dy[d] * step});
    return moves;
}

Queen::Queen(Color color, Position pos) : ChessPiece{color, pos, PieceType::Queen} {}

std::vector<Position> Queen::get_valid_moves() const {
    std::vector<Position> moves;
    int x = pos.getX(), y = pos.getY();
    int dx[] = {1, -1, 0, 0, 1, 1, -1, -1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};
    for (int d = 0; d < 8; ++d)
        for (int step = 1; step < 8; ++step)
            moves.push_back(Position{x + dx[d] * step, y + dy[d] * step});
    return moves;
}

King::King(Color color, Position pos) : ChessPiece{color, pos, PieceType::King} {}

std::vector<Position> King::get_valid_moves() const {
    std::vector<Position> moves;
    int x = pos.getX(), y = pos.getY();
    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy)
            if (dx != 0 || dy != 0)
                moves.push_back(Position{x + dx, y + dy});
    return moves;
}

char Pawn::symbol() const { return color == White ? 'P' : 'p'; }
char Rook::symbol() const { return color == White ? 'R' : 'r'; }
char Knight::symbol() const { return color == White ? 'N' : 'n'; }
char Bishop::symbol() const { return color == White ? 'B' : 'b'; }
char Queen::symbol() const { return color == White ? 'Q' : 'q'; }
char King::symbol() const { return color == White ? 'K' : 'k'; }

std::unique_ptr<ChessPiece> makePiece(PieceType type, Color color, Position pos) {
    switch (type) {
        case PieceType::Pawn: return std::make_unique<Pawn>(color, pos);
        case PieceType::Rook: return std::make_unique<Rook>(color, pos);
        case PieceType::Knight: return std::make_unique<Knight>(color, pos);
        case PieceType::Bishop: return std::make_unique<Bishop>(color, pos);
        case PieceType::Queen: return std::make_unique<Queen>(color, pos);
        case PieceType::King: return std::make_unique<King>(color, pos);
        default: __builtin_unreachable();
    }
}
