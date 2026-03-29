module chess_program;

import std;
import board;
import pieces;
import strategy;
import graphic_display;


PlayerType ChessProgram::parsePlayerType(const std::string& s) {
    if (s == "human")     return PlayerType::Human;
    if (s == "computer1") return PlayerType::Computer1;
    if (s == "computer2") return PlayerType::Computer2;
    if (s == "computer3") return PlayerType::Computer3;
    if (s == "computer4") return PlayerType::Computer4;
    return PlayerType::Human;
}

PieceType ChessProgram::parsePromotion(char c) {
    switch (std::toupper(c)) {
        case 'R': return PieceType::Rook;
        case 'B': return PieceType::Bishop;
        case 'N': return PieceType::Knight;
        default:  return PieceType::Queen;
    }
}


void ChessProgram::handleMove(std::istringstream& iss) {
    if (game.isOver()) return;

    PlayerType cur = game.currentPlayerType();

    if (cur == PlayerType::Human) {
        std::string fromStr, toStr;
        if (!(iss >> fromStr >> toStr)) return;
        PieceType promo = PieceType::Queen;
        std::string promoStr;
        if (iss >> promoStr && !promoStr.empty())
            promo = parsePromotion(promoStr[0]);
        game.makeMove(Position::fromNotation(fromStr),
                      Position::fromNotation(toStr), promo);
    } else {
        Board& b = game.getBoard();
        b.setNotify(false);
        auto strat = makeStrategy(cur);
        Move m = strat->pickMove(b, game.currentTurn());
        b.setNotify(true);
        game.makeMove(m.from, m.to);
    }
}


bool ChessProgram::validateSetup() {
    Board& b = game.getBoard();
    int whiteKings = 0, blackKings = 0;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            ChessPiece* p = b.getPiece(Position{x, y});
            if (!p) continue;
            if (p->getPieceType() == PieceType::King) {
                if (p->getColor() == White) ++whiteKings;
                else                        ++blackKings;
            }
            if (p->getPieceType() == PieceType::Pawn && (y == 0 || y == 7)) {
                std::cout << "Invalid setup: pawns cannot be on the first or last row.\n";
                return false;
            }
        }
    }

    if (whiteKings != 1 || blackKings != 1) {
        std::cout << "Invalid setup: board must have exactly one white king and one black king.\n";
        return false;
    }
    if (b.isInCheck(White)) {
        std::cout << "Invalid setup: white king is in check.\n";
        return false;
    }
    if (b.isInCheck(Black)) {
        std::cout << "Invalid setup: black king is in check.\n";
        return false;
    }
    return true;
}


bool ChessProgram::setupMode() {
    Board& b = game.getBoard();
    b.reset();
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y)
            b.removePiece(Position{x, y});

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss{line};
        std::string cmd;
        if (!(iss >> cmd)) continue;

        if (cmd == "+") {
            std::string pieceStr, squareStr;
            if (!(iss >> pieceStr >> squareStr)) continue;
            if (pieceStr.empty()) continue;
            b.placePiece(Position::fromNotation(squareStr), pieceStr[0]);

        } else if (cmd == "-") {
            std::string squareStr;
            if (!(iss >> squareStr)) continue;
            b.removePiece(Position::fromNotation(squareStr));

        } else if (cmd == "=") {
            std::string colorStr;
            if (!(iss >> colorStr)) continue;
            game.setTurn(colorStr == "black" ? Black : White);

        } else if (cmd == "done") {
            if (validateSetup()) return true;
        }
    }
    return false;
}


void ChessProgram::run() {
    std::unique_ptr<GraphicDisplay> gd;
    try {
        gd = std::make_unique<GraphicDisplay>(game.getBoard());
        game.getBoard().attach(gd.get());
    } catch (...) {}

    bool customSetup = false;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss{line};
        std::string cmd;
        if (!(iss >> cmd)) continue;

        if (cmd == "game") {
            std::string wStr, bStr;
            if (!(iss >> wStr >> bStr)) continue;
            whiteType = parsePlayerType(wStr);
            blackType = parsePlayerType(bStr);
            game.startGame(whiteType, blackType, !customSetup);
            customSetup = false;

        } else if (cmd == "resign") {
            if (!game.isOver())
                game.resign(game.currentTurn());

        } else if (cmd == "move") {
            handleMove(iss);

        } else if (cmd == "undo") {
            game.undoMove();

        } else if (cmd == "setup") {
            if (game.isOver()) {
                customSetup = setupMode();
            } else {
                std::cout << "Cannot enter setup mode while a game is in progress.\n";
            }
        }
    }

    game.printScore();
}
