#include "checker_board.hpp"
#include "figure.hpp"

CheckerBoard::CheckerBoard(bool isWhiteInDown) : whiteInDown(isWhiteInDown)
{
	board = new Figure*[8];
	for (int i = 0; i < 8; i++) {
		board[i] = new Figure[8];
	}
}

CheckerBoard::~CheckerBoard()
{
	for (int i = 0; i < 8; i++) {
		delete[] board[i];
	}
	delete[] board;
}

// std::ostream& operator<<(std::ostream out, CheckerBoard checker) {

// }
