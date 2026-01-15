# Console Chess Game

A fully-featured, cross-platform chess game implementation in C++ with a beautiful Unicode-based console interface. This project demonstrates advanced chess rules including castling, en passant, pawn promotion, and comprehensive draw conditions.

![Chess Board](chess_board.png)

---

## Overview

This console-based chess game provides a complete chess experience with all standard rules implemented. The game features a visually appealing board using Unicode box-drawing characters and chess piece symbols, making it easy to play directly in your terminal.

### Key Highlights

- ✓ Full implementation of chess rules and piece movements
- ✓ Special moves: Castling, En Passant, Pawn Promotion
- ✓ Check, Checkmate, and Stalemate detection
- ✓ Draw conditions: Threefold Repetition, 50-Move Rule, Insufficient Material, Agreement
- ✓ Cross-platform support (Windows, Linux, macOS)
- ✓ Clean, Unicode-based visual board representation
- ✓ Move validation to prevent illegal moves

---

## Features

### Core Gameplay

- **Complete Piece Movement**: All six piece types (Pawn, Knight, Bishop, Rook, Queen, King) with accurate movement rules
- **Move Validation**: Prevents illegal moves and ensures players cannot move into check
- **Turn-Based System**: Alternating turns between White and Black
- **Coordinate-Based Input**: Standard algebraic notation for easy move entry (e.g., `e2 e4`)

### Special Moves

#### Castling
- **Kingside Castling**: King moves two squares toward the h-file rook
- **Queenside Castling**: King moves two squares toward the a-file rook
- **Conditions**: Neither piece has moved, squares between are empty, king not in check, king doesn't pass through check

#### En Passant
- Special pawn capture when an enemy pawn advances two squares from its starting position
- Automatically detected and validated by the game engine

#### Pawn Promotion
- Pawns reaching the opposite end of the board can promote to Queen, Rook, Bishop, or Knight
- Interactive promotion selection during gameplay

### Game End Conditions

#### Win Conditions
- **Checkmate**: King is in check with no legal moves available
- Automatic detection with winner announcement

#### Draw Conditions
- **Stalemate**: Player has no legal moves but is not in check
- **Threefold Repetition**: Same board position occurs three times
- **50-Move Rule**: 50 consecutive moves without pawn movement or captures
- **Insufficient Material**: Neither player has enough pieces to checkmate
  - King vs King
  - King + Bishop vs King
  - King + Knight vs King
  - King + Bishop vs King + Bishop (same-colored bishops)
- **Draw by Agreement**: Players can offer and accept draws using the `draw` command

### Visual Features

- **Unicode Chess Pieces**: 
  - White: ♔ ♕ ♖ ♗ ♘ ♙
  - Black: ♚ ♛ ♜ ♝ ♞ ♙
- **Box-Drawing Board**: Clean visual representation with borders and grid
- **Board Coordinates**: Row numbers (1-8) and file letters (a-h) for easy reference
- **Turn Indicator**: Clear display of whose turn it is
- **Check Warning**: Notifies players when their king is in check

---

## Usage

### Starting the Game

Run the compiled executable to start a new game. The board will display in its initial position with White's turn first.

### Making Moves

Moves are entered using standard algebraic notation coordinates:

```
Format: [source] [destination]
Example: e2 e4
```

**Examples:**
- `e2 e4` - Move pawn from e2 to e4
- `g1 f3` - Move knight from g1 to f3
- `e1 g1` - Kingside castling (White)
- `e7 e5` - Move pawn from e7 to e5

### Special Commands

- **Offer Draw**: Type `draw` for either coordinate to offer a draw
- The opponent will be prompted to accept (y) or decline (n)

### Pawn Promotion

When a pawn reaches the opposite end:
1. You'll be prompted: `Promote to (Q/R/B/N):`
2. Enter your choice:
   - `Q` - Queen (recommended)
   - `R` - Rook
   - `B` - Bishop
   - `N` - Knight

---

## Gameplay Rules

### Piece Movement

