#ifndef CHECKER_BOARD_HPP
#define CHECKER_BOARD_HPP

#include "figure.hpp"

class CheckerBoard {
	Figure **board{nullptr};
	bool whiteInDown{true};

public:
	CheckerBoard(bool isWhiteInDown);
	~CheckerBoard();

	Figure **getBoard() const { return board; };
	bool isWhiteInDown() const { return whiteInDown; }

	// friend std::ostream& operator<<(std::ostream &out, CheckerBoard checker);
};

#endif // CHECKER_BOARD_HPP
