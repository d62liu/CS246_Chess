# chess

A terminal chess game written in C++ with an optional graphical display. Supports human vs human, human vs computer, and computer vs computer.

## Features

- Multi-level AI engine: greedy heuristics (levels 1–3), 2-ply minimax (level 4), and 4-ply alpha-beta minimax with piece-square tables, and quiescence search (level 5)
- Graphical display via X11 window
- Custom board setup mode
- Full chess rules (castling, en passant, promotion)

## Build & Run

```bash
make
./chess
```

## Usage

```
game white human black computer4   # start a game
move e2 e4                         # make a move
resign                             # forfeit
```
