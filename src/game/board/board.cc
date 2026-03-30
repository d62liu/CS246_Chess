module board;

import std;
import pieces;


Board::Board() {
    initBoard();
}

void Board::initBoard() {
    auto place = [&](PieceType type, Color color, int x, int y) {
        grid[x][y] = makePiece(type, color, Position{x, y});
    };

    place(PieceType::Rook, White, 0, 0);
    place(PieceType::Knight, White, 1, 0);
    place(PieceType::Bishop, White, 2, 0);
    place(PieceType::Queen, White, 3, 0);
    place(PieceType::King, White, 4, 0);
    place(PieceType::Bishop, White, 5, 0);
    place(PieceType::Knight, White, 6, 0);
    place(PieceType::Rook, White, 7, 0);
    for (int x = 0; x < 8; ++x) place(PieceType::Pawn, White, x, 1);

    place(PieceType::Rook, Black, 0, 7);
    place(PieceType::Knight, Black, 1, 7);
    place(PieceType::Bishop, Black, 2, 7);
    place(PieceType::Queen, Black, 3, 7);
    place(PieceType::King, Black, 4, 7);
    place(PieceType::Bishop, Black, 5, 7);
    place(PieceType::Knight, Black, 6, 7);
    place(PieceType::Rook, Black, 7, 7);
    for (int x = 0; x < 8; ++x) place(PieceType::Pawn, Black, x, 6);
}


ChessPiece* Board::getPiece(Position pos) const {
    return grid[pos.getX()][pos.getY()].get();
}

const Move* Board::getLastMove() const {
    return history.empty() ? nullptr : &history.back();
}

Position Board::findKing(Color color) const {
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y)
            if (grid[x][y] &&
                grid[x][y]->getPieceType() == PieceType::King &&
                grid[x][y]->getColor() == color)
                return Position{x, y};
    throw std::runtime_error{"King not found"};
}

bool Board::isPathClear(Position from, Position to) const {
    int dx = to.getX() - from.getX();
    int dy = to.getY() - from.getY();
    int stepX = (dx == 0) ? 0 : dx / std::abs(dx);
    int stepY = (dy == 0) ? 0 : dy / std::abs(dy);
    int x = from.getX() + stepX;
    int y = from.getY() + stepY;
    while (x != to.getX() || y != to.getY()) {
        if (grid[x][y]) return false;
        x += stepX;
        y += stepY;
    }
    return true;
}

bool Board::isUnderAttack(Position pos, Color byColor) const {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            ChessPiece* piece = grid[x][y].get();
            if (!piece || piece->getColor() != byColor) continue;
            for (auto& m : getCandidateMoves(piece))
                if (m == pos) return true;
        }
    }
    return false;
}

bool Board::isInCheck(Color color) const {
    Color enemy = (color == White) ? Black : White;
    return isUnderAttack(findKing(color), enemy);
}

std::vector<Position> Board::getCandidateMoves(ChessPiece* piece) const {
    auto raw = piece->get_valid_moves();
    std::vector<Position> valid;

    PieceType type = piece->getPieceType();
    Color color = piece->getColor();
    Position from = piece->getPosition();
    bool isSlider = (type == PieceType::Rook || type == PieceType::Bishop || type == PieceType::Queen);

    for (auto& to : raw) {
        if (to.getX() < 0 || to.getX() >= 8 || to.getY() < 0 || to.getY() >= 8) continue;

        ChessPiece* target = grid[to.getX()][to.getY()].get();

        if (target && target->getColor() == color) continue;

        if (isSlider && !isPathClear(from, to)) continue;

        if (type == PieceType::Pawn) {
            bool isDiag = (from.getX() != to.getX());
            if (isDiag) {
                bool isEP = false;
                if (!history.empty()) {
                    const Move& last = history.back();
                    if (last.piece_moved->getPieceType() == PieceType::Pawn &&
                        std::abs(last.to.getY() - last.from.getY()) == 2 &&
                        last.to.getX() == to.getX() &&
                        last.to.getY() == from.getY()) {
                        isEP = true;
                    }
                }
                if (!target && !isEP) continue;
            } else {
                if (target) continue;
                if (std::abs(to.getY() - from.getY()) == 2) {
                    int midY = (from.getY() + to.getY()) / 2;
                    if (grid[from.getX()][midY]) continue;
                }
            }
        }

        valid.push_back(to);
    }
    return valid;
}

