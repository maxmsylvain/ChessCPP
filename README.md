# Chess Board Implementation in C++

This C++ program defines a basic chess engine supporting piece movement, board display, and check/checkmate detection.

## Components

### `Coordinates` Class
Encapsulates 2D coordinates with getter/setter methods for `x` and `y`.

### `Piece` Struct
Represents a chess piece:
- `type` (e.g., "Pawn", "Rook")
- `coordinate` (instance of `Coordinates`)
- `symbol` (char used for display)
- `isWhite` (true for white pieces)

### `Board` Class
Handles the game state:
- Initializes and sets up pieces on an 8×8 board
- Displays the board with standard chess notation
- Manages piece movement and captures
- Validates moves for all standard pieces
- Checks for check and checkmate conditions
- Handles pawn promotion

## Key Features
- Move validation per piece type
- Board updates after each move
- Ensures only the correct player can move
- Detects and prevents illegal moves including self-check
- Prints game-ending conditions (checkmate)

> Currently supports simplified chess rules without castling, en passant, or draw conditions.
