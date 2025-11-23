#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>
#include "checker_board.hpp"

using namespace std;

class Interface {
	string colorBorder{"\033[40m"};
	string colorWhite{"\033[097m"};
	string colorBlack{"\033[091m"};
	const string colorEnd{"\033[0m"};

	string symbolBorder = "|";
	string symbolPiece = "◯";
	string symbolKing = "⬤";

public:
	void fillBoardFromFile(CheckerBoard &checker, std::string fileName);
	void drowBoard(CheckerBoard &checker);
};

#endif // INTERFACE_HPP
