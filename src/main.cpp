#include <iostream>
#include "utils.hpp"
#include "checker_board.hpp"
#include "interface.hpp"

int main(void) {
	CheckerBoard checkerBoard;
	Interface interface(checkerBoard);

	try {
		CLS();
		interface.setWhiteSide();
		interface.fillBoardFromFile("./figures.txt");
		interface.showMenu();
	} catch (const std::string err) {
		std::cerr << std::endl << err;
	} catch (const char* err) {
		std::cerr << std::endl << err;
	}
	return 0;
}