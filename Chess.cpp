#include <iostream>
#include <string>

// Platform-specific includes for UTF-8 console support
#ifdef _WIN32
    #include <windows.h>
#else
    #include <locale>
#endif

using namespace std;

// Global variables
char board[8][8];
bool whiteTurn = true;
int whiteKingRow = 7, whiteKingCol = 4;
int blackKingRow = 0, blackKingCol = 4;
bool whiteKingMoved = false, blackKingMoved = false;
bool whiteRookLeftMoved = false, whiteRookRightMoved = false;
bool blackRookLeftMoved = false, blackRookRightMoved = false;
int enPassantCol = -1;
int enPassantRow = -1;  // Store row for en passant
int moveCount = 0;
int movesSinceCaptureOrPawn = 0;
string moveHistory[1000];  // For threefold repetition
int historyIndex = 0;

void initBoard() {
    // Black pieces (uppercase)
    board[0][0] = 'R'; board[0][1] = 'N'; board[0][2] = 'B'; board[0][3] = 'Q';
    board[0][4] = 'K'; board[0][5] = 'B'; board[0][6] = 'N'; board[0][7] = 'R';
    for(int i = 0; i < 8; i++) board[1][i] = 'P';
    
    // Empty squares
    for(int i = 2; i < 6; i++)
        for(int j = 0; j < 8; j++)
            board[i][j] = '.';
    
    // White pieces (lowercase)
    for(int i = 0; i < 8; i++) board[6][i] = 'p';
    board[7][0] = 'r'; board[7][1] = 'n'; board[7][2] = 'b'; board[7][3] = 'q';
    board[7][4] = 'k'; board[7][5] = 'b'; board[7][6] = 'n'; board[7][7] = 'r';
}

void printBoard() {
    cout << "\n  ╔════╦════╦════╦════╦════╦════╦════╦════╗\n";
    for(int i = 0; i < 8; i++) {
        cout << (8-i) << " ║";
        for(int j = 0; j < 8; j++) {
            char piece = board[i][j];
            
            // Fixed-width display
            switch(piece) {
                case 'K': cout << " ♚ "; break;
                case 'Q': cout << " ♛ "; break;
                case 'R': cout << " ♜ "; break;
                case 'B': cout << " ♝ "; break;
                case 'N': cout << " ♞ "; break;
                case 'P': cout << " ♟ "; break;
                case 'k': cout << " ♔ "; break;
                case 'q': cout << " ♕ "; break;
                case 'r': cout << " ♖ "; break;
                case 'b': cout << " ♗ "; break;
                case 'n': cout << " ♘ "; break;
                case 'p': cout << " ♙ "; break;
                default:  cout << "   "; break;
            }
            
            if(j < 7) cout << " ║";
        }
        cout << " ║ " << "\n";
        
        if(i < 7) cout << "  ╠════╬════╬════╬════╬════╬════╬════╬════╣\n";
    }
    cout << "  ╚════╩════╩════╩════╩════╩════╩════╩════╝\n";
    cout << "    a    b    c    d    e    f    g    h\n\n";
}

bool isWhite(char piece) {
    return piece >= 'a' && piece <= 'z';
}

bool isBlack(char piece) {
    return piece >= 'A' && piece <= 'Z';
}

bool isValidSquare(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

bool isSquareAttacked(int row, int col, bool byWhite) {
    // Check all pieces of attacking color
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            char piece = board[i][j];
            if(piece == '.') continue;
            if(byWhite && !isWhite(piece)) continue;
            if(!byWhite && !isBlack(piece)) continue;
            
            char p = (piece >= 'a' && piece <= 'z') ? piece - 32 : piece;
            
            // Pawn attacks
            if(p == 'P') {
                int dir = byWhite ? -1 : 1;
                if(i + dir == row && (j - 1 == col || j + 1 == col)) return true;
            }
            // Knight attacks
            else if(p == 'N') {
                int dr[] = {-2,-2,-1,-1,1,1,2,2};
                int dc[] = {-1,1,-2,2,-2,2,-1,1};
                for(int k = 0; k < 8; k++) {
                    if(isValidSquare(i + dr[k], j + dc[k]) && 
                       i + dr[k] == row && j + dc[k] == col) return true;
                }
            }
            // King attacks
            else if(p == 'K') {
                if(abs(i - row) <= 1 && abs(j - col) <= 1) return true;
            }
            // Bishop/Queen diagonal
            else if(p == 'B' || p == 'Q') {
                int dr[] = {-1,-1,1,1};
                int dc[] = {-1,1,-1,1};
                for(int k = 0; k < 4; k++) {
                    int r = i + dr[k], c = j + dc[k];
                    while(isValidSquare(r, c)) {
                        if(r == row && c == col) return true;
                        if(board[r][c] != '.') break;
                        r += dr[k]; c += dc[k];
                    }
                }
            }
            // Rook/Queen straight
            if(p == 'R' || p == 'Q') {
                int dr[] = {-1,1,0,0};
                int dc[] = {0,0,-1,1};
                for(int k = 0; k < 4; k++) {
                    int r = i + dr[k], c = j + dc[k];
                    while(isValidSquare(r, c)) {
                        if(r == row && c == col) return true;
                        if(board[r][c] != '.') break;
                        r += dr[k]; c += dc[k];
                    }
                }
            }
        }
    }
    return false;
}

