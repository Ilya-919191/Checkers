#ifndef CHECKER_BOARD_HPP
#define CHECKER_BOARD_HPP

#include "figure.hpp"
#include "board.hpp"

class CheckerBoard {
	Figure **figures{nullptr};
	Board **board{nullptr};
	bool whiteInDown{true};

public:
	CheckerBoard(bool isWhiteInDown);
	~CheckerBoard();

	Figure **getFigures() const { return figures; };
	Board **getBoard() const { return board; }
	void transformPosition(std::string position, int &col, int &row);
	bool isWhiteInDown() const { return whiteInDown; }

	// friend std::ostream& operator<<(std::ostream &out, CheckerBoard checker);
};

#endif // CHECKER_BOARD_HPP
