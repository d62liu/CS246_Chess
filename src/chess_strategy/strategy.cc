module strategy;

import std;
import types;
import board;
import pieces;


static int pieceValue(PieceType t) {
    switch (t) {
        case PieceType::Pawn: return 1;
        case PieceType::Knight: return 3;
        case PieceType::Bishop: return 3;
        case PieceType::Rook: return 5;
        case PieceType::Queen: return 9;
        case PieceType::King: return 0;
        default: return 0;
    }
}

static int materialScore(Board& board, Color color) {
    int score = 0;
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y) {
            ChessPiece* p = board.getPiece(Position{x, y});
            if (!p) continue;
            int v = pieceValue(p->getPieceType());
            score += (p->getColor() == color) ? v : -v;
        }
    return score;
}


Move Level1Strategy::pickMove(Board& board, Color color) {
    auto moves = board.allLegalMoves(color);
    std::uniform_int_distribution<std::size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

Move Level2Strategy::pickMove(Board& board, Color color) {
    auto moves = board.allLegalMoves(color);

    auto score = [&](const Move& m) {
        int s = 0;
        if (m.captured) s += pieceValue(m.captured->getPieceType()) * 10;
        if (board.givesCheck(m.from, m.to)) s += 5;
        return s;
    };

    int best = std::numeric_limits<int>::min();
    std::vector<Move> bestMoves;
    for (auto& m : moves) {
        int s = score(m);
        if (s > best) { best = s; bestMoves.clear(); }
        if (s == best) bestMoves.push_back(m);
    }
    std::uniform_int_distribution<std::size_t> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng)];
}

Move Level3Strategy::pickMove(Board& board, Color color) {
    auto moves = board.allLegalMoves(color);
    Color enemy = (color == White) ? Black : White;

    auto score = [&](const Move& m) {
        int s = 0;
        if (m.captured) s += pieceValue(m.captured->getPieceType()) * 10;
        if (board.givesCheck(m.from, m.to)) s += 5;
        if (board.isUnderAttack(m.to, enemy))
            s -= pieceValue(m.piece_moved->getPieceType()) * 8;
        return s;
    };

    int best = std::numeric_limits<int>::min();
    std::vector<Move> bestMoves;
    for (auto& m : moves) {
        int s = score(m);
        if (s > best) { best = s; bestMoves.clear(); }
        if (s == best) bestMoves.push_back(m);
    }
    std::uniform_int_distribution<std::size_t> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng)];
}

Move Level4Strategy::pickMove(Board& board, Color color) {
    auto moves = board.allLegalMoves(color);
    Color enemy = (color == White) ? Black : White;

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = moves[0];

    for (auto& m : moves) {
        board.movePiece(m.from, m.to);

        int score;
        if (board.isCheckmate(enemy)) {
            score = 100000;
        } else if (board.isStalemate(enemy)) {
            score = 0;
        } else {
            auto responses = board.allLegalMoves(enemy);
            if (responses.empty()) {
                score = materialScore(board, color);
            } else {
                int worstCase = std::numeric_limits<int>::max();
                for (auto& r : responses) {
                    board.movePiece(r.from, r.to);
                    int s = materialScore(board, color);
                    if (s < worstCase) worstCase = s;
                    board.undoMove();
                }
                score = worstCase;
            }
        }

        board.undoMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }

    return bestMove;
}


std::unique_ptr<Strategy> makeStrategy(PlayerType type) {
    switch (type) {
        case PlayerType::Computer2: return std::make_unique<Level2Strategy>();
        case PlayerType::Computer3: return std::make_unique<Level3Strategy>();
        case PlayerType::Computer4: return std::make_unique<Level4Strategy>();
        default: return std::make_unique<Level1Strategy>();
    }
}
