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

	void clearTiles();
	pair<int, int> getNextPosByDir(int i, int j, Direction direction);
	Figure& getNextFigureByDir(int i, int j, Direction direction);
	void setBackgroudOfTile(int i, int j, TileMoment moment);
	Direction reversDir(Direction direction);
	bool checkFigureForAttack(int i, int j, Direction direction, Figure& startFigure);
	int figureAnalisis(int i, int j, Direction direction, Figure& startFigure, bool isDrawing,
		bool isAttacking, int attackCount, AttackPath& currentPath, AttackPath& bestPath, vector<pair<int, int>>& visitedPositions);
	bool canAttackTarget(int attackerRow, int attackerCol, int targetRow, int targetCol, Figure& attacker);
  void visualizeThreats(int targetRow, int targetCol, const vector<pair<string, pair<int, int>>>& threats);
  Direction findAttackDirection(int fromRow, int fromCol, int toRow, int toCol);
  void drawAttackLine(int fromRow, int fromCol, int toRow, int toCol, Direction dir);

public:
	Interface(CheckerBoard& checkerBoard);

	void fillBoardFromFile(std::string fileName);
	void showPossibleMovesAndAttacks();
	void drowBoard(string text = "");
	void showWhoCanAttackFigure();
	void showThreatsToFigure();
};

#endif // INTERFACE_HPP
