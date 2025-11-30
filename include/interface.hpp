#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>
#include <utility>
#include "board.hpp"
#include "checker_board.hpp"
#include "checker_element.hpp"
#include "figure.hpp"

using namespace std;

enum class Direction {
	current,
  topRight,
  bottomRight,
  bottomLeft,
  topLeft
};

class Interface {
	const string colorWhite{"\033[097m"};
	const string colorBlack{"\033[091m"};
	const string colorBackgroundWhite{"\033[0107m "};
	const string colorBackgroundBlack{"\033[040m "};
	const string colorAttack{"\033[101m "};
	const string colorActive{"\033[104m "};
	const string colorFinish{"\033[102m "};
	const string colorMove{"\033[103m "};
	const string colorEnd{"\033[0m"};

	const string symbolPiece = "P"; // ⬤ P
	const string symbolKing = "K"; // ◯ K

	CheckerBoard& checker;

	pair<int, int> getPosByDir(int i, int j, Direction direction);
	Figure& getFigureByDir(int i, int j, Direction direction);
	void setBackgroudOfTile(int i, int j, TileMoment moment);
	Direction reversDir(Direction direction);
	bool checkFigureForAttack(int i, int j, Direction direction, Figure& startFigure);
	int figureAnalisis(int i, int j, Direction direction, Figure& startFigure, bool isDrow, bool isAttacked = false, int sumAtk = 0);
public:
	Interface(CheckerBoard& checkerBoard);
	void fillBoardFromFile(std::string fileName);
	void drowBoard();
	void showPossiblyAttack();
};

#endif // INTERFACE_HPP
