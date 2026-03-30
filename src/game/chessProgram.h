export module chess_program;

import std;
import types;
import game;

export class ChessProgram {
public:
    void run();

private:
    Game game;

    PlayerType whiteType = PlayerType::Human;
    PlayerType blackType = PlayerType::Human;

    void handleMove(std::istringstream& iss);
    bool setupMode();
    bool validateSetup();

    static PlayerType parsePlayerType(const std::string& s);
    static PieceType  parsePromotion(char c);
};
