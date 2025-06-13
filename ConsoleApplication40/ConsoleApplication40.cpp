#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

enum class PlayerType { USER, BOT };
enum class PieceColor { WHITE, BLACK };
enum class PieceType { MAN, KING };

class Piece {
public:
    PieceColor color;
    PieceType type;

    Piece(PieceColor color) : color(color), type(PieceType::MAN) {}

    void promote() {
        type = PieceType::KING;
    }
};

class Cell {
public:
    int row;
    int col;
    shared_ptr<Piece> piece;

    Cell(int r, int c) : row(r), col(c), piece(nullptr) {}

    bool isEmpty() const {
        return piece == nullptr;
    }
};

class Board {
public:
    static const int SIZE = 8;
    vector<vector<Cell>> cells;

    Board() {
        for (int r = 0; r < SIZE; ++r) {
            vector<Cell> row;
            for (int c = 0; c < SIZE; ++c) {
                row.emplace_back(r, c);
            }
            cells.push_back(row);
        }
    }

    void initialize() {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                cells[r][c].piece = nullptr;
                if ((r + c) % 2 == 1) {
                    if (r < 3) cells[r][c].piece = make_shared<Piece>(PieceColor::BLACK);
                    else if (r > 4) cells[r][c].piece = make_shared<Piece>(PieceColor::WHITE);
                }
            }
        }
    }

    void print() const {
        cout << "  A B C D E F G H\n";
        for (int r = 0; r < SIZE; ++r) {
            cout << r + 1 << " ";
            for (int c = 0; c < SIZE; ++c) {
                if (cells[r][c].isEmpty()) cout << ". ";
                else {
                    auto p = cells[r][c].piece;
                    char ch = (p->color == PieceColor::WHITE) ? 'w' : 'b';
                    if (p->type == PieceType::KING) ch = toupper(ch);
                    cout << ch << " ";
                }
            }
            cout << endl;
        }
    }

    bool isInside(int r, int c) const {
        return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
    }

    bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
        if (!isInside(fromRow, fromCol) || !isInside(toRow, toCol)) return false;
        if (cells[fromRow][fromCol].isEmpty() || !cells[toRow][toCol].isEmpty()) return false;
        auto piece = cells[fromRow][fromCol].piece;
        int dir = (piece->color == PieceColor::WHITE) ? -1 : 1;

        int dr = toRow - fromRow;
        int dc = toCol - fromCol;

        if (abs(dr) == 1 && abs(dc) == 1 && (piece->type == PieceType::KING || dr == dir)) return true;

        if (abs(dr) == 2 && abs(dc) == 2) {
            int midR = (fromRow + toRow) / 2;
            int midC = (fromCol + toCol) / 2;
            if (!cells[midR][midC].isEmpty() && cells[midR][midC].piece->color != piece->color) return true;
        }
        return false;
    }

    void makeMove(int fromRow, int fromCol, int toRow, int toCol) {
        auto piece = cells[fromRow][fromCol].piece;
        cells[toRow][toCol].piece = piece;
        cells[fromRow][fromCol].piece = nullptr;

        if (abs(toRow - fromRow) == 2) {
            int midR = (fromRow + toRow) / 2;
            int midC = (fromCol + toCol) / 2;
            cells[midR][midC].piece = nullptr;
        }

        if ((piece->color == PieceColor::WHITE && toRow == 0) ||
            (piece->color == PieceColor::BLACK && toRow == SIZE - 1)) {
            piece->promote();
        }
    }
};

class Player {
public:
    PlayerType type;
    PieceColor color;

    Player(PlayerType type, PieceColor color) : type(type), color(color) {}

    void makeMove(Board& board) {
        while (true) {
            string from, to;
            cout << "Хід гравця " << ((color == PieceColor::WHITE) ? "(білі)" : "(чорні)") << ": ";
            cin >> from >> to;
            int fr = from[1] - '1';
            int fc = tolower(from[0]) - 'a';
            int tr = to[1] - '1';
            int tc = tolower(to[0]) - 'a';
            if (board.isMoveValid(fr, fc, tr, tc)) {
                board.makeMove(fr, fc, tr, tc);
                break;
            }
            else {
                cout << "Невірний хід. Спробуйте ще раз.\n";
            }
        }
    }
};

class Game {
private:
    Board board;
    Player player1;
    Player player2;
    Player* currentPlayer;

public:
    Game() : player1(PlayerType::USER, PieceColor::WHITE), player2(PlayerType::USER, PieceColor::BLACK) {
        currentPlayer = &player1;
        board.initialize();
    }

    void start() {
        while (!isGameOver()) {
            board.print();
            currentPlayer->makeMove(board);
            switchPlayer();
        }
        showWinner();
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
    }

    bool isGameOver() const {
        int whiteCount = 0, blackCount = 0;
        for (int r = 0; r < Board::SIZE; ++r) {
            for (int c = 0; c < Board::SIZE; ++c) {
                if (!board.cells[r][c].isEmpty()) {
                    if (board.cells[r][c].piece->color == PieceColor::WHITE) ++whiteCount;
                    else ++blackCount;
                }
            }
        }
        return whiteCount == 0 || blackCount == 0;
    }

    void showWinner() const {
        int whiteCount = 0, blackCount = 0;
        for (int r = 0; r < Board::SIZE; ++r) {
            for (int c = 0; c < Board::SIZE; ++c) {
                if (!board.cells[r][c].isEmpty()) {
                    if (board.cells[r][c].piece->color == PieceColor::WHITE) ++whiteCount;
                    else ++blackCount;
                }
            }
        }
        if (whiteCount > blackCount) cout << "Перемогли білі!\n";
        else cout << "Перемогли чорні!\n";
    }
};

int main() {
    system("chcp 1251 > nul");
    Game game;
    game.start();
    return 0;
}