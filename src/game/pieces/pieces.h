export module pieces;

import std;
import types;

export class ChessPiece {
public:
    ChessPiece(Color color, Position pos, PieceType piece_type);
    Color getColor() const;
    PieceType getPieceType() const;
    Position getPosition() const;
    bool hasMoved() const;
    void setMoved(bool moved);
    void setPosition(Position newPos);
    virtual std::vector<Position> get_valid_moves() const = 0;
    virtual char symbol() const = 0;
    virtual ~ChessPiece() = default;
protected:
    Color color;
    Position pos;
    PieceType piece_type;
    bool has_moved;
};

export class Pawn : public ChessPiece {
public:
    Pawn(Color color, Position pos);
    std::vector<Position> get_valid_moves() const override;
    char symbol() const override;
};

export class Rook : public ChessPiece {
public:
    Rook(Color color, Position pos);
    std::vector<Position> get_valid_moves() const override;
    char symbol() const override;
};

export class Knight : public ChessPiece {
public:
    Knight(Color color, Position pos);
    std::vector<Position> get_valid_moves() const override;
    char symbol() const override;
};

export class Bishop : public ChessPiece {
public:
    Bishop(Color color, Position pos);
    std::vector<Position> get_valid_moves() const override;
    char symbol() const override;
};

export class Queen : public ChessPiece {
public:
    Queen(Color color, Position pos);
    std::vector<Position> get_valid_moves() const override;
    char symbol() const override;
};

export class King : public ChessPiece {
public:
    King(Color color, Position pos);
    std::vector<Position> get_valid_moves() const override;
    char symbol() const override;
};

export std::unique_ptr<ChessPiece> makePiece(PieceType type, Color color, Position pos);
