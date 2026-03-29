export module board;

import std;
import types;
import observer;
import pieces;

export struct Move {
    Move(Position from, Position to, ChessPiece* pieceMoved,
         std::shared_ptr<ChessPiece> captured, Position capturedAt, bool prevHadMoved)
        : from{from}, to{to}, piece_moved{pieceMoved},
          captured{std::move(captured)}, captured_at{capturedAt},
          prev_had_moved{prevHadMoved} {}

    Position from;
    Position to;
    ChessPiece* piece_moved;                  
    std::shared_ptr<ChessPiece> captured;     
    Position captured_at;                     
    bool prev_had_moved;

    bool is_en_passant = false;
    bool is_castling = false;

    ChessPiece* rook_moved = nullptr;         // non-owning: rook remains in grid
    Position rook_from = Position{0, 0};
    Position rook_to   = Position{0, 0};
    bool rook_prev_had_moved = false;

    std::shared_ptr<ChessPiece> promoted_from; // owns original pawn after promotion
};

export class Board : public Subject {
public:
    Board();

    ChessPiece* getPiece(Position pos) const;
    bool movePiece(Position from, Position to, PieceType promotion = PieceType::Queen);
    void undoMove();
    void placePiece(Position pos, char symbol);
    void removePiece(Position pos);
    void reset();
    void setNotify(bool enabled);

    std::vector<Move> allLegalMoves(Color color);
    bool isInCheck(Color color) const;
    bool isCheckmate(Color color);
    bool isStalemate(Color color);
    bool givesCheck(Position from, Position to);
    bool isUnderAttack(Position pos, Color byColor) const;
    const Move* getLastMove() const;

private:
    std::shared_ptr<ChessPiece> grid[8][8];
    std::vector<Move> history;
    bool notify_enabled = true;

    void initBoard();
    std::vector<Position> getCandidateMoves(ChessPiece* piece) const;
    std::vector<Move> getLegalMoves(ChessPiece* piece);
    bool isPathClear(Position from, Position to) const;
    bool wouldLeaveInCheck(Position from, Position to, Color color);
    Position findKing(Color color) const;
};
