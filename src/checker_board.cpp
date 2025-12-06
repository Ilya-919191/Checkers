#include <string>
#include "checker_board.hpp"
#include "figure.hpp"
#include "board.hpp"

CheckerBoard::CheckerBoard()
{
	figures = new Figure*[8];
	for (int i = 0; i < 8; i++) {
		figures[i] = new Figure[8];
	}

	board = new Board*[8];
	for (int i = 0; i < 8; i++) {
		board[i] = new Board[8];
	}
}

CheckerBoard::~CheckerBoard()
{
	for (int i = 0; i < 8; i++) {
		delete[] figures[i];
	}
	delete[] figures;

	for (int i = 0; i < 8; i++) {
		delete[] board[i];
	}
	delete[] board;
}

bool CheckerBoard::transformPosition(const std::string& pos, int& col, int& row) const {
    if (pos.length() < 2) return false;
    
    char colChar = std::toupper(pos[0]);
    if (colChar < 'A' || colChar > 'H') return false;
    
    char rowChar = pos[1];
    if (rowChar < '1' || rowChar > '8') return false;
    
    col = colChar - 'A';
    row = rowChar - '1';
    
    return true;
}