| Piece | Movement Pattern |
|-------|-----------------|
| **Pawn** | Moves forward one square (two from starting position). Captures diagonally. |
| **Knight** | Moves in an L-shape: 2 squares in one direction, 1 square perpendicular. Can jump over pieces. |
| **Bishop** | Moves diagonally any number of squares. |
| **Rook** | Moves horizontally or vertically any number of squares. |
| **Queen** | Combines rook and bishop movement. |
| **King** | Moves one square in any direction. |

### Check and Checkmate

- **Check**: Your king is under attack. You must move out of check.
- **Checkmate**: King is in check with no legal moves. Game over.
- **Illegal Moves**: You cannot make a move that puts or leaves your king in check.

---

## Advanced Chess Rules

### Castling Requirements

Both kingside and queenside castling are available if:
1. ✓ King has never moved
2. ✓ Chosen rook has never moved
3. ✓ No pieces between king and rook
4. ✓ King is not in check
5. ✓ King doesn't pass through an attacked square
6. ✓ King's destination square is not attacked

### En Passant

- Available when an enemy pawn moves two squares forward from its starting position
- Your pawn must be on the 5th rank (White) or 4th rank (Black)
- Can capture the enemy pawn "in passing" on the very next move only
- The game automatically tracks and validates en passant opportunities

### Threefold Repetition

- Automatically tracked throughout the game
- Draw is declared if the exact same position occurs three times
- Position includes piece placement, castling rights, and en passant status

### 50-Move Rule

- Draw declared if 50 consecutive moves occur without:
  - Any pawn movement
  - Any piece capture
- Counter automatically maintained by the game engine

### Insufficient Material

Draw is automatically declared when neither player can checkmate:
- King vs King
- King + Bishop vs King
- King + Knight vs King  
- King + Bishop vs King + Bishop (bishops on same color)

---

## Technical Details

### Architecture

The game is implemented as a single-file C++ program with clean separation of concerns:

- **Game State Management**: Global variables track board state, castling rights, en passant
- **Board Representation**: 8x8 character array with standard notation
- **Move Validation**: Comprehensive legal move checking
- **Attack Detection**: Efficient square attack calculation for check detection
- **State History**: Move history tracking for repetition detection

### Character Encoding

The program uses **UTF-8 encoding** for proper display of Unicode characters:
- **Windows**: Automatically sets console code page to UTF-8
- **Linux/macOS**: Uses standard locale settings (UTF-8 typically default)

### Piece Representation

- **White pieces**: Lowercase letters (`p`, `r`, `n`, `b`, `q`, `k`)
- **Black pieces**: Uppercase letters (`P`, `R`, `N`, `B`, `Q`, `K`)
- **Empty squares**: Period character (`.`)

### Board Coordinates

- **Rows**: 0-7 internally (displayed as 8-1)
- **Columns**: 0-7 internally (displayed as a-h)
- **Convention**: Row 0 = 8th rank (Black's starting position)

---

## Code Structure

### Key Functions

```cpp
void initBoard()                    // Initialize starting position
void printBoard()                   // Display the board
bool isValidMove(...)               // Validate move legality
void makeMove(...)                  // Execute a move
bool isInCheck(bool white)          // Check if king is in check
bool hasLegalMoves(bool white)      // Detect checkmate/stalemate
bool isSquareAttacked(...)          // Attack detection
bool isThreefoldRepetition()        // Repetition detection
bool isInsufficientMaterial()       // Material-based draw
```

### Global State Variables

- `board[8][8]` - Current board position
- `whiteTurn` - Active player
- `whiteKingRow/Col`, `blackKingRow/Col` - King positions
- `whiteKingMoved`, `blackKingMoved` - Castling tracking
- `whiteRookLeftMoved`, etc. - Rook movement tracking
- `enPassantCol`, `enPassantRow` - En passant opportunity
- `movesSinceCaptureOrPawn` - 50-move rule counter
- `moveHistory[]` - Position history for repetition

---

## Platform Compatibility

### Cross-Platform Design

The code uses conditional compilation for platform-specific features:

```cpp
#ifdef _WIN32
    // Windows-specific code
    SetConsoleOutputCP(CP_UTF8);
#else
    // Linux/macOS code
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
```

---

**Enjoy your game of chess! ♔♕♖♗♘♙**