bool Board::wouldLeaveInCheck(Position from, Position to, Color color) {
    ChessPiece* movedRaw = grid[from.getX()][from.getY()].get();
    Position capturedAt  = to;
    bool isEP = false;

    auto savedTo = std::move(grid[to.getX()][to.getY()]);
    std::shared_ptr<ChessPiece> savedEP;

    if (movedRaw->getPieceType() == PieceType::Pawn &&
        from.getX() != to.getX() && !savedTo) {
        isEP = true;
        capturedAt = Position{to.getX(), from.getY()};
        savedEP = std::move(grid[capturedAt.getX()][capturedAt.getY()]);
    }

    grid[to.getX()][to.getY()] = std::move(grid[from.getX()][from.getY()]);
    movedRaw->setPosition(to);

    bool inCheck = isInCheck(color);

    movedRaw->setPosition(from);
    grid[from.getX()][from.getY()] = std::move(grid[to.getX()][to.getY()]);
    grid[to.getX()][to.getY()]     = std::move(savedTo);
    if (isEP) grid[capturedAt.getX()][capturedAt.getY()] = std::move(savedEP);

    return inCheck;
}

std::vector<Move> Board::getLegalMoves(ChessPiece* piece) {
    std::vector<Move> legal;
    Color color = piece->getColor();
    Color enemy = (color == White) ? Black : White;
    Position from = piece->getPosition();

    for (auto& to : getCandidateMoves(piece)) {
        if (wouldLeaveInCheck(from, to, color)) continue;

        auto captured = grid[to.getX()][to.getY()]; 
        Position capturedAt = to;
        bool isEP = false;

        if (piece->getPieceType() == PieceType::Pawn &&
            from.getX() != to.getX() && !captured) {
            isEP = true;
            capturedAt = Position{to.getX(), from.getY()};
            captured = grid[capturedAt.getX()][capturedAt.getY()];
        }

        Move m{from, to, piece, captured, capturedAt, piece->hasMoved()};
        m.is_en_passant = isEP;
        legal.push_back(m);
    }

    if (piece->getPieceType() == PieceType::King &&
        !piece->hasMoved() && !isInCheck(color)) {
        int y = (color == White) ? 0 : 7;

        ChessPiece* kRook = grid[7][y].get();
        if (kRook && kRook->getPieceType() == PieceType::Rook && !kRook->hasMoved() &&
            !grid[5][y] && !grid[6][y] &&
            !isUnderAttack(Position{5, y}, enemy) &&
            !isUnderAttack(Position{6, y}, enemy)) {
            Move m{from, Position{6, y}, piece, nullptr, Position{6, y}, piece->hasMoved()};
            m.is_castling = true;
            m.rook_moved = kRook;
            m.rook_from = Position{7, y};
            m.rook_to = Position{5, y};
            m.rook_prev_had_moved = kRook->hasMoved();
            legal.push_back(m);
        }

        ChessPiece* qRook = grid[0][y].get();
        if (qRook && qRook->getPieceType() == PieceType::Rook && !qRook->hasMoved() &&
            !grid[1][y] && !grid[2][y] && !grid[3][y] &&
            !isUnderAttack(Position{3, y}, enemy) &&
            !isUnderAttack(Position{2, y}, enemy)) {
            Move m{from, Position{2, y}, piece, nullptr, Position{2, y}, piece->hasMoved()};
            m.is_castling = true;
            m.rook_moved = qRook;
            m.rook_from = Position{0, y};
            m.rook_to = Position{3, y};
            m.rook_prev_had_moved = qRook->hasMoved();
            legal.push_back(m);
        }
    }

    return legal;
}

std::vector<Move> Board::allLegalMoves(Color color) {
    std::vector<Move> all;
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y) {
            ChessPiece* piece = grid[x][y].get();
            if (!piece || piece->getColor() != color) continue;
            auto moves = getLegalMoves(piece);
            all.insert(all.end(), moves.begin(), moves.end());
        }
    return all;
}

bool Board::isCheckmate(Color color) {
    return isInCheck(color) && allLegalMoves(color).empty();
}

bool Board::isStalemate(Color color) {
    return !isInCheck(color) && allLegalMoves(color).empty();
}