bool isInCheck(bool white) {
    int kingRow = white ? whiteKingRow : blackKingRow;
    int kingCol = white ? whiteKingCol : blackKingCol;
    return isSquareAttacked(kingRow, kingCol, !white);
}

bool wouldBeInCheck(int fromRow, int fromCol, int toRow, int toCol, bool white) {
    char tempFrom = board[fromRow][fromCol];
    char tempTo = board[toRow][toCol];
    
    // Temporarily move piece
    board[toRow][toCol] = tempFrom;
    board[fromRow][fromCol] = '.';
    
    // Update king position only if moving king
    if(tempFrom == 'k' || tempFrom == 'K') {
        if(white) { 
            whiteKingRow = toRow; 
            whiteKingCol = toCol; 
        } else { 
            blackKingRow = toRow; 
            blackKingCol = toCol; 
        }
    }
    
    // Check if in check
    bool inCheck = isInCheck(white);
    
    // Restore everything
    board[fromRow][fromCol] = tempFrom;
    board[toRow][toCol] = tempTo;
    
    // Restore king position only if it was a king
    if(tempFrom == 'k' || tempFrom == 'K') {
        if(white) { 
            whiteKingRow = fromRow; 
            whiteKingCol = fromCol; 
        } else { 
            blackKingRow = fromRow; 
            blackKingCol = fromCol; 
        }
    }
    
    return inCheck;
}

bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) {
    if(!isValidSquare(fromRow, fromCol) || !isValidSquare(toRow, toCol)) return false;
    if(fromRow == toRow && fromCol == toCol) return false;
    
    char piece = board[fromRow][fromCol];
    if(piece == '.') return false;
    
    bool pieceIsWhite = isWhite(piece);
    if(whiteTurn != pieceIsWhite) return false;
    
    char target = board[toRow][toCol];
    if(target != '.' && pieceIsWhite == isWhite(target)) return false;
    
    char p = (piece >= 'a' && piece <= 'z') ? piece - 32 : piece;
    
    // Pawn moves
    if(p == 'P') {
        int dir = pieceIsWhite ? -1 : 1;
        int startRow = pieceIsWhite ? 6 : 1;
        
        // Forward move
        if(fromCol == toCol && target == '.') {
            if(toRow == fromRow + dir) 
                return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
            if(fromRow == startRow && toRow == fromRow + 2*dir && 
               board[fromRow + dir][fromCol] == '.')
                return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
        }
        // Capture
        if(abs(fromCol - toCol) == 1 && toRow == fromRow + dir) {
            // Normal capture
            if(target != '.' && pieceIsWhite != isWhite(target))
                return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
            // En passant
            if(toCol == enPassantCol && toRow == enPassantRow) {
                // The captured pawn is behind the destination square
                int capturedRow = pieceIsWhite ? toRow + 1 : toRow - 1;
                char captured = board[capturedRow][toCol];
                if((pieceIsWhite && captured == 'P') || (!pieceIsWhite && captured == 'p'))
                    return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
            }
        }
        return false;
    }
    
    // Knight moves
    if(p == 'N') {
        int dr = abs(fromRow - toRow);
        int dc = abs(fromCol - toCol);
        if((dr == 2 && dc == 1) || (dr == 1 && dc == 2))
            return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
        return false;
    }
    
    // King moves
    if(p == 'K') {
        // Normal king move
        if(abs(fromRow - toRow) <= 1 && abs(fromCol - toCol) <= 1)
            return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
        
        // Castling
        if(fromRow == toRow && abs(fromCol - toCol) == 2) {
            bool kingMoved = pieceIsWhite ? whiteKingMoved : blackKingMoved;
            if(kingMoved || isInCheck(pieceIsWhite)) return false;
            
            if(toCol == 6) { // Kingside
                bool rookMoved = pieceIsWhite ? whiteRookRightMoved : blackRookRightMoved;
                if(rookMoved) return false;
                if(board[fromRow][5] != '.' || board[fromRow][6] != '.') return false;
                if(isSquareAttacked(fromRow, 4, !pieceIsWhite) ||  // King's original square
                   isSquareAttacked(fromRow, 5, !pieceIsWhite) ||  // Square king moves through
                   isSquareAttacked(fromRow, 6, !pieceIsWhite))    // Square king moves to
                    return false;
                return true;
            } else if(toCol == 2) { // Queenside
                bool rookMoved = pieceIsWhite ? whiteRookLeftMoved : blackRookLeftMoved;
                if(rookMoved) return false;
                // Only squares between king and rook must be empty
                if(board[fromRow][1] != '.' || board[fromRow][2] != '.' || board[fromRow][3] != '.') 
                    return false;
                if(isSquareAttacked(fromRow, 4, !pieceIsWhite) ||  // King's original square
                   isSquareAttacked(fromRow, 3, !pieceIsWhite) ||  // Square king moves through
                   isSquareAttacked(fromRow, 2, !pieceIsWhite))    // Square king moves to
                    return false;
                return true;
            }
        }
        return false;
    }
    
    // Bishop moves
    if(p == 'B') {
        if(abs(fromRow - toRow) != abs(fromCol - toCol)) return false;
        int dr = (toRow > fromRow) ? 1 : -1;
        int dc = (toCol > fromCol) ? 1 : -1;
        int r = fromRow + dr, c = fromCol + dc;
        while(r != toRow || c != toCol) {
            if(board[r][c] != '.') return false;
            r += dr; c += dc;
        }
        return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
    }
    
    // Rook moves
    if(p == 'R') {
        if(fromRow != toRow && fromCol != toCol) return false;
        int dr = (fromRow == toRow) ? 0 : ((toRow > fromRow) ? 1 : -1);
        int dc = (fromCol == toCol) ? 0 : ((toCol > fromCol) ? 1 : -1);
        int r = fromRow + dr, c = fromCol + dc;
        while(r != toRow || c != toCol) {
            if(board[r][c] != '.') return false;
            r += dr; c += dc;
        }
        return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
    }
    
    // Queen moves
    if(p == 'Q') {
        bool straight = (fromRow == toRow || fromCol == toCol);
        bool diagonal = (abs(fromRow - toRow) == abs(fromCol - toCol));
        if(!straight && !diagonal) return false;
        
        int dr = (fromRow == toRow) ? 0 : ((toRow > fromRow) ? 1 : -1);
        int dc = (fromCol == toCol) ? 0 : ((toCol > fromCol) ? 1 : -1);
        int r = fromRow + dr, c = fromCol + dc;
        while(r != toRow || c != toCol) {
            if(board[r][c] != '.') return false;
            r += dr; c += dc;
        }
        return !wouldBeInCheck(fromRow, fromCol, toRow, toCol, pieceIsWhite);
    }
    
    return false;
}

bool hasLegalMoves(bool white) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            char piece = board[i][j];
            if(piece == '.') continue;
            if(white != isWhite(piece)) continue;
            
            for(int ti = 0; ti < 8; ti++) {
                for(int tj = 0; tj < 8; tj++) {
                    if(isValidMove(i, j, ti, tj)) return true;
                }
            }
        }
    }
    return false;
}

string getBoardState() {
    string state;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            state += board[i][j];
        }
    }
    state += whiteTurn ? 'W' : 'B';
    state += (whiteKingMoved ? '1' : '0');
    state += (blackKingMoved ? '1' : '0');
    state += (whiteRookLeftMoved ? '1' : '0');
    state += (whiteRookRightMoved ? '1' : '0');
    state += (blackRookLeftMoved ? '1' : '0');
    state += (blackRookRightMoved ? '1' : '0');
    return state;
}

bool isThreefoldRepetition() {
    string currentState = getBoardState();
    int count = 0;
    for(int i = 0; i < historyIndex; i++) {
        if(moveHistory[i] == currentState) {
            count++;
            if(count >= 2) return true;
        }
    }
    return false;
}

