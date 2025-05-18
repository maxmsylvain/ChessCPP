#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>

class Coordinates {
public:
    Coordinates() : x(0), y(0) {}
    Coordinates(int xVal, int yVal) : x(xVal), y(yVal) {}

    int getX() const { return x; }
    int getY() const { return y; }

    void setX(int xVal) { x = xVal; }
    void setY(int yVal) { y = yVal; }

private:
    int x;
    int y;
};

struct Piece {
    std::string type;
    Coordinates coordinate;
    char symbol;
    bool isWhite;

    Piece(){}

    Piece(std::string t, int x, int y, char s, bool white) :
        type(t), coordinate(x, y), symbol(s), isWhite(white) {}
};

class Board {
public:
    Board() {
        initializeBoard();
        setupPieces();
    }

    void initializeBoard() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                boardArray[i][j] = '.';
            }
        }
    }

    void setupPieces() {
        pieces.clear();
        for (int i = 0; i < 8; i++) {
            pieces.push_back(Piece("Pawn", i, 1, 'P', true));
            pieces.push_back(Piece("Pawn", i, 6, 'p', false));
        }
        pieces.push_back(Piece("Rook", 0, 0, 'R', true));
        pieces.push_back(Piece("Rook", 7, 0, 'R', true));
        pieces.push_back(Piece("Rook", 0, 7, 'r', false));
        pieces.push_back(Piece("Rook", 7, 7, 'r', false));
        pieces.push_back(Piece("Knight", 1, 0, 'N', true));
        pieces.push_back(Piece("Knight", 6, 0, 'N', true));
        pieces.push_back(Piece("Knight", 1, 7, 'n', false));
        pieces.push_back(Piece("Knight", 6, 7, 'n', false));
        pieces.push_back(Piece("Bishop", 2, 0, 'B', true));
        pieces.push_back(Piece("Bishop", 5, 0, 'B', true));
        pieces.push_back(Piece("Bishop", 2, 7, 'b', false));
        pieces.push_back(Piece("Bishop", 5, 7, 'b', false));
        pieces.push_back(Piece("Queen", 3, 0, 'Q', true));
        pieces.push_back(Piece("Queen", 3, 7, 'q', false));
        pieces.push_back(Piece("King", 4, 0, 'K', true));
        pieces.push_back(Piece("King", 4, 7, 'k', false));

        updateBoardFromPieces();
    }

    void updateBoardFromPieces() {
        initializeBoard();
        for (const Piece& piece : pieces) {
            int x = piece.coordinate.getX();
            int y = piece.coordinate.getY();
            boardArray[y][x] = piece.symbol;
        }
    }

    void displayBoard() {
        std::cout << "  ________________"<< std::endl;
        for (int i = 7; i >= 0; i--) {
            std::cout << i+1 << "|";
            for (int j = 0; j < 8; j++) {
                std::cout << boardArray[i][j] << " ";
            }
            std::cout << "|";
            std::cout << std::endl;
        }
        std::cout << "  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾" << std::endl;
        std::cout << "  a b c d e f g h" << std::endl;
    }

    bool movePiece(int fromX, int fromY, int toX, int toY) {
        if (fromX < 0 || fromX > 7 || fromY < 0 || fromY > 7 ||
            toX < 0 || toX > 7 || toY < 0 || toY > 7) {
            return false;
        }

        int pieceIndex = -1;
        for (size_t i = 0; i < pieces.size(); i++) {
            if (pieces[i].coordinate.getX() == fromX && pieces[i].coordinate.getY() == fromY) {
                pieceIndex = i;
                break;
            }
        }

        if (pieceIndex == -1) {
            return false;
        }

        bool isWhiteTurn = whiteToMove;
        if (pieces[pieceIndex].isWhite != isWhiteTurn) {
            return false;
        }

        int captureIndex = -1;
        for (size_t i = 0; i < pieces.size(); i++) {
            if (pieces[i].coordinate.getX() == toX && pieces[i].coordinate.getY() == toY) {
                if (pieces[i].isWhite == pieces[pieceIndex].isWhite) {
                    return false;
                }
                captureIndex = i;
                break;
            }
        }

        if (!isValidMove(pieces[pieceIndex], toX, toY)) {
            return false;
        }

        std::vector<Piece> savedPieces = pieces;

        if (captureIndex != -1) {
            pieces.erase(pieces.begin() + captureIndex);
            if (captureIndex < pieceIndex) {
                pieceIndex--;
            }
        }
        pieces[pieceIndex].coordinate = Coordinates(toX, toY);

        if (isKingInCheck(pieces[pieceIndex].isWhite)) {
            pieces = savedPieces;
            return false;
        }

        if (pieces[pieceIndex].type == "Pawn") {
            if ((pieces[pieceIndex].isWhite && toY == 7) ||
                (!pieces[pieceIndex].isWhite && toY == 0)) {
                promotePawn(pieceIndex);
            }
        }

        updateBoardFromPieces();
        whiteToMove = !whiteToMove;

        bool opponentInCheck = isKingInCheck(!pieces[pieceIndex].isWhite);
        check = false;
        checkmate = false;

        if (opponentInCheck) {
            check = true;
            checkColor = !pieces[pieceIndex].isWhite;

            bool opponentInCheckmate = isCheckmate(!pieces[pieceIndex].isWhite);
            if (opponentInCheckmate) {
                checkmate = true;
                checkmateColor = !pieces[pieceIndex].isWhite;
                std::cout << "Checkmate detected!" << std::endl;

                std::string losingColor = checkmateColor ? "Black" : "White";
                std::string winningColor = checkmateColor ? "White" : "Black";
                
                std::cout << losingColor << " is in checkmate!" << std::endl;
                std::cout << winningColor << " wins!" << std::endl;
            }
        }

        return true;
    }

    bool isValidMove(const Piece& piece, int toX, int toY) {
        int fromX = piece.coordinate.getX();
        int fromY = piece.coordinate.getY();
        
        if (piece.type == "Pawn") {
            int direction;
            if (piece.isWhite) {
                direction = 1;
            } else {
                direction = -1;
            }
            
            if (fromX == toX && toY == fromY + direction && !hasPieceAt(toX, toY)) {
                return true;
            }
            if (fromX == toX && 
                ((piece.isWhite && fromY == 1 && toY == 3) || 
                 (!piece.isWhite && fromY == 6 && toY == 4)) &&
                !hasPieceAt(toX, toY) && !hasPieceAt(toX, fromY + direction)) {
                return true;
            }
            if ((toX == fromX + 1 || toX == fromX - 1) && toY == fromY + direction && hasPieceAt(toX, toY) && getPieceAt(toX,toY)->isWhite != piece.isWhite) {
                return true;
            }
            return false;
        }
        
        if (piece.type == "Rook") {
            return isValidRookMove(fromX, fromY, toX, toY);
        }
        
        if (piece.type == "Knight") {
            int dx = abs(toX - fromX);
            int dy = abs(toY - fromY);
            return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
        }
        
        if (piece.type == "Bishop") {
            return isValidBishopMove(fromX, fromY, toX, toY);
        }
        
        if (piece.type == "Queen") {
            return isValidRookMove(fromX, fromY, toX, toY) || isValidBishopMove(fromX, fromY, toX, toY);
        }
        
        if (piece.type == "King") {
            return abs(toX - fromX) <= 1 && abs(toY - fromY) <= 1;
        }
        
        return false;
    }

    bool isValidRookMove(int fromX, int fromY, int toX, int toY) {
        if (fromX != toX && fromY != toY) {
            return false;
        }
        
        if (fromX == toX) {
            int step = (fromY < toY) ? 1 : -1;
            for (int y = fromY + step; y != toY; y += step) {
                if (hasPieceAt(fromX, y)) {
                    return false;
                }
            }
        } else {
            int step = (fromX < toX) ? 1 : -1;
            for (int x = fromX + step; x != toX; x += step) {
                if (hasPieceAt(x, fromY)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool isValidBishopMove(int fromX, int fromY, int toX, int toY) {
        if (abs(toX - fromX) != abs(toY - fromY)) {
            return false;
        }
        
        int stepX = (fromX < toX) ? 1 : -1;
        int stepY = (fromY < toY) ? 1 : -1;
        
        int x = fromX + stepX;
        int y = fromY + stepY;
        
        while (x != toX) { 
            if (hasPieceAt(x, y)) {
                return false;
            }
            x += stepX;
            y += stepY;
        }
        return true;
    }

    bool hasPieceAt(int x, int y) {
        for (const Piece& piece : pieces) {
            if (piece.coordinate.getX() == x && piece.coordinate.getY() == y) {
                return true;
            }
        }
        return false;
    }

    Piece* getPieceAt(int x, int y) {
        for (Piece& piece : pieces) {
            if (piece.coordinate.getX() == x && piece.coordinate.getY() == y) {
                return &piece;
            }
        }
        return nullptr;
    }

    bool parseMove(const std::string& move) {
        if (move.length() != 4) {
            return false;
        }

        int fromX = move[0] - 'a';
        int fromY = move[1] - '1';
        int toX = move[2] - 'a';
        int toY = move[3] - '1';

        return movePiece(fromX, fromY, toX, toY);
    }

    bool isKingInCheck(bool isWhiteKing) {
        int kingX = -1;
        int kingY = -1;

        for (const Piece& piece : pieces) {
            if (piece.type == "King" && piece.isWhite == isWhiteKing) {
                kingX = piece.coordinate.getX();
                kingY = piece.coordinate.getY();
                break;
            }
        }

        if (kingX == -1 || kingY == -1) {
            return false;
        }

        for (const Piece& piece : pieces) {
            if (piece.isWhite != isWhiteKing) {
                if (isValidMove(piece, kingX, kingY)) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isCheckmate(bool isWhiteKing) {

        if (!isKingInCheck(isWhiteKing)) {
            return false;
        }
        
        std::cout << "King is in check, checking for possible escape moves..." << std::endl;

        std::vector<Piece> originalPieces = pieces; 
        
        for (size_t i = 0; i < originalPieces.size(); i++) {
            if (originalPieces[i].isWhite == isWhiteKing) {
                Piece& currentPiece = originalPieces[i];
                int fromX = currentPiece.coordinate.getX();
                int fromY = currentPiece.coordinate.getY();
                std::string pieceType = currentPiece.type;

                for (int toX = 0; toX < 8; toX++) {
                    for (int toY = 0; toY < 8; toY++) {
                        if (toX == fromX && toY == fromY) {
                            continue;
                        }

                        pieces = originalPieces;

                        int pieceIndex = -1;
                        for (size_t j = 0; j < pieces.size(); j++) {
                            if (pieces[j].coordinate.getX() == fromX && pieces[j].coordinate.getY() == fromY) {
                                pieceIndex = j;
                                break;
                            }
                        }
                        
                        if (pieceIndex == -1) continue;

                        if (!isValidMove(pieces[pieceIndex], toX, toY)) {
                            continue;
                        }

                        bool invalidCapture = false;
                        for (const Piece& p : pieces) {
                            if (p.coordinate.getX() == toX && p.coordinate.getY() == toY && 
                                p.isWhite == pieces[pieceIndex].isWhite) {
                                invalidCapture = true;
                                break;
                            }
                        }
                        
                        if (invalidCapture) continue;
                        int captureIndex = -1;
                        for (size_t j = 0; j < pieces.size(); j++) {
                            if (pieces[j].coordinate.getX() == toX && pieces[j].coordinate.getY() == toY) {
                                captureIndex = j;
                                break;
                            }
                        }
                        
                        std::vector<Piece> savedPieces = pieces;
                        
                        if (captureIndex != -1) {
                            pieces.erase(pieces.begin() + captureIndex);
                            if (captureIndex < pieceIndex) {
                                pieceIndex--;
                            }
                        }
                        
                        pieces[pieceIndex].coordinate = Coordinates(toX, toY);
                        bool stillInCheck = isKingInCheck(isWhiteKing);
                        char fromFile = 'a' + fromX;
                        char toFile = 'a' + toX;
                        std::cout << "Testing move: " << pieceType << " from " << fromFile << (fromY + 1) 
                                  << " to " << toFile << (toY + 1);
                        if (stillInCheck) {
                            std::cout << " - Still in check" << std::endl;
                        } else {
                            std::cout << " - Escapes check!" << std::endl;
                        }
                        if (!stillInCheck) {
                            std::cout << "Found escape move: " << pieceType << " from " << fromFile << (fromY + 1) 
                                      << " to " << toFile << (toY + 1) << std::endl;
                            pieces = originalPieces;
                            return false;
                        }
                        
                        pieces = savedPieces;
                    }
                }
            }
        }

        pieces = originalPieces;
        
        std::cout << "No escape moves found - it's checkmate!" << std::endl;
        return true;
    }

    bool isGameOver() const {
        return checkmate;
    }

    bool isCheck() const {
        return check;
    }

    bool getWhiteToMove() const {
        return whiteToMove;
    }

    void setWhiteToMove(bool white) {
        whiteToMove = white;
    }

    bool getCheckmateColor() const {
        return checkmateColor;
    }

private:
    char boardArray[8][8];
    std::vector<Piece> pieces;
    bool whiteToMove = true;
    bool check = false;
    bool checkmate = false;
    bool checkColor = false;
    bool checkmateColor = false;

    void promotePawn(int pieceIndex) {
        bool isWhite = pieces[pieceIndex].isWhite;
        char choice;
        std::cout << "Pawn promotion! Choose piece (Q=Queen, R=Rook, B=Bishop, N=Knight): ";
        while (true) {
            std::cin >> choice;
            choice = toupper(choice);
            if (choice == 'Q' || choice == 'R' || choice == 'B' || choice == 'N') {
                break;
            }
            std::cout << "Invalid choice. Please enter Q, R, B, or N: ";
        }

        int x = pieces[pieceIndex].coordinate.getX();
        int y = pieces[pieceIndex].coordinate.getY();
        char symbol = isWhite ? choice : tolower(choice);
        std::string type;

        switch(choice) {
            case 'Q':
                type = "Queen";
                break;
            case 'R':
                type = "Rook";
                break;
            case 'B':
                type = "Bishop";
                break;
            case 'N':
                type = "Knight";
                break;
        }

        pieces[pieceIndex] = Piece(type, x, y, symbol, isWhite);
    }
};

int main() {
    Board chessBoard;
    std::string move;
    bool whiteToMove = true;
    bool gameOver = false;

    chessBoard.displayBoard();

    while (!gameOver) {
        if (chessBoard.isCheck()) {
            std::string currentColor = whiteToMove ? "White" : "Black";
            std::cout << currentColor << " is in check!" << std::endl;
        }

        std::string currentColor = whiteToMove ? "White" : "Black";
        std::cout << currentColor << " to move (e.g., e2e4): ";
        std::cin >> move;

        if (move == "quit") {
            break;
        }

        chessBoard.setWhiteToMove(whiteToMove);

        if (chessBoard.parseMove(move)) {
            chessBoard.displayBoard();
            whiteToMove = !whiteToMove;
            if (chessBoard.isGameOver()) {
                gameOver = true;
                std::cout << "Game over. Thanks for playing!" << std::endl;
                break;
            }
        } else {
            std::cout << "Invalid move! Try again." << std::endl;
        }
    }

    return 0;
}