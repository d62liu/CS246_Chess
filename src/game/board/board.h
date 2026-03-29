export module board;

import std;
import types;
import pieces;


export class Move {
public:
    Move(Position from, Position to, ChessPiece* pieceMoved,
         ChessPiece* captured, Position capturedAt, bool prevHadMoved)
        : from{from}, to{to}, piece_moved{pieceMoved},
          captured{captured}, captured_at{capturedAt},
          prev_had_moved{prevHadMoved} {}

    Position from;
    Position to;
    ChessPiece* piece_moved;
    ChessPiece* captured;        // nullptr if no capture
    Position captured_at;        // differs from `to` for en passant
    bool prev_had_moved;

    bool is_en_passant = false;
    bool is_castling = false;

    // Castling only: rook's part of the move
    ChessPiece* rook_moved = nullptr;
    Position rook_from = Position{0, 0};
    Position rook_to   = Position{0, 0};
    bool rook_prev_had_moved = false;

    // Promotion only: the original pawn, kept alive for undo
    ChessPiece* promoted_from = nullptr;
};

export class Board {
public:
    Board();
    ~Board();

    ChessPiece* getPiece(Position pos) const;
    // Returns false if the move is illegal.
    bool movePiece(Position from, Position to, PieceType promotion = PieceType::Queen);
    void undoMove();

    std::vector<Move> allLegalMoves(Color color) const;
    bool isInCheck(Color color) const;
    bool isCheckmate(Color color) const;
    bool isStalemate(Color color) const;
    const Move* getLastMove() const;


private:
    mutable ChessPiece* grid[8][8];
    std::vector<Move> history;

    void initBoard();

    // Candidate squares a piece can reach, applying bounds/blocking/pawn rules.
    std::vector<Position> getCandidateMoves(ChessPiece* piece) const;
    // Candidates filtered to exclude moves that leave own king in check.
    std::vector<Move> getLegalMoves(ChessPiece* piece) const;

    bool isPathClear(Position from, Position to) const;
    bool isUnderAttack(Position pos, Color byColor) const;
    // Temporarily applies from->to on the grid and tests for check.
    bool wouldLeaveInCheck(Position from, Position to, Color color) const;
    Position findKing(Color color) const;
    ostream& printBoard(ostream& os) const;
};
