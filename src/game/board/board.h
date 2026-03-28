export module board;
import pieces.cc;
export const int BOARD_SIZE = 8;

export class Board {
public:
    Board();
    ~Board();
    ChessPiece* get_piece_at(Position pos) const;
    void move_piece(Position from, Position to);
};
