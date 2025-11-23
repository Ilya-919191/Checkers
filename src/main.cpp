#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include "checker_board.hpp"
#include "checker_board.hpp"
#include "interface.hpp"

int main(void)
{
	Interface interface;
	CheckerBoard checkerBoard(true);

	system("clear");
	interface.drowBoard(checkerBoard);
	interface.fillBoardFromFile(checkerBoard, "./file.txt");
	interface.drowBoard(checkerBoard);

	return 0;
}