import std;
import types;
import pieces;
import board;

int main(int argc, char* argv[]) {
    Board board;
    Position from(0, 1);
    Position to(0, 3);
    if (board.movePiece(from, to)) {
        std::cout << "Move successful!" << std::endl;
    } else {
        std::cout << "Invalid move!" << std::endl;
    }
    return 0;
}
