#ifndef CHECKER_BOARD_HPP
#define CHECKER_BOARD_HPP

#include "figure.hpp"
#include "board.hpp"

class CheckerBoard {
	Figure **figures{nullptr};
	Board **board{nullptr};

public:
	CheckerBoard();
	~CheckerBoard();

	Figure **getFigures() const { return figures; };
	Board **getBoard() const { return board; }

	bool transformPosition(std::string pos, int col, int row) const;
};

#endif // CHECKER_BOARD_HPP
