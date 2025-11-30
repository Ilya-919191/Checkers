#include <iostream>
#include "checker_board.hpp"
#include "checker_board.hpp"
#include "interface.hpp"

int main(void)
{
	CheckerBoard checkerBoard(true);
	Interface interface(checkerBoard);

	try {
		interface.fillBoardFromFile("./figures.txt");
		interface.drowBoard();
		interface.showPossiblyAttack();
	} catch (const std::string err) {
		std::cerr << err;
	}
	return 0;
}