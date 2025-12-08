#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>
#include <utility>
#include <vector>
#include "board.hpp"
#include "checker_board.hpp"
#include "figure.hpp"

using namespace std;

enum class Direction {
	none,
  topRight,
  bottomRight,
  bottomLeft,
  topLeft
};

struct AttackPath {
	std::vector<std::pair<int, int>> path;
	std::vector<std::pair<int, int>> captured;
	int attackCount{0};
	
	void addPosition(int i, int j);
	void addCaptured(int i, int j);
	bool isAlreadyCaptured(int i, int j) const;
};

class Interface
{
	CheckerBoard& checker;
	bool whiteInDown{true};

	void clearTiles();
	pair<int, int> getNextPosByDir(int i, int j, Direction direction);
	Figure& getNextFigureByDir(int i, int j, Direction direction);
	Direction reversDir(Direction direction);
	void setBackgroudOfTile(int i, int j, TileMoment moment);
	int figureAnalisis(int i, int j, Direction direction, Figure& startFigure, bool isDrawing,
										bool isAttacking, int attackCount, AttackPath& currentPath, AttackPath& bestPath,
										vector<pair<int, int>>& visitedPositions);
public:
	Interface(CheckerBoard& checkerBoard);

	void setWhiteSide();
	void whereWhiteSide();
	void fillBoardFromFile(std::string fileName);
	void showPossibleMovesAndAttacks();
	void showThreatsToAllFigures();
	void drowBoard(string text = "");
	void showMenu();
};

#endif // INTERFACE_HPP