bool isInsufficientMaterial() {
    int whitePieces = 0, blackPieces = 0;
    int whiteBishops = 0, blackBishops = 0;
    int whiteKnights = 0, blackKnights = 0;
    char whiteBishopColor = -1, blackBishopColor = -1;
    
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            char piece = board[i][j];
            if(piece == '.') continue;
            
            char p = (piece >= 'a' && piece <= 'z') ? piece - 32 : piece;
            bool isPieceWhite = isWhite(piece);
            
            if(p == 'Q' || p == 'R' || p == 'P') {
                // Any queen, rook, or pawn means sufficient material
                return false;
            }
            
            if(p == 'K') continue;  // Kings don't count
            
            if(isPieceWhite) {
                whitePieces++;
                if(p == 'B') {
                    whiteBishops++;
                    // Check bishop color (light or dark square)
                    if((i + j) % 2 == 0) whiteBishopColor = 0;  // Dark
                    else whiteBishopColor = 1;  // Light
                } else if(p == 'N') {
                    whiteKnights++;
                }
            } else {
                blackPieces++;
                if(p == 'B') {
                    blackBishops++;
                    if((i + j) % 2 == 0) blackBishopColor = 0;
                    else blackBishopColor = 1;
                } else if(p == 'N') {
                    blackKnights++;
                }
            }
        }
    }
    
    // King vs King
    if(whitePieces == 0 && blackPieces == 0) return true;
    
    // King + Bishop vs King
    if((whitePieces == 1 && whiteBishops == 1 && blackPieces == 0) ||
       (blackPieces == 1 && blackBishops == 1 && whitePieces == 0)) return true;
    
    // King + Knight vs King
    if((whitePieces == 1 && whiteKnights == 1 && blackPieces == 0) ||
       (blackPieces == 1 && blackKnights == 1 && whitePieces == 0)) return true;
    
    // King + Bishop vs King + Bishop with bishops on same color
    if(whitePieces == 1 && whiteBishops == 1 &&
       blackPieces == 1 && blackBishops == 1 &&
       whiteBishopColor == blackBishopColor) return true;
    
    return false;
}

void makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    char piece = board[fromRow][fromCol];
    char p = (piece >= 'a' && piece <= 'z') ? piece - 32 : piece;
    bool pieceIsWhite = isWhite(piece);
    
    // Save move for history
    moveHistory[historyIndex++] = getBoardState();
    
    // Handle en passant capture
    if(p == 'P' && toCol == enPassantCol && toRow == enPassantRow && 
        abs(fromCol - toCol) == 1 && board[toRow][toCol] == '.') {
        int capturedRow = pieceIsWhite ? toRow + 1 : toRow - 1;
        // Add validation that captured square has enemy pawn
        char captured = board[capturedRow][toCol];
        if((pieceIsWhite && captured == 'P') || (!pieceIsWhite && captured == 'p')) {
            board[capturedRow][toCol] = '.';
            movesSinceCaptureOrPawn = 0;
        }
    }
    
    // Update 50-move rule counter
    if(p == 'P' || board[toRow][toCol] != '.') {
        movesSinceCaptureOrPawn = 0;
    } else {
        movesSinceCaptureOrPawn++;
    }
    
    // Reset en passant
    enPassantCol = -1;
    enPassantRow = -1;
    
    // Set en passant for double pawn move
    if(p == 'P' && abs(fromRow - toRow) == 2) {
        enPassantCol = fromCol;
        enPassantRow = (fromRow + toRow) / 2;  // Middle square
    }
    
    // Handle castling
    if(p == 'K' && abs(fromCol - toCol) == 2) {
        if(toCol == 6) { // Kingside
            board[fromRow][5] = board[fromRow][7];
            board[fromRow][7] = '.';
            // Update rook moved flag
            if(pieceIsWhite) whiteRookRightMoved = true;
            else blackRookRightMoved = true;
        } else { // Queenside
            board[fromRow][3] = board[fromRow][0];
            board[fromRow][0] = '.';
            // Update rook moved flag
            if(pieceIsWhite) whiteRookLeftMoved = true;
            else blackRookLeftMoved = true;
        }
    }
    
    // Move piece
    char captured = board[toRow][toCol];
    board[toRow][toCol] = piece;
    board[fromRow][fromCol] = '.';
    
    // Update king position
    if(p == 'K') {
        if(pieceIsWhite) {
            whiteKingRow = toRow;
            whiteKingCol = toCol;
            whiteKingMoved = true;
        } else {
            blackKingRow = toRow;
            blackKingCol = toCol;
            blackKingMoved = true;
        }
    }
    
    // Update rook moved flags (when rook moves from starting position)
    if(p == 'R') {
        if(pieceIsWhite) {
            if(fromRow == 7 && fromCol == 0) whiteRookLeftMoved = true;
            if(fromRow == 7 && fromCol == 7) whiteRookRightMoved = true;
        } else {
            if(fromRow == 0 && fromCol == 0) blackRookLeftMoved = true;
            if(fromRow == 0 && fromCol == 7) blackRookRightMoved = true;
        }
    }
    
    // Update rook moved flags (when rook is captured)
    if(captured == 'R' || captured == 'r') {
        if(captured == 'r') {
            if(toRow == 7 && toCol == 0) whiteRookLeftMoved = true;
            if(toRow == 7 && toCol == 7) whiteRookRightMoved = true;
        } else {
            if(toRow == 0 && toCol == 0) blackRookLeftMoved = true;
            if(toRow == 0 && toCol == 7) blackRookRightMoved = true;
        }
    }
    
    // Pawn promotion
    if(p == 'P' && (toRow == 0 || toRow == 7)) {
        char promo;
        bool validPromo = false;
        
        do {
            cout << "Promote to (Q/R/B/N): ";
            cin >> promo;
            // Clear input buffer
            while(cin.get() != '\n');
            
            if(promo >= 'a' && promo <= 'z') promo -= 32;
            
            if(promo == 'Q' || promo == 'R' || promo == 'B' || promo == 'N') {
                validPromo = true;
            } else {
                cout << "Invalid choice! Please enter Q, R, B, or N.\n";
            }
        } while(!validPromo);
        
        board[toRow][toCol] = pieceIsWhite ? (promo + 32) : promo;
        movesSinceCaptureOrPawn = 0;  // Promotion is a pawn move
    }
    
    moveCount++;
    whiteTurn = !whiteTurn;
}

