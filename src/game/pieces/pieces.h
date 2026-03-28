export module pieces;

import <memory>;
import <string>;
import <vector>;
import board;

export enum Color { White, Black };
export enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King };

export class Position {
public:
    Position(int x, int y);
    int getX() const;
    int getY() const;
private:
    int x;
    int y;
};

export class ChessPiece {
public:
    ChessPiece(Color color, Position pos, PieceType piece_type);
    Color getColor() const;
    PieceType getPieceType() const;
    Position getPosition() const;
    bool hasMoved() const;
    virtual std::vector<Position> get_valid_moves(Board& board) const = 0;
    virtual ~ChessPiece() = default;
protected:
    bool has_moved;
    Color color;
    Position pos;
    PieceType piece_type;
};

export class Pawn : public ChessPiece {
public:
    Pawn(Color color, Position pos);
    std::vector<Position> get_valid_moves(Board& board) const override;
};

export class Rook : public ChessPiece {
public:
    Rook(Color color, Position pos);
    std::vector<Position> get_valid_moves(Board& board) const override;
};

export class Knight : public ChessPiece {
public:
    Knight(Color color, Position pos);
    std::vector<Position> get_valid_moves(Board& board) const override;
};

export class Bishop : public ChessPiece {
public:
    Bishop(Color color, Position pos);
    std::vector<Position> get_valid_moves(Board& board) const override;
};

export class Queen : public ChessPiece {
public:
    Queen(Color color, Position pos);
    std::vector<Position> get_valid_moves(Board& board) const override;
};

export class King : public ChessPiece {
public:
    King(Color color, Position pos);
    std::vector<Position> get_valid_moves(Board& board) const override;
};

export std::unique_ptr<ChessPiece> makePiece(PieceType type, Color color, Position pos);
