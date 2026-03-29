module game;

import types;
import std;
import pieces;

Game::Game() : display{board}, whitePlayer{PlayerType::Human}, blackPlayer{PlayerType::Human},
               turn{White}, whiteScore{0}, blackScore{0}, gameInProgress{false} {
    board.attach(&display);
}

void Game::startGame(PlayerType white, PlayerType black, bool resetBoard) {
    if (resetBoard) board.reset(); 
    whitePlayer = white;
    blackPlayer = black;
    turn = White;
    gameInProgress = true;
    board.notifyObservers();
}

bool Game::makeMove(Position from, Position to, PieceType promotion) {
    if (!gameInProgress) return false;
    ChessPiece* piece = board.getPiece(from);
    if (!piece || piece->getColor() != turn) return false;
    if (!board.movePiece(from, to, promotion)) return false;

    turn = (turn == White) ? Black : White;

    if (board.isCheckmate(turn)) {
        Color winner = (turn == White) ? Black : White;
        std::cout << "Checkmate! " << (winner == White ? "White" : "Black") << " wins!\n";
        (winner == White ? whiteScore : blackScore) += 1;
        gameInProgress = false;
    } else if (board.isStalemate(turn)) {
        std::cout << "Stalemate!\n";
        whiteScore += 0.5;
        blackScore += 0.5;
        gameInProgress = false;
    } else if (board.isInCheck(turn)) {
        std::cout << (turn == White ? "White" : "Black") << " is in check.\n";
    }

    return true;
}

void Game::resign(Color color) {
    if (!gameInProgress) return;
    Color winner = (color == White) ? Black : White;
    std::cout << (winner == White ? "White" : "Black") << " wins!\n";
    (winner == White ? whiteScore : blackScore) += 1;
    gameInProgress = false;
}

void Game::undoMove() {
    if (!gameInProgress) return;
    board.undoMove();
    turn = (turn == White) ? Black : White;
    board.notifyObservers();
}

bool Game::isOver() const { return !gameInProgress; }
Color Game::currentTurn() const { return turn; }
PlayerType Game::currentPlayerType() const {
    return (turn == White) ? whitePlayer : blackPlayer;
}
void Game::setTurn(Color c) { turn = c; }
Board& Game::getBoard() { return board; }

void Game::printScore() const {
    std::cout << "Final Score:\n" << "White: " << whiteScore << "\n" << "Black: " << blackScore << "\n";
}