bool Board::movePiece(Position from, Position to, PieceType promotion) {
    ChessPiece* piece = grid[from.getX()][from.getY()].get();
    if (!piece) return false;

    auto legal = getLegalMoves(piece);
    Move* match = nullptr;
    for (auto& m : legal)
        if (m.to == to) { match = &m; break; }
    if (!match) return false;

    Move move = *match;

    if (move.captured)
        grid[move.captured_at.getX()][move.captured_at.getY()] = nullptr;

    grid[to.getX()][to.getY()]     = std::move(grid[from.getX()][from.getY()]);
    piece->setPosition(to);
    piece->setMoved(true);

    if (move.is_castling) {
        grid[move.rook_to.getX()][move.rook_to.getY()] =
            std::move(grid[move.rook_from.getX()][move.rook_from.getY()]);
        move.rook_moved->setPosition(move.rook_to);
        move.rook_moved->setMoved(true);
    }

    if (piece->getPieceType() == PieceType::Pawn) {
        int backRank = (piece->getColor() == White) ? 7 : 0;
        if (to.getY() == backRank) {
            auto promoted = makePiece(promotion, piece->getColor(), to);
            promoted->setMoved(true);
            move.promoted_from = std::move(grid[to.getX()][to.getY()]);
            grid[to.getX()][to.getY()] = std::move(promoted);
        }
    }

    history.push_back(std::move(move));
    if (notify_enabled) notifyObservers();
    return true;
}

void Board::setNotify(bool enabled) { notify_enabled = enabled; }

bool Board::givesCheck(Position from, Position to) {
    ChessPiece* movedRaw = grid[from.getX()][from.getY()].get();
    if (!movedRaw) return false;
    Color enemy = (movedRaw->getColor() == White) ? Black : White;

    Position capturedAt = to;
    bool isEP = false;

    auto savedTo = std::move(grid[to.getX()][to.getY()]);
    std::shared_ptr<ChessPiece> savedEP;

    if (movedRaw->getPieceType() == PieceType::Pawn &&
        from.getX() != to.getX() && !savedTo) {
        isEP = true;
        capturedAt = Position{to.getX(), from.getY()};
        savedEP = std::move(grid[capturedAt.getX()][capturedAt.getY()]);
    }

    grid[to.getX()][to.getY()] = std::move(grid[from.getX()][from.getY()]);
    movedRaw->setPosition(to);

    bool check = isInCheck(enemy);

    movedRaw->setPosition(from);
    grid[from.getX()][from.getY()] = std::move(grid[to.getX()][to.getY()]);
    grid[to.getX()][to.getY()]     = std::move(savedTo);
    if (isEP) grid[capturedAt.getX()][capturedAt.getY()] = std::move(savedEP);

    return check;
}

void Board::placePiece(Position pos, char symbol) {
    Color color = std::isupper(symbol) ? White : Black;
    PieceType type;
    switch (std::toupper(symbol)) {
        case 'K': type = PieceType::King;   break;
        case 'Q': type = PieceType::Queen;  break;
        case 'R': type = PieceType::Rook;   break;
        case 'B': type = PieceType::Bishop; break;
        case 'N': type = PieceType::Knight; break;
        case 'P': type = PieceType::Pawn;   break;
        default: return;
    }
    int x = pos.getX(), y = pos.getY();
    grid[x][y] = makePiece(type, color, pos);
    if (notify_enabled) notifyObservers();
}

void Board::removePiece(Position pos) {
    int x = pos.getX(), y = pos.getY();
    if (!grid[x][y]) return;
    grid[x][y] = nullptr;
    if (notify_enabled) notifyObservers();
}

void Board::reset() {
    history.clear();
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y)
            grid[x][y] = nullptr;
    initBoard();
}

void Board::undoMove() {
    if (history.empty()) return;
    Move move = std::move(history.back());
    history.pop_back();

    ChessPiece* piece;

    if (move.promoted_from) {
        grid[move.to.getX()][move.to.getY()] = nullptr;
        piece = move.promoted_from.get();
        grid[move.from.getX()][move.from.getY()] = std::move(move.promoted_from);
    } else {
        piece = grid[move.to.getX()][move.to.getY()].get();
        grid[move.from.getX()][move.from.getY()] = std::move(grid[move.to.getX()][move.to.getY()]);
    }

    piece->setPosition(move.from);
    piece->setMoved(move.prev_had_moved);

    if (move.captured)
        grid[move.captured_at.getX()][move.captured_at.getY()] = std::move(move.captured);

    if (move.is_castling) {
        grid[move.rook_from.getX()][move.rook_from.getY()] =
            std::move(grid[move.rook_to.getX()][move.rook_to.getY()]);
        move.rook_moved->setPosition(move.rook_from);
        move.rook_moved->setMoved(move.rook_prev_had_moved);
    }
}