int main() {
    // Platform-specific setup for UTF-8 encoding
    #ifdef _WIN32
        // Windows: Set console code page to UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #else
        // Linux/Unix: Set locale to UTF-8 (usually default, but ensures compatibility)
        setlocale(LC_ALL, "en_US.UTF-8");
    #endif
    
    initBoard();
    
    cout << "════════════════════════════════════════\n";
    cout << "       CONSOLE CHESS GAME\n";
    cout << "════════════════════════════════════════\n";
    cout << "White: ♔ ♕ ♖ ♗ ♘ ♙  Black: ♚ ♛ ♜ ♝ ♞ ♟\n";
    cout << "Enter moves as: e2 e4\n";
    cout << "════════════════════════════════════════\n";
    
    while(true) {
        printBoard();
        
        // Check for draws first
        if(isThreefoldRepetition()) {
            cout << "Draw by threefold repetition!\n";
            break;
        }
        
        if(movesSinceCaptureOrPawn >= 100) {  // 50 moves each = 100 half-moves
            cout << "Draw by 50-move rule!\n";
            break;
        }
        
        if(isInsufficientMaterial()) {
            cout << "Draw by insufficient material!\n";
            break;
        }
        
        if(isInCheck(whiteTurn)) {
            if(!hasLegalMoves(whiteTurn)) {
                cout << "CHECKMATE! " << (whiteTurn ? "Black" : "White") << " wins!\n";
                break;
            }
            cout << (whiteTurn ? "White" : "Black") << " is in CHECK!\n";
        } else if(!hasLegalMoves(whiteTurn)) {
            cout << "STALEMATE! It's a draw!\n";
            break;
        }
        
        cout << (whiteTurn ? "White" : "Black") << "'s turn: ";
        string from, to;
        cin >> from >> to;
        
        // Check for draw offer
        if(from == "draw" || to == "draw") {
            cout << "Draw offered. Accept? (y/n): ";
            char response;
            cin >> response;
            if(response == 'y' || response == 'Y') {
                cout << "Game drawn by agreement!\n";
                break;
            }
            continue;
        }
        
        if(from.length() != 2 || to.length() != 2) {
            cout << "Invalid input! Use format: e2 e4\n";
            continue;
        }
        
        int fromCol = from[0] - 'a';
        int fromRow = 8 - (from[1] - '0');
        int toCol = to[0] - 'a';
        int toRow = 8 - (to[1] - '0');
        
        // Validate coordinates
        if(fromRow < 0 || fromRow > 7 || fromCol < 0 || fromCol > 7 || 
           toRow < 0 || toRow > 7 || toCol < 0 || toCol > 7) {
            cout << "Invalid coordinates! Use a-h and 1-8.\n";
            continue;
        }
        
        if(isValidMove(fromRow, fromCol, toRow, toCol)) {
            makeMove(fromRow, fromCol, toRow, toCol);
        } else {
            cout << "Illegal move! Try again.\n";
        }
    }
    
    cout << "\nGame over after " << moveCount << " moves.\n";
    cout << "Final position:\n";
    printBoard();
    
    return 0;
}
