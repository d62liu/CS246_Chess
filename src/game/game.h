export module game;

import std;
import types;
import board;
import text_display;


export class Game {
public:
    Game();

    void startGame(PlayerType white, PlayerType black, bool resetBoard = true);
    bool makeMove(Position from, Position to, PieceType promotion = PieceType::Queen);
    void resign(Color color);
    void undoMove();

    bool isOver() const;
    Color currentTurn() const;
    PlayerType currentPlayerType() const;
    void setTurn(Color c);
    Board& getBoard();
    void printScore() const;

private:
    Board board;
    TextDisplay display;  
    PlayerType whitePlayer;
    PlayerType blackPlayer;
    Color turn;
    double whiteScore;
    double blackScore;
    bool gameInProgress;
};
