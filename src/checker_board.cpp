#include <string>
#include "checker_board.hpp"
#include "figure.hpp"
#include "board.hpp"

CheckerBoard::CheckerBoard(bool isWhiteInDown) : whiteInDown(isWhiteInDown)
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

void CheckerBoard::transformPosition(std::string position, int &col, int &row) {
	if (position.length() != 2) {
		throw "The position of figures must consist of one letter and one number. Example: A6";
	}
	col = position[0] - 'A';
	row = position[1] - '1';
}

// std::ostream& operator<<(std::ostream out, CheckerBoard checker) {

// }
