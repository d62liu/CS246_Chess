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
static constexpr int PST_PAWN[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  0,  0,  0,  0,  0,  0,  0,  0 },
};
static constexpr int PST_KNIGHT[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50 },
    {-40,-20,  0,  0,  0,  0,-20,-40 },
    {-30,  0, 10, 15, 15, 10,  0,-30 },
    {-30,  5, 15, 20, 20, 15,  5,-30 },
    {-30,  0, 15, 20, 20, 15,  0,-30 },
    {-30,  5, 10, 15, 15, 10,  5,-30 },
    {-40,-20,  0,  5,  5,  0,-20,-40 },
    {-50,-40,-30,-30,-30,-30,-40,-50 },
};
static constexpr int PST_BISHOP[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5, 10, 10,  5,  0,-10 },
    {-10,  5,  5, 10, 10,  5,  5,-10 },
    {-10,  0, 10, 10, 10, 10,  0,-10 },
    {-10, 10, 10, 10, 10, 10, 10,-10 },
    {-10,  5,  0,  0,  0,  0,  5,-10 },
    {-20,-10,-10,-10,-10,-10,-10,-20 },
};
static constexpr int PST_ROOK[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10, 10, 10, 10, 10,  5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    {  0,  0,  0,  5,  5,  0,  0,  0 },
};
static constexpr int PST_QUEEN[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5,  5,  5,  5,  0,-10 },
    { -5,  0,  5,  5,  5,  5,  0, -5 },
    {  0,  0,  5,  5,  5,  5,  0, -5 },
    {-10,  5,  5,  5,  5,  5,  0,-10 },
    {-10,  0,  5,  0,  0,  0,  0,-10 },
    {-20,-10,-10, -5, -5,-10,-10,-20 },
};
static constexpr int PST_KING_MG[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-20,-30,-30,-40,-40,-30,-30,-20 },
    {-10,-20,-20,-20,-20,-20,-20,-10 },
    { 20, 20,  0,  0,  0,  0, 20, 20 },
    { 20, 30, 10,  0,  0, 10, 30, 20 },
};

static int pstLookup(PieceType t, int file, int rank) {
    switch (t) {
        case PieceType::Pawn:   return PST_PAWN[rank][file];
        case PieceType::Knight: return PST_KNIGHT[rank][file];
        case PieceType::Bishop: return PST_BISHOP[rank][file];
        case PieceType::Rook:   return PST_ROOK[rank][file];
        case PieceType::Queen:  return PST_QUEEN[rank][file];
        case PieceType::King:   return PST_KING_MG[rank][file];
        default: return 0;
    }
}

static constexpr int PIECE_VAL_CP[] = { 100, 500, 320, 330, 900, 20000 }; // P R N B Q K

static int pieceValueCP(PieceType t) {
    return PIECE_VAL_CP[static_cast<int>(t)];
}

int Level5Strategy::evaluate(Board& board, Color color) {
    Color enemy = (color == White) ? Black : White;
    int score = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            ChessPiece* p = board.getPiece(Position{x, y});
            if (!p) continue;
            int rank = (p->getColor() == White) ? y : (7 - y);
            int bonus = pieceValueCP(p->getPieceType()) + pstLookup(p->getPieceType(), x, rank);
            score += (p->getColor() == color) ? bonus : -bonus;
        }
    }
    // Mobility bonus
    score += static_cast<int>(board.allLegalMoves(color).size()) * 5;
    score -= static_cast<int>(board.allLegalMoves(enemy).size()) * 5;
    return score;
}

// Order captures first, then checks, for better alpha-beta cutoffs
static std::vector<Move> orderedMoves(Board& board, Color color) {
    auto moves = board.allLegalMoves(color);
    std::stable_sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        int sa = 0, sb = 0;
        if (a.captured) sa += pieceValueCP(a.captured->getPieceType()) * 10
                             - pieceValueCP(a.piece_moved->getPieceType());
        if (b.captured) sb += pieceValueCP(b.captured->getPieceType()) * 10
                             - pieceValueCP(b.piece_moved->getPieceType());
        if (board.givesCheck(a.from, a.to)) sa += 50;
        if (board.givesCheck(b.from, b.to)) sb += 50;
        return sa > sb;
    });
    return moves;
}

// Quiescence search: only search captures to avoid horizon effect
int Level5Strategy::quiesce(Board& board, int alpha, int beta, Color maximizing, Color root) {
    int stand_pat = evaluate(board, root);
    if (maximizing == root) {
        if (stand_pat >= beta) return beta;
        if (stand_pat > alpha) alpha = stand_pat;
        for (auto& m : orderedMoves(board, maximizing)) {
            if (!m.captured) continue;
            board.movePiece(m.from, m.to);
            Color next = (maximizing == White) ? Black : White;
            int score = quiesce(board, alpha, beta, next, root);
            board.undoMove();
            if (score > alpha) alpha = score;
            if (alpha >= beta) return beta;
        }
        return alpha;
    } else {
        if (stand_pat <= alpha) return alpha;
        if (stand_pat < beta) beta = stand_pat;
        for (auto& m : orderedMoves(board, maximizing)) {
            if (!m.captured) continue;
            board.movePiece(m.from, m.to);
            Color next = (maximizing == White) ? Black : White;
            int score = quiesce(board, alpha, beta, next, root);
            board.undoMove();
            if (score < beta) beta = score;
            if (alpha >= beta) return alpha;
        }
        return beta;
    }
}

int Level5Strategy::alphaBeta(Board& board, int depth, int alpha, int beta, Color maximizing, Color root) {
    Color enemy = (maximizing == White) ? Black : White;
    if (board.isCheckmate(maximizing)) return (maximizing == root) ? -1000000 : 1000000;
    if (board.isStalemate(maximizing)) return 0;
    if (depth == 0) return quiesce(board, alpha, beta, maximizing, root);

    auto moves = orderedMoves(board, maximizing);
    if (moves.empty()) return evaluate(board, root);

    if (maximizing == root) {
        int best = std::numeric_limits<int>::min();
        for (auto& m : moves) {
            board.movePiece(m.from, m.to);
            int score = alphaBeta(board, depth - 1, alpha, beta, enemy, root);
            board.undoMove();
            best = std::max(best, score);
            alpha = std::max(alpha, best);
            if (alpha >= beta) break;
        }
        return best;
    } else {
        int best = std::numeric_limits<int>::max();
        for (auto& m : moves) {
            board.movePiece(m.from, m.to);
            int score = alphaBeta(board, depth - 1, alpha, beta, enemy, root);
            board.undoMove();
            best = std::min(best, score);
            beta = std::min(beta, best);
            if (alpha >= beta) break;
        }
        return best;
    }
}

Move Level5Strategy::pickMove(Board& board, Color color) {
    auto moves = orderedMoves(board, color);
    Color enemy = (color == White) ? Black : White;

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = moves[0];
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    for (auto& m : moves) {
        board.movePiece(m.from, m.to);
        int score = alphaBeta(board, MAX_DEPTH - 1, alpha, beta, enemy, color);
        board.undoMove();
        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
        alpha = std::max(alpha, bestScore);
    }
    return bestMove;
}

std::unique_ptr<Strategy> makeStrategy(PlayerType type) {
    switch (type) {
        case PlayerType::Computer2: return std::make_unique<Level2Strategy>();
        case PlayerType::Computer3: return std::make_unique<Level3Strategy>();
        case PlayerType::Computer4: return std::make_unique<Level4Strategy>();
        case PlayerType::Computer5: return std::make_unique<Level5Strategy>();
        default: return std::make_unique<Level1Strategy>();
    }
}